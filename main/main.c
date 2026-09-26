/**
 * @file main.c
 * @brief ESP32-S3 智能手表 LVGL 应用
 */

#include <stdio.h>
#include <sys/time.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "esp_log.h"
#include "esp_err.h"
#include "esp_timer.h"
#include "lvgl.h"

#include "board_display.h"
#include "board_display_config.h"
#include "gui_guider.h"
#include "custom.h"
#include "ap_wifi.h"
#include "esp_sntp.h"
#include <time.h>
#include "nvs_flash.h"
#include "weather.h"
#include "esp_spiffs.h"
#include "max98357a.h"
#include "sensors.h"
#include "system_settings.h"
#include "watch_standby.h"
#include "alarm_clock.h"
#include "ai_chat/ai_chat_config.h"
#include "wifi_connect.h"
#include "battery_ui.h"
#include "sd_card_fs.h"
#include "sd_serial_transfer.h"
#include "voice_assistant.h"

static const char *TAG = "esp32_s3_watch";

lv_ui guider_ui;

// Forward declaration to ensure prototype is visible in this file
void set_home_time(lv_ui *ui, int year, int month, int day, int w_day, int hour, int min, int sec);

// 星期名称数组
static const char *weekday_names[] = {"日", "一", "二", "三", "四", "五", "六"};

// 实现 set_home_time：更新主界面时间、日期、星期
void set_home_time(lv_ui *ui, int year, int month, int day, int w_day, int hour, int min, int sec)
{
    if (!ui) return;
    
    // 更新数字时钟
    if (ui->screen_home_digital_clock && lv_obj_is_valid(ui->screen_home_digital_clock)) {
        lv_label_set_text_fmt(ui->screen_home_digital_clock, "%d:%02d:%02d", hour, min, sec);
    }
    
    // 更新日期
    if (ui->screen_home_label_data && lv_obj_is_valid(ui->screen_home_label_data)) {
        lv_label_set_text_fmt(ui->screen_home_label_data, "%04d/%02d/%02d", year, month, day);
    }
    
    // 更新星期
    if (ui->screen_home_label_week && lv_obj_is_valid(ui->screen_home_label_week)) {
        if (w_day >= 0 && w_day <= 6) {
            lv_label_set_text_fmt(ui->screen_home_label_week, "星期%s", weekday_names[w_day]);
        }
    }
}

// SNTP 时间同步数据
static struct {
    bool updated;
    int year;
    int month;
    int day;
    int wday;
    int hour;
    int min;
    int sec;
} sntp_time_data = {0};

static st7789v_t *g_lcd = NULL;
SemaphoreHandle_t lvgl_mutex = NULL;  // 全局变量，供其他模块使用
static void *buf1 = NULL;

// 音频输出句柄（供AI聊天等模块使用）
static max98357a_handle_t *g_audio_handle = NULL;

/**
 * @brief 获取音频输出句柄指针的指针
 * @return 音频句柄指针的地址
 */
max98357a_handle_t **get_audio_handle(void)
{
    return &g_audio_handle;
}

static void img_spiffs_init(void)
{
    //定义挂载点
    esp_vfs_spiffs_conf_t conf = {
        .base_path = "/img",            //挂载点
        .partition_label = "img",         //分区名称
        .max_files = 5,                    //最大打开的文件数
        .format_if_mount_failed = false    //挂载失败是否执行格式化
        };
    //挂载spiffs
    ESP_ERROR_CHECK(esp_vfs_spiffs_register(&conf));
}

/**
 * @brief LVGL 显示刷新回调
 */
static void lvgl_flush_cb(lv_display_t *disp, const lv_area_t *area, uint8_t *px_map)
{
    if (!g_lcd) {
        lv_display_flush_ready(disp);
        return;
    }

    size_t width = (size_t)(area->x2 - area->x1 + 1);
    size_t height = (size_t)(area->y2 - area->y1 + 1);
    size_t pixel_count = width * height;
    size_t total_bytes = pixel_count * 2;
    
    st7789v_set_window(g_lcd, (uint16_t)area->x1, (uint16_t)area->y1, 
                       (uint16_t)area->x2, (uint16_t)area->y2);
    
    uint16_t *pixels = (uint16_t *)px_map;
    for (size_t i = 0; i < pixel_count; i++) {
        pixels[i] = (pixels[i] >> 8) | (pixels[i] << 8);
    }

    // SPI 总线一次最多传 4096 字节；按块发送，避免每行都建立一次事务。
    const size_t spi_chunk_bytes = 4096;
    for (size_t offset = 0; offset < total_bytes; offset += spi_chunk_bytes) {
        size_t remaining = total_bytes - offset;
        size_t chunk_bytes = remaining < spi_chunk_bytes ? remaining : spi_chunk_bytes;
        st7789v_write_pixels(g_lcd, px_map + offset, chunk_bytes);
    }

    lv_display_flush_ready(disp);
}

