/**
 * @file alarm_clock.c
 * @brief 闹钟功能模块实现
 */

#include "alarm_clock.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "nvs.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "max98357a.h"
#include "system_settings.h"
#include <time.h>
#include <string.h>
#include <stdio.h>
#include <math.h>

// 外部音频句柄获取函数
extern max98357a_handle_t **get_audio_handle(void);

static const char *TAG = "AlarmClock";

// 闹钟数据
static alarm_time_t g_alarms[ALARM_COUNT] = {
    {8, 0, false},
    {12, 0, false},
    {14, 0, false},
    {19, 0, false}
};

// 当前正在编辑的闹钟索引
static int g_editing_index = 0;

// 闹钟任务句柄
static TaskHandle_t g_alarm_task_handle = NULL;

// 是否正在响铃
static volatile bool g_is_ringing = false;

// NVS存储键
#define NVS_NAMESPACE "alarm"
#define NVS_KEY_ALARM0 "alarm0"
#define NVS_KEY_ALARM1 "alarm1"
#define NVS_KEY_ALARM2 "alarm2"
#define NVS_KEY_ALARM3 "alarm3"

// LVGL互斥锁 (从main.c extern)
extern SemaphoreHandle_t lvgl_mutex;

/**
 * @brief 从NVS加载闹钟设置
 */
static void load_alarms_from_nvs(void)
{
    nvs_handle_t nvs_handle;
    esp_err_t err = nvs_open(NVS_NAMESPACE, NVS_READONLY, &nvs_handle);
    if (err == ESP_OK) {
        const char *keys[] = {NVS_KEY_ALARM0, NVS_KEY_ALARM1, NVS_KEY_ALARM2, NVS_KEY_ALARM3};
        for (int i = 0; i < ALARM_COUNT; i++) {
            uint32_t val;
            if (nvs_get_u32(nvs_handle, keys[i], &val) == ESP_OK) {
                g_alarms[i].hour = (val >> 16) & 0xFF;
                g_alarms[i].minute = (val >> 8) & 0xFF;
                g_alarms[i].enabled = (val & 0xFF) != 0;
                ESP_LOGI(TAG, "加载闹钟%d: %02d:%02d %s", i, 
                         g_alarms[i].hour, g_alarms[i].minute, 
                         g_alarms[i].enabled ? "启用" : "禁用");
            }
        }
        nvs_close(nvs_handle);
    }
}

/**
 * @brief 保存闹钟设置到NVS
 */
static void save_alarms_to_nvs(void)
{
    nvs_handle_t nvs_handle;
    esp_err_t err = nvs_open(NVS_NAMESPACE, NVS_READWRITE, &nvs_handle);
    if (err == ESP_OK) {
        const char *keys[] = {NVS_KEY_ALARM0, NVS_KEY_ALARM1, NVS_KEY_ALARM2, NVS_KEY_ALARM3};
        for (int i = 0; i < ALARM_COUNT; i++) {
            uint32_t val = ((uint32_t)g_alarms[i].hour << 16) | 
                          ((uint32_t)g_alarms[i].minute << 8) | 
                          (g_alarms[i].enabled ? 1 : 0);
            nvs_set_u32(nvs_handle, keys[i], val);
        }
        nvs_commit(nvs_handle);
        nvs_close(nvs_handle);
    }
}

/**
 * @brief 生成方波噪音的PCM数据
 */
static void generate_beep_sound(int16_t *buffer, size_t samples, int frequency)
{
    for (size_t i = 0; i < samples; i++) {
        // 生成方波
        float t = (float)i / 16000.0f;  // 16kHz采样率
        int16_t val = (((int)(t * frequency * 2) % 2) == 0) ? 16000 : -16000;
        
        // 应用音量
        uint8_t volume = system_get_volume();
        val = (int16_t)(val * volume / 100);
        
        buffer[i * 2] = val;      // 左声道
        buffer[i * 2 + 1] = val;  // 右声道
    }
}

