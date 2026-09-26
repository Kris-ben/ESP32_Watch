/**
 * @file sensors.c
 * @brief 传感器管理模块实现 - MPU6050步数计数、BMP280气压传感器、BM8563 RTC
 */

#include "sensors.h"
#include "watch_standby.h"
#include "mpu6050.h"
#include "bmp280.h"
#include "bm8563.h"
#include "em7028.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include <string.h>
#include <stdio.h>
#include <time.h>
#include <sys/time.h>

static const char *TAG = "Sensors";

// 外部变量
extern lv_ui guider_ui;
extern SemaphoreHandle_t lvgl_mutex;

// BMP280设备实例
static bmp280_dev_t bmp280_dev;

// BM8563 RTC设备实例
static bm8563_handle_t rtc_handle;
static bool g_rtc_initialized = false;

// EM7028心率传感器实例
static em7028_dev_t em7028_dev;
static bool g_em7028_ready = false;

// MPU6050步数计数器
static step_counter_t step_counter;

// 传感器数据
static uint32_t g_step_count = 0;
static bool g_steps_valid = false;
static float g_pressure = 0.0f;
static bool g_sensors_valid = false;
static uint16_t g_heart_raw = 0;
static uint8_t g_heart_rate_bpm = 0;
static bool g_heart_rate_valid = false;

// 传感器任务句柄
static TaskHandle_t sensors_task_handle = NULL;

static void heart_rate_update_from_raw(uint16_t raw)
{
    static uint16_t baseline = 0;
    static uint16_t amplitude = 0;
    static bool above_threshold = false;
    static TickType_t last_peak_tick = 0;

    if (raw == 0) {
        g_heart_rate_valid = false;
        return;
    }

    if (baseline == 0) {
        baseline = raw;
        return;
    }

    baseline = (uint16_t)(((uint32_t)baseline * 15U + raw) / 16U);
    uint16_t diff = (raw > baseline) ? (raw - baseline) : (baseline - raw);
    amplitude = (uint16_t)(((uint32_t)amplitude * 7U + diff) / 8U);

    uint16_t threshold = baseline + (amplitude > 40 ? amplitude : 40);
    bool now_above = raw > threshold;
    TickType_t now_tick = xTaskGetTickCount();

    if (now_above && !above_threshold) {
        if (last_peak_tick != 0) {
            uint32_t interval_ms = (now_tick - last_peak_tick) * portTICK_PERIOD_MS;
            if (interval_ms >= 300 && interval_ms <= 2000) {
                uint32_t bpm = 60000U / interval_ms;
                if (bpm >= 40 && bpm <= 200) {
                    g_heart_rate_bpm = (uint8_t)bpm;
                    g_heart_rate_valid = true;
                }
            }
        }
        last_peak_tick = now_tick;
    }

    above_threshold = now_above;
}

/**
 * @brief 传感器任务
 */
static void sensors_task(void *param)
{
    mpu6050_data_t mpu_data;
    float temperature, pressure;
    
    ESP_LOGI(TAG, "传感器任务启动");
    
    // 等待一段时间让系统稳定
    vTaskDelay(pdMS_TO_TICKS(1000));
    
    while (1) {
        // 读取MPU6050数据并更新步数
        if (mpu6050_read_data(&mpu_data) == ESP_OK) {
            watch_standby_feed_motion(mpu_data.accel_x, mpu_data.accel_y, mpu_data.accel_z);
            uint32_t steps = step_counter_update(&step_counter, &mpu_data);
            __atomic_store_n(&g_step_count, steps, __ATOMIC_RELAXED);
            __atomic_store_n(&g_steps_valid, true, __ATOMIC_RELEASE);
            g_sensors_valid = true;
        } else {
            ESP_LOGW(TAG, "MPU6050读取失败");
        }
        
        // 读取BMP280气压数据
        if (bmp280_read_data(&bmp280_dev, &temperature, &pressure) == ESP_OK) {
            g_pressure = pressure;
            g_sensors_valid = true;
        } else {
            ESP_LOGW(TAG, "BMP280读取失败");
        }

        // 读取EM7028心率原始波形
        if (g_em7028_ready) {
            uint16_t heart_raw = 0;
            if (em7028_read_hrs1_raw(&em7028_dev, &heart_raw) == ESP_OK) {
                static TickType_t last_heart_log_tick = 0;
                g_heart_raw = heart_raw;
                heart_rate_update_from_raw(heart_raw);
                TickType_t now_tick = xTaskGetTickCount();
                if (now_tick - last_heart_log_tick >= pdMS_TO_TICKS(1000)) {
                    ESP_LOGI(TAG, "EM7028 raw=%u, bpm=%u, valid=%d",
                             (unsigned int)g_heart_raw,
                             (unsigned int)g_heart_rate_bpm,
                             g_heart_rate_valid);
                    last_heart_log_tick = now_tick;
                }
                g_sensors_valid = true;
            } else {
                g_heart_rate_valid = false;
                ESP_LOGW(TAG, "EM7028读取失败");
            }
        }
        
        // 更新UI（每500ms更新一次）
        if (g_sensors_valid) {
            update_home_sensors(&guider_ui);
        }
        
        // 50ms读取一次加速度（步数检测需要较高采样率）
        vTaskDelay(pdMS_TO_TICKS(50));
    }
}