/**
 * @brief LVGL 定时器回调
 */
static void lvgl_tick_cb(void *arg)
{
    lv_tick_inc(2);
}

/**
 * @brief LVGL 任务
 */
static void lvgl_task(void *pvParameters)
{
    uint32_t last_wifi_refresh_ms = 0;
    while (1) {
        uint32_t delay_ms = 10;
        if (xSemaphoreTake(lvgl_mutex, portMAX_DELAY) == pdTRUE) {
            uint32_t now_ms = (uint32_t)(esp_timer_get_time() / 1000);
            if (now_ms - last_wifi_refresh_ms >= 1000) {
                wifi_connect_update_icon_nolock(&guider_ui, wifi_connect_is_connected());
                last_wifi_refresh_ms = now_ms;
            }
            // 检查是否有 SNTP 时间更新
            if (sntp_time_data.updated) {
                set_home_time(&guider_ui, 
                             sntp_time_data.year,
                             sntp_time_data.month,
                             sntp_time_data.day,
                             sntp_time_data.wday,
                             sntp_time_data.hour,
                             sntp_time_data.min,
                             sntp_time_data.sec);
                sntp_time_data.updated = false;
            }
            
            delay_ms = lv_task_handler();
            watch_standby_poll();
            xSemaphoreGive(lvgl_mutex);
        }
        if (delay_ms > 500) delay_ms = 500;
        if (delay_ms < 1) delay_ms = 1;
        TickType_t delay_ticks = pdMS_TO_TICKS(delay_ms);
        vTaskDelay(delay_ticks > 0 ? delay_ticks : 1);
    }
}

/**
 * @brief 触摸读取回调
 */
static void touch_read_cb(lv_indev_t *indev, lv_indev_data_t *data)
{
    (void)indev;
    if (!board_touch_is_initialized()) {
        data->state = LV_INDEV_STATE_RELEASED;
        return;
    }
    
    cst816t_touch_data_t touch_data;
    esp_err_t touch_err = board_touch_read(&touch_data);
    if (watch_standby_touch(touch_err == ESP_OK && touch_data.pressed)) {
        data->state = LV_INDEV_STATE_RELEASED;
        return;
    }
    if (touch_err == ESP_OK && touch_data.pressed) {
        uint16_t x = touch_data.x;
        uint16_t y = touch_data.y;
        if (x >= BOARD_LCD_WIDTH) x = BOARD_LCD_WIDTH - 1;
        if (y >= BOARD_LCD_HEIGHT) y = BOARD_LCD_HEIGHT - 1;
        y = BOARD_LCD_HEIGHT - 1 - y;  // Y轴反转
        
        data->point.x = x;
        data->point.y = y;
        data->state = LV_INDEV_STATE_PRESSED;
    } else {
        data->state = LV_INDEV_STATE_RELEASED;
        if (touch_err != ESP_OK) {
            static int64_t last_touch_error_us = 0;
            int64_t now_us = esp_timer_get_time();
            if (now_us - last_touch_error_us >= 5000000) {
                ESP_LOGW(TAG, "Touch read failed: %s", esp_err_to_name(touch_err));
                last_touch_error_us = now_us;
            }
        }
    }
}

static void sntp_finsh_callback(struct timeval *tv)
{
    // 验证时间是否合理（2024年之后）
    if (tv->tv_sec < 1704067200) {
        ESP_LOGW(TAG, "SNTP 返回的时间不合理: %ld (< 2024-01-01)，忽略此次同步", tv->tv_sec);
        return;  // 忽略不合理的时间
    }
    
    struct tm  t;
    localtime_r(&tv->tv_sec, &t);
    
    // 保存时间数据，稍后在 LVGL 线程中更新 UI
    sntp_time_data.year = t.tm_year + 1900;
    sntp_time_data.month = t.tm_mon + 1;
    sntp_time_data.day = t.tm_mday;
    sntp_time_data.wday = t.tm_wday;
    sntp_time_data.hour = t.tm_hour;
    sntp_time_data.min = t.tm_min;
    sntp_time_data.sec = t.tm_sec;
    sntp_time_data.updated = true;
    
    ESP_LOGI(TAG, "✓ SNTP 时间同步成功: %04d-%02d-%02d %02d:%02d:%02d", 
             sntp_time_data.year, sntp_time_data.month, sntp_time_data.day,
             sntp_time_data.hour, sntp_time_data.min, sntp_time_data.sec);
    
    // 将网络时间保存到RTC，下次开机可直接从RTC读取
    rtc_sync_from_system();
}