/**
 * @brief 播放闹钟铃声 (每秒一次beep，持续30秒)
 */
static void play_alarm_sound(void)
{
    ESP_LOGI(TAG, "开始播放闹钟铃声");
    
    max98357a_handle_t **pp_handle = get_audio_handle();
    if (!pp_handle || !*pp_handle) {
        ESP_LOGE(TAG, "音频设备未初始化");
        return;
    }
    max98357a_handle_t *handle = *pp_handle;
    
    // 生成较小的beep音块 (100ms @ 16kHz)
    const size_t samples = 1600;  // 0.1秒 @ 16kHz
    const size_t buffer_size = samples * 2 * sizeof(int16_t);  // 立体声 = 6.4KB
    
    int16_t *beep_buffer = heap_caps_malloc(buffer_size, MALLOC_CAP_DMA);
    if (!beep_buffer) {
        // 尝试使用普通内存
        beep_buffer = malloc(buffer_size);
        if (!beep_buffer) {
            ESP_LOGE(TAG, "无法分配beep缓冲区");
            return;
        }
    }
    
    // 生成1000Hz方波
    generate_beep_sound(beep_buffer, samples, 1000);
    
    // 播放30次（每秒一次，持续30秒）
    for (int i = 0; i < 30 && g_is_ringing; i++) {
        // 播放0.5秒 = 5个100ms块
        for (int j = 0; j < 5 && g_is_ringing; j++) {
            size_t bytes_written = 0;
            esp_err_t ret = max98357a_write(handle, beep_buffer, buffer_size, &bytes_written, 1000);
            if (ret != ESP_OK) {
                ESP_LOGE(TAG, "播放失败: %s", esp_err_to_name(ret));
                goto cleanup;
            }
        }
        
        // 等待剩余0.5秒
        vTaskDelay(pdMS_TO_TICKS(500));
        
        if (!g_is_ringing) {
            ESP_LOGI(TAG, "闹钟被手动停止");
            break;
        }
    }
    
cleanup:
    free(beep_buffer);
    g_is_ringing = false;
    ESP_LOGI(TAG, "闹钟铃声播放结束");
}

/**
 * @brief 闹钟检查任务
 */
static void alarm_check_task(void *arg)
{
    ESP_LOGI(TAG, "闹钟检查任务启动");
    
    int last_triggered_minute = -1;  // 防止同一分钟内重复触发
    
    while (1) {
        // 每秒检查一次
        vTaskDelay(pdMS_TO_TICKS(1000));
        
        // 如果正在响铃，跳过检查
        if (g_is_ringing) {
            continue;
        }
        
        // 获取当前时间
        time_t now;
        time(&now);
        struct tm timeinfo;
        localtime_r(&now, &timeinfo);
        
        int current_hour = timeinfo.tm_hour;
        int current_minute = timeinfo.tm_min;
        int current_total_minutes = current_hour * 60 + current_minute;
        
        // 检查每个闹钟
        for (int i = 0; i < ALARM_COUNT; i++) {
            if (!g_alarms[i].enabled) {
                continue;
            }
            
            int alarm_total_minutes = g_alarms[i].hour * 60 + g_alarms[i].minute;
            
            // 检查是否匹配且不是重复触发
            if (current_total_minutes == alarm_total_minutes && 
                current_total_minutes != last_triggered_minute) {
                
                ESP_LOGI(TAG, "闹钟%d触发: %02d:%02d", i, 
                         g_alarms[i].hour, g_alarms[i].minute);
                
                last_triggered_minute = current_total_minutes;
                g_is_ringing = true;
                
                // 播放闹钟铃声
                play_alarm_sound();
                
                break;  // 一次只触发一个闹钟
            }
        }
        
        // 每分钟重置触发记录
        if (timeinfo.tm_sec == 0) {
            last_triggered_minute = -1;
        }
    }
}