bool sensors_get_step_count(uint32_t *out_steps)
{
    if (!out_steps || !__atomic_load_n(&g_steps_valid, __ATOMIC_ACQUIRE)) {
        return false;
    }
    *out_steps = __atomic_load_n(&g_step_count, __ATOMIC_RELAXED);
    return true;
}

/**
 * @brief 初始化传感器模块
 */
esp_err_t sensors_start(void)
{
    esp_err_t err;
    
    ESP_LOGI(TAG, "初始化传感器模块...");
    
    // 初始化MPU6050（会初始化I2C总线）
    err = mpu6050_init();
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "MPU6050初始化失败: %s", esp_err_to_name(err));
        return err;
    }
    ESP_LOGI(TAG, "MPU6050初始化成功");
    
    // 初始化步数计数器
    step_counter_init(&step_counter);
    
    // 初始化BMP280（使用MPU6050已初始化的I2C总线）
    err = bmp280_init(&bmp280_dev, I2C_MASTER_NUM, BMP280_I2C_ADDR_PRIM);
    if (err != ESP_OK) {
        // 尝试备用地址
        ESP_LOGW(TAG, "BMP280地址0x76失败，尝试0x77...");
        err = bmp280_init(&bmp280_dev, I2C_MASTER_NUM, BMP280_I2C_ADDR_SEC);
        if (err != ESP_OK) {
            ESP_LOGE(TAG, "BMP280初始化失败: %s", esp_err_to_name(err));
            // 即使BMP280失败，也继续运行MPU6050
        }
    }
    
    if (err == ESP_OK) {
        ESP_LOGI(TAG, "BMP280初始化成功");
        
        // 配置BMP280
        bmp280_config_t config;
        bmp280_get_default_config(&config);
        config.mode = BMP280_MODE_NORMAL;
        config.osr_pressure = BMP280_OVERSAMPLING_4X;
        config.osr_temperature = BMP280_OVERSAMPLING_2X;
        config.filter = BMP280_FILTER_4;
        config.standby_time = BMP280_STANDBY_125MS;
        
        err = bmp280_configure(&bmp280_dev, &config);
        if (err != ESP_OK) {
            ESP_LOGW(TAG, "BMP280配置失败: %s", esp_err_to_name(err));
        }
    }

    err = em7028_init(&em7028_dev, I2C_MASTER_NUM);
    if (err != ESP_OK) {
        g_em7028_ready = false;
        ESP_LOGW(TAG, "EM7028心率传感器初始化失败: %s (期望I2C地址0x%02X, PID 0x%02X)",
                 esp_err_to_name(err), EM7028_I2C_ADDR, EM7028_PID_VALUE);
    } else {
        g_em7028_ready = true;
        ESP_LOGI(TAG, "EM7028心率传感器初始化成功");
    }
    
    // 创建传感器任务
    BaseType_t ret = xTaskCreatePinnedToCore(
        sensors_task,
        "sensors_task",
        4096,
        NULL,
        3,
        &sensors_task_handle,
        1
    );
    
    if (ret != pdPASS) {
        ESP_LOGE(TAG, "创建传感器任务失败");
        return ESP_FAIL;
    }
    
    // 初始化BM8563 RTC（使用相同的I2C总线）
    err = bm8563_init(&rtc_handle, I2C_MASTER_NUM);
    if (err != ESP_OK) {
        ESP_LOGW(TAG, "BM8563 RTC初始化失败: %s", esp_err_to_name(err));
        // RTC失败不影响其他传感器
    } else {
        g_rtc_initialized = true;
        ESP_LOGI(TAG, "BM8563 RTC初始化成功");
    }
    
    ESP_LOGI(TAG, "传感器模块启动成功");
    return ESP_OK;
}

/**
 * @brief 更新UI上的传感器数据
 */