void my_sntp_init(void)
{
    if(!esp_sntp_enabled()){
        ESP_LOGI(TAG, "SNTP init...");
        esp_sntp_setoperatingmode(ESP_SNTP_OPMODE_POLL);
        esp_sntp_setservername(0, "ntp.aliyun.com");
        esp_sntp_setservername(1, "time.asia.apple.com");
        esp_sntp_setservername(2, "pool.ntp.org");   
        esp_sntp_set_time_sync_notification_cb(sntp_finsh_callback);
        esp_sntp_init();
        ESP_LOGI(TAG, "SNTP 已启动，等待后台同步");
    }
}
void wifi_state_callback(WIFI_STATE state)
{
    if (state == WIFI_STATE_CONNECTED) {
        ESP_LOGI(TAG, "WiFi connected!");
        
        // 更新WiFi图标为已连接状态
        wifi_connect_update_icon(&guider_ui, true);
        my_sntp_init();
    } else {
        ESP_LOGI(TAG, "WiFi disconnected!");
        
        // 更新WiFi图标为断开状态
        wifi_connect_update_icon(&guider_ui, false);
    }
}

void app_main(void)
{
    nvs_flash_init();
    
    // 设置时区
    setenv("TZ", "CST-8", 1);
    tzset();
    
    // 先启动传感器（包括RTC初始化）
    sensors_start();
    
    // 尝试从RTC读取时间到系统
    if (rtc_is_initialized()) {
        if (rtc_sync_to_system() == ESP_OK) {
            ESP_LOGI(TAG, "✓ 已从RTC恢复系统时间");
        } else {
            ESP_LOGW(TAG, "RTC时间无效，使用默认时间");
            // Fallback：手动设置系统时间为 2026-02-05 13:00:00 (UTC+8)
            struct timeval tv = {
                .tv_sec = 1770220800,  // 2026-02-05 05:00:00 UTC
                .tv_usec = 0
            };
            settimeofday(&tv, NULL);
        }
    } else {
        ESP_LOGW(TAG, "RTC未初始化，使用默认时间");
        // Fallback：手动设置系统时间
        struct timeval tv = {
            .tv_sec = 1770220800,
            .tv_usec = 0
        };
        settimeofday(&tv, NULL);
    }
    
    time_t now;
    time(&now);
    struct tm timeinfo;
    localtime_r(&now, &timeinfo);
    char strftime_buf[64];
    strftime(strftime_buf, sizeof(strftime_buf), "%Y-%m-%d %H:%M:%S", &timeinfo);
    ESP_LOGI(TAG, "✓ 当前系统时间: %s (时间戳: %ld)", strftime_buf, now);
    
    // 初始化 LCD
    ESP_LOGI(TAG, "Init LCD...");
    ESP_ERROR_CHECK(board_display_init());
    g_lcd = board_display_get_lcd();
    
    // 初始化系统设置（亮度、音量）
    system_settings_init();
    
    // 初始化音频输出（供闹钟等模块使用）
    ESP_LOGI(TAG, "Init Audio Output...");
    max98357a_config_t audio_config = max98357a_get_default_config();
    audio_config.i2s_port = I2S_NUM_1;
    audio_config.bclk_pin = (gpio_num_t)AUDIO_BCLK_PIN;
    audio_config.lrclk_pin = (gpio_num_t)AUDIO_LRCLK_PIN;
    audio_config.din_pin = (gpio_num_t)AUDIO_DIN_PIN;
    audio_config.sd_mode_pin = (gpio_num_t)AUDIO_SD_MODE_PIN;
    audio_config.sample_rate = 16000;
    audio_config.bits_per_sample = I2S_DATA_BIT_WIDTH_16BIT;
    audio_config.gain = MAX98357A_GAIN_9DB;
    audio_config.channel = MAX98357A_CHANNEL_LEFT;
    
    esp_err_t audio_ret = max98357a_init(&audio_config, &g_audio_handle);
    if (audio_ret == ESP_OK) {
        ESP_LOGI(TAG, "Audio init OK");
    } else {
        ESP_LOGW(TAG, "Audio init failed: %s", esp_err_to_name(audio_ret));
    }
    
    img_spiffs_init();
    // 初始化 LVGL
    ESP_LOGI(TAG, "Init LVGL...");
    lv_init();
    
    lv_display_t *disp = lv_display_create(BOARD_LCD_WIDTH, BOARD_LCD_HEIGHT);
    lv_display_set_flush_cb(disp, lvgl_flush_cb);
    lv_display_set_color_format(disp, LV_COLOR_FORMAT_RGB565);
    
    // 刷屏回调同步等待 SPI 完成，一块较大的 DMA 缓冲可减少整页滚动时的分段次数。
    size_t buf_bytes = BOARD_LCD_WIDTH * 80 * 2;
    buf1 = heap_caps_malloc(buf_bytes, MALLOC_CAP_DMA);
    if (!buf1) {
        buf_bytes = BOARD_LCD_WIDTH * 40 * 2;
        buf1 = heap_caps_malloc(buf_bytes, MALLOC_CAP_DMA);
        ESP_LOGW(TAG, "80-line DMA buffer unavailable, using 40 lines");
    }
    if (!buf1) {
        ESP_LOGE(TAG, "Failed to allocate LVGL buffers");
        return;
    }
    lv_display_set_buffers(disp, buf1, NULL, buf_bytes, LV_DISPLAY_RENDER_MODE_PARTIAL);
    
    // LVGL 定时器
    const esp_timer_create_args_t timer_args = { .callback = &lvgl_tick_cb, .name = "lvgl_tick" };
    esp_timer_handle_t lvgl_timer = NULL;
    ESP_ERROR_CHECK(esp_timer_create(&timer_args, &lvgl_timer));
    ESP_ERROR_CHECK(esp_timer_start_periodic(lvgl_timer, 2000));
    
    lvgl_mutex = xSemaphoreCreateMutex();
    
    // 初始化触摸
    if (board_touch_init() == ESP_OK) {
        lv_indev_t *indev = lv_indev_create();
        lv_indev_set_type(indev, LV_INDEV_TYPE_POINTER);
        lv_indev_set_read_cb(indev, touch_read_cb);
        ESP_LOGI(TAG, "Touch init OK");
    }
    
    // 初始化 UI
    if (xSemaphoreTake(lvgl_mutex, portMAX_DELAY) == pdTRUE) {
        setup_ui(&guider_ui);
        custom_init(&guider_ui);
        time(&now);
        localtime_r(&now, &timeinfo);
        set_home_time(&guider_ui, timeinfo.tm_year + 1900, timeinfo.tm_mon + 1,
                      timeinfo.tm_mday, timeinfo.tm_wday,
                      timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec);
        xSemaphoreGive(lvgl_mutex);
    }
    
    ap_wifi_init(wifi_state_callback);
#if defined(WIFI_SSID) && defined(WIFI_PASSWORD)
    // 开机自动连接 ai_chat/ai_chat_config_user.h 里配置的 WiFi，
    // 覆盖 NVS 中的旧记录（否则会先去连上次配过的热点）
    ap_wifi_set(WIFI_SSID, WIFI_PASSWORD);
#endif
    watch_standby_init();

    // 启动 LVGL 任务
    xTaskCreate(lvgl_task, "lvgl", 8192, NULL, 4, NULL);

    // 启动语音助手：监听唤醒词"小智"，并把 screen_AI 的说话按钮接到同一引擎
    // （voice_dialog 与 ai_chat 都会独占麦克风/喇叭，故只启用 voice_dialog 一套）
    if (voice_assistant_start(&guider_ui) != ESP_OK) {
        ESP_LOGW(TAG, "语音助手启动失败（AI语音不可用，其它功能不受影响）");
    }

    ESP_LOGI(TAG, "Started");
    weather_start();
    
    // 初始化并启动闹钟模块
    alarm_clock_init();
    alarm_clock_start();

    // 初始化电池UI模块（定期刷新电池图标和百分比）
    if (!battery_ui_init(&guider_ui)) {
        ESP_LOGW(TAG, "电池UI模块初始化失败");
    }

    // 初始化SD卡并列出文件
    if (sd_card_fs_mount() == ESP_OK) {
        sd_card_fs_list_all(NULL, 0);
        if (sd_serial_transfer_start() != ESP_OK) {
            ESP_LOGW(TAG, "SD 串口传歌通道启动失败");
        }
    }
}