esp_err_t alarm_clock_init(void)
{
    ESP_LOGI(TAG, "初始化闹钟模块...");
    
    // 从NVS加载闹钟设置
    load_alarms_from_nvs();
    
    ESP_LOGI(TAG, "闹钟模块初始化完成");
    return ESP_OK;
}

esp_err_t alarm_clock_start(void)
{
    if (g_alarm_task_handle != NULL) {
        ESP_LOGW(TAG, "闹钟任务已在运行");
        return ESP_OK;
    }
    
    BaseType_t ret = xTaskCreatePinnedToCore(
        alarm_check_task,
        "alarm_task",
        4096,
        NULL,
        2,
        &g_alarm_task_handle,
        0
    );
    
    if (ret != pdPASS) {
        ESP_LOGE(TAG, "创建闹钟任务失败");
        return ESP_FAIL;
    }
    
    ESP_LOGI(TAG, "闹钟检查任务启动成功");
    return ESP_OK;
}

void alarm_set_editing_index(int index)
{
    if (index >= 0 && index < ALARM_COUNT) {
        g_editing_index = index;
        ESP_LOGI(TAG, "设置编辑闹钟索引: %d", index);
    }
}

int alarm_get_editing_index(void)
{
    return g_editing_index;
}

esp_err_t alarm_set_time(int index, uint8_t hour, uint8_t minute)
{
    if (index < 0 || index >= ALARM_COUNT) {
        return ESP_ERR_INVALID_ARG;
    }
    
    g_alarms[index].hour = hour;
    g_alarms[index].minute = minute;
    
    ESP_LOGI(TAG, "设置闹钟%d时间: %02d:%02d", index, hour, minute);
    
    // 保存到NVS
    save_alarms_to_nvs();
    
    return ESP_OK;
}

esp_err_t alarm_get_time(int index, uint8_t *hour, uint8_t *minute)
{
    if (index < 0 || index >= ALARM_COUNT) {
        return ESP_ERR_INVALID_ARG;
    }
    
    if (hour) *hour = g_alarms[index].hour;
    if (minute) *minute = g_alarms[index].minute;
    
    return ESP_OK;
}

esp_err_t alarm_set_enabled(int index, bool enabled)
{
    if (index < 0 || index >= ALARM_COUNT) {
        return ESP_ERR_INVALID_ARG;
    }
    
    g_alarms[index].enabled = enabled;
    
    ESP_LOGI(TAG, "闹钟%d %s", index, enabled ? "启用" : "禁用");
    
    // 保存到NVS
    save_alarms_to_nvs();
    
    return ESP_OK;
}

bool alarm_is_enabled(int index)
{
    if (index < 0 || index >= ALARM_COUNT) {
        return false;
    }
    return g_alarms[index].enabled;
}

void alarm_update_ui(lv_ui *ui)
{
    if (!ui) return;
    
    // 更新list_clock中的时间显示
    lv_obj_t *items[] = {
        ui->screen_clock_list_clock_item0,
        ui->screen_clock_list_clock_item1,
        ui->screen_clock_list_clock_item2,
        ui->screen_clock_list_clock_item3
    };
    
    for (int i = 0; i < ALARM_COUNT; i++) {
        if (items[i] && lv_obj_is_valid(items[i])) {
            char time_str[16];
            snprintf(time_str, sizeof(time_str), "%d:%02d", 
                     g_alarms[i].hour, g_alarms[i].minute);
            // 使用LVGL提供的API更新列表按钮文本
            lv_list_set_button_text(ui->screen_clock_list_clock, items[i], time_str);
            ESP_LOGI(TAG, "更新闹钟%d显示: %s", i, time_str);
        }
    }
    
    // 更新开关状态
    lv_obj_t *switches[] = {
        ui->screen_clock_sw_1,
        ui->screen_clock_sw_2,
        ui->screen_clock_sw_3,
        ui->screen_clock_sw_4
    };
    
    for (int i = 0; i < ALARM_COUNT; i++) {
        if (switches[i] && lv_obj_is_valid(switches[i])) {
            if (g_alarms[i].enabled) {
                lv_obj_add_state(switches[i], LV_STATE_CHECKED);
            } else {
                lv_obj_remove_state(switches[i], LV_STATE_CHECKED);
            }
        }
    }
    
    ESP_LOGI(TAG, "UI已更新");
}