void update_home_sensors(lv_ui *ui)
{
    static uint32_t last_update_tick = 0;
    uint32_t current_tick = xTaskGetTickCount();
    
    // 限制更新频率为500ms一次
    if (current_tick - last_update_tick < pdMS_TO_TICKS(500)) {
        return;
    }
    last_update_tick = current_tick;
    
    if (!ui || !lvgl_mutex) {
        return;
    }
    
    if (xSemaphoreTake(lvgl_mutex, pdMS_TO_TICKS(100)) == pdTRUE) {
        // 检查主界面是否存在
        if (ui->screen_home && lv_obj_is_valid(ui->screen_home)) {
            char buf[32];
            
            // 更新步数
            if (ui->screen_home_label_foot && lv_obj_is_valid(ui->screen_home_label_foot)) {
                uint32_t steps = 0;
                if (sensors_get_step_count(&steps)) {
                    snprintf(buf, sizeof(buf), "%lu", (unsigned long)steps);
                } else {
                    snprintf(buf, sizeof(buf), "--");
                }
                lv_label_set_text(ui->screen_home_label_foot, buf);
            }
            
            // 更新气压（转换为hPa显示，更直观）
            if (ui->screen_home_label_pa && lv_obj_is_valid(ui->screen_home_label_pa)) {
                // g_pressure单位是Pa，转换为hPa (1hPa = 100Pa)
                float pressure_hpa = g_pressure / 100.0f;
                snprintf(buf, sizeof(buf), "%.0fhPa", pressure_hpa);
                lv_label_set_text(ui->screen_home_label_pa, buf);
            }

            // 更新心率。未形成稳定波形前显示检测中，方便区分芯片在线和算法未锁定。
            if (ui->screen_home_label_hr && lv_obj_is_valid(ui->screen_home_label_hr)) {
                if (!g_em7028_ready) {
                    lv_label_set_text(ui->screen_home_label_hr, "--次/分");
                } else if (g_heart_rate_valid) {
                    snprintf(buf, sizeof(buf), "%u次/分", (unsigned int)g_heart_rate_bpm);
                    lv_label_set_text(ui->screen_home_label_hr, buf);
                } else if (g_heart_raw > 0) {
                    lv_label_set_text(ui->screen_home_label_hr, "检测中");
                } else {
                    lv_label_set_text(ui->screen_home_label_hr, "--次/分");
                }
            }

            if (ui->screen_home_arc_heart_rate && lv_obj_is_valid(ui->screen_home_arc_heart_rate)) {
                lv_arc_set_value(ui->screen_home_arc_heart_rate,
                                 g_heart_rate_valid ? (g_heart_rate_bpm > 100 ? 100 : g_heart_rate_bpm) : 0);
            }
        }
        xSemaphoreGive(lvgl_mutex);
    }
}

/**
 * @brief 检查RTC是否已初始化
 */
bool rtc_is_initialized(void)
{
    return g_rtc_initialized;
}

/**
 * @brief 从RTC读取时间并设置系统时间
 */
esp_err_t rtc_sync_to_system(void)
{
    if (!g_rtc_initialized) {
        ESP_LOGW(TAG, "RTC未初始化");
        return ESP_ERR_INVALID_STATE;
    }
    
    bm8563_time_t rtc_time;
    esp_err_t err = bm8563_get_time(&rtc_handle, &rtc_time);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "读取RTC时间失败: %s", esp_err_to_name(err));
        return err;
    }
    
    // 检查RTC时间是否有效（年份>=24表示2024年及以后）
    if (rtc_time.year < 24) {
        ESP_LOGW(TAG, "RTC时间无效 (year=%d)，跳过同步", rtc_time.year);
        return ESP_ERR_INVALID_STATE;
    }
    
    // 将RTC时间转换为struct tm
    struct tm timeinfo = {
        .tm_sec = rtc_time.second,
        .tm_min = rtc_time.minute,
        .tm_hour = rtc_time.hour,
        .tm_mday = rtc_time.day,
        .tm_mon = rtc_time.month - 1,  // tm_mon是0-11
        .tm_year = rtc_time.year + 100, // tm_year是从1900年起的年数
        .tm_wday = rtc_time.weekday,
        .tm_isdst = 0
    };
    
    // 转换为时间戳并设置系统时间
    time_t t = mktime(&timeinfo);
    struct timeval tv = {
        .tv_sec = t,
        .tv_usec = 0
    };
    settimeofday(&tv, NULL);
    
    ESP_LOGI(TAG, "从RTC同步系统时间: %04d-%02d-%02d %02d:%02d:%02d",
             2000 + rtc_time.year, rtc_time.month, rtc_time.day,
             rtc_time.hour, rtc_time.minute, rtc_time.second);
    
    return ESP_OK;
}

/**
 * @brief 将系统时间写入RTC
 */
esp_err_t rtc_sync_from_system(void)
{
    if (!g_rtc_initialized) {
        ESP_LOGW(TAG, "RTC未初始化");
        return ESP_ERR_INVALID_STATE;
    }
    
    // 获取系统时间
    time_t now;
    time(&now);
    struct tm timeinfo;
    localtime_r(&now, &timeinfo);
    
    // 转换为BM8563时间格式
    bm8563_time_t rtc_time = {
        .second = (uint8_t)timeinfo.tm_sec,
        .minute = (uint8_t)timeinfo.tm_min,
        .hour = (uint8_t)timeinfo.tm_hour,
        .day = (uint8_t)timeinfo.tm_mday,
        .weekday = (uint8_t)timeinfo.tm_wday,
        .month = (uint8_t)(timeinfo.tm_mon + 1),  // tm_mon是0-11
        .year = (uint8_t)(timeinfo.tm_year - 100)  // BM8563年份是0-99表示2000-2099
    };
    
    esp_err_t err = bm8563_set_time(&rtc_handle, &rtc_time);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "写入RTC时间失败: %s", esp_err_to_name(err));
        return err;
    }
    
    ESP_LOGI(TAG, "系统时间已写入RTC: %04d-%02d-%02d %02d:%02d:%02d",
             2000 + rtc_time.year, rtc_time.month, rtc_time.day,
             rtc_time.hour, rtc_time.minute, rtc_time.second);
    
    return ESP_OK;
}