void alarm_save_from_roller(lv_ui *ui)
{
    if (!ui) return;
    
    // 获取roller当前选择的值
    uint32_t hour_index = lv_roller_get_selected(ui->screen_set_clock_roller_hour);
    uint32_t minute_index = lv_roller_get_selected(ui->screen_set_clock_roller_minute);
    
    // roller选项: 1时,2时,...,23时,0时 -> index 0-23 对应 hour 1-23,0
    uint8_t hour;
    if (hour_index == 23) {
        hour = 0;  // 最后一个选项是0时
    } else {
        hour = hour_index + 1;  // 其他选项: index 0 = 1时, index 1 = 2时, ...
    }
    
    // roller选项: 00分,10分,20分,30分,40分,50分 -> index * 10
    uint8_t minute = minute_index * 10;
    
    // 保存到当前编辑的闹钟
    if (alarm_set_time(g_editing_index, hour, minute) == ESP_OK) {
        // “确定”表示启用这个时间；列表上的开关在返回时同步显示。
        alarm_set_enabled(g_editing_index, true);
    }
    
    ESP_LOGI(TAG, "从roller保存闹钟%d: %02d:%02d (hour_idx=%lu, min_idx=%lu)", 
             g_editing_index, hour, minute, hour_index, minute_index);
}

void alarm_stop_ringing(void)
{
    g_is_ringing = false;
}

// 开关事件处理回调
static void alarm_switch_event_cb(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    if (code != LV_EVENT_VALUE_CHANGED) return;
    
    lv_obj_t *sw = lv_event_get_target(e);
    lv_ui *ui = lv_event_get_user_data(e);
    
    // 确定是哪个开关
    int index = -1;
    if (sw == ui->screen_clock_sw_1) index = 0;
    else if (sw == ui->screen_clock_sw_2) index = 1;
    else if (sw == ui->screen_clock_sw_3) index = 2;
    else if (sw == ui->screen_clock_sw_4) index = 3;
    
    if (index >= 0) {
        bool enabled = lv_obj_has_state(sw, LV_STATE_CHECKED);
        alarm_set_enabled(index, enabled);
    }
}

// list item点击事件处理回调
static void alarm_list_item_event_cb(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    if (code != LV_EVENT_CLICKED) return;
    
    lv_obj_t *item = lv_event_get_target(e);
    lv_ui *ui = lv_event_get_user_data(e);
    
    // 确定是哪个选项
    int index = -1;
    if (item == ui->screen_clock_list_clock_item0) index = 0;
    else if (item == ui->screen_clock_list_clock_item1) index = 1;
    else if (item == ui->screen_clock_list_clock_item2) index = 2;
    else if (item == ui->screen_clock_list_clock_item3) index = 3;
    
    if (index >= 0) {
        alarm_set_editing_index(index);
        ESP_LOGI(TAG, "点击闹钟%d，准备编辑", index);
    }
}

// 确定按钮事件处理回调
static void alarm_btn_set_event_cb(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    if (code != LV_EVENT_CLICKED) return;
    
    lv_ui *ui = lv_event_get_user_data(e);
    
    // 保存roller设置的时间
    alarm_save_from_roller(ui);
    
    ESP_LOGI(TAG, "确定按钮点击，保存闹钟设置");
}

void alarm_register_clock_events(lv_ui *ui)
{
    if (!ui) return;

    // 新列表建好后立即填入已保存的时间和开关状态。
    alarm_update_ui(ui);
    
    // 注册开关事件
    if (ui->screen_clock_sw_1 && lv_obj_is_valid(ui->screen_clock_sw_1)) {
        lv_obj_add_event_cb(ui->screen_clock_sw_1, alarm_switch_event_cb, LV_EVENT_VALUE_CHANGED, ui);
    }
    if (ui->screen_clock_sw_2 && lv_obj_is_valid(ui->screen_clock_sw_2)) {
        lv_obj_add_event_cb(ui->screen_clock_sw_2, alarm_switch_event_cb, LV_EVENT_VALUE_CHANGED, ui);
    }
    if (ui->screen_clock_sw_3 && lv_obj_is_valid(ui->screen_clock_sw_3)) {
        lv_obj_add_event_cb(ui->screen_clock_sw_3, alarm_switch_event_cb, LV_EVENT_VALUE_CHANGED, ui);
    }
    if (ui->screen_clock_sw_4 && lv_obj_is_valid(ui->screen_clock_sw_4)) {
        lv_obj_add_event_cb(ui->screen_clock_sw_4, alarm_switch_event_cb, LV_EVENT_VALUE_CHANGED, ui);
    }
    
    // 注册list item点击事件（记录编辑索引）
    if (ui->screen_clock_list_clock_item0 && lv_obj_is_valid(ui->screen_clock_list_clock_item0)) {
        lv_obj_add_event_cb(ui->screen_clock_list_clock_item0, alarm_list_item_event_cb, LV_EVENT_CLICKED, ui);
    }
    if (ui->screen_clock_list_clock_item1 && lv_obj_is_valid(ui->screen_clock_list_clock_item1)) {
        lv_obj_add_event_cb(ui->screen_clock_list_clock_item1, alarm_list_item_event_cb, LV_EVENT_CLICKED, ui);
    }
    if (ui->screen_clock_list_clock_item2 && lv_obj_is_valid(ui->screen_clock_list_clock_item2)) {
        lv_obj_add_event_cb(ui->screen_clock_list_clock_item2, alarm_list_item_event_cb, LV_EVENT_CLICKED, ui);
    }
    if (ui->screen_clock_list_clock_item3 && lv_obj_is_valid(ui->screen_clock_list_clock_item3)) {
        lv_obj_add_event_cb(ui->screen_clock_list_clock_item3, alarm_list_item_event_cb, LV_EVENT_CLICKED, ui);
    }
    
    ESP_LOGI(TAG, "screen_clock事件已注册");
}

void alarm_register_set_clock_events(lv_ui *ui)
{
    if (!ui) return;
    
    // 注册确定按钮事件
    if (ui->screen_set_clock_btn_set && lv_obj_is_valid(ui->screen_set_clock_btn_set)) {
        lv_obj_add_event_cb(ui->screen_set_clock_btn_set, alarm_btn_set_event_cb, LV_EVENT_CLICKED, ui);
    }
    
    // 设置roller初始值为当前编辑闹钟的时间
    int index = g_editing_index;
    uint8_t hour = g_alarms[index].hour;
    uint8_t minute = g_alarms[index].minute;
    
    // 计算hour对应的roller索引
    uint32_t hour_index;
    if (hour == 0) {
        hour_index = 23;  // 0时是最后一个选项
    } else {
        hour_index = hour - 1;  // 1时是index 0, 2时是index 1, ...
    }
    
    // 计算minute对应的roller索引 (00分=0, 10分=1, ...)
    uint32_t minute_index = minute / 10;
    
    if (ui->screen_set_clock_roller_hour && lv_obj_is_valid(ui->screen_set_clock_roller_hour)) {
        lv_roller_set_selected(ui->screen_set_clock_roller_hour, hour_index, LV_ANIM_OFF);
    }
    if (ui->screen_set_clock_roller_minute && lv_obj_is_valid(ui->screen_set_clock_roller_minute)) {
        lv_roller_set_selected(ui->screen_set_clock_roller_minute, minute_index, LV_ANIM_OFF);
    }
    
    ESP_LOGI(TAG, "screen_set_clock事件已注册，初始时间: %02d:%02d", hour, minute);
}
