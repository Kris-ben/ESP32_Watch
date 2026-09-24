/**
 * @file battery_monitor.c
 * @brief ESP32-S3 电池电量监控模块实现
 */

#include "battery_monitor.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_adc/adc_oneshot.h"
#include "esp_adc/adc_cali.h"
#include "esp_adc/adc_cali_scheme.h"
#include "esp_log.h"
#include <stdint.h>
#include "driver/gpio.h"

// ==================== 私有变量 ====================
static const char *TAG = "BATTERY";

static adc_oneshot_unit_handle_t adc_handle = NULL;
static adc_cali_handle_t adc_cali_handle = NULL;
static bool adc_calibrated = false;
static bool is_initialized = false;

// 计算分压比
#define VOLTAGE_DIVIDER_RATIO   ((float)(R19_RESISTANCE + R20_RESISTANCE) / R20_RESISTANCE)
#define BATTERY_FILTER_ALPHA_NUM 1
#define BATTERY_FILTER_ALPHA_DEN 4

// ==================== 私有函数 ====================

/**
 * @brief ADC校准初始化
 */
static bool adc_calibration_init(void)
{
    esp_err_t ret = ESP_FAIL;
    
#if ADC_CALI_SCHEME_CURVE_FITTING_SUPPORTED
    adc_cali_curve_fitting_config_t cali_config = {
        .unit_id = BATTERY_ADC_UNIT,
        .atten = BATTERY_ADC_ATTEN,
        .bitwidth = BATTERY_ADC_BITWIDTH,
    };
    ret = adc_cali_create_scheme_curve_fitting(&cali_config, &adc_cali_handle);
    if (ret == ESP_OK) {
        adc_calibrated = true;
        ESP_LOGI(TAG, "ADC校准成功 (Curve Fitting)");
    }
#elif ADC_CALI_SCHEME_LINE_FITTING_SUPPORTED
    adc_cali_line_fitting_config_t cali_config = {
        .unit_id = BATTERY_ADC_UNIT,
        .atten = BATTERY_ADC_ATTEN,
        .bitwidth = BATTERY_ADC_BITWIDTH,
    };
    ret = adc_cali_create_scheme_line_fitting(&cali_config, &adc_cali_handle);
    if (ret == ESP_OK) {
        adc_calibrated = true;
        ESP_LOGI(TAG, "ADC校准成功 (Line Fitting)");
    }
#endif
    
    if (!adc_calibrated) {
        ESP_LOGW(TAG, "ADC校准失败，使用原始值");
    }
    
    return adc_calibrated;
}

/**
 * @brief 电池监控任务
 */
static void battery_monitor_task(void *arg)
{
    uint32_t interval_ms = (uint32_t)(uintptr_t)arg;
    battery_info_t info;
    
    ESP_LOGI(TAG, "监控任务启动 (周期: %lu ms)", interval_ms);
    ESP_LOGI(TAG, "配置: 满电=%dmV, 空电=%dmV, 分压比=%.2f", 
             BATTERY_MAX_VOLTAGE, BATTERY_MIN_VOLTAGE, VOLTAGE_DIVIDER_RATIO);
    
    while (1) {
        if (battery_get_info(&info)) {
            ESP_LOGI(TAG, "========================================");
            ESP_LOGI(TAG, "电压: %d mV (%.2f V)", info.voltage_mv, info.voltage_mv / 1000.0);
            ESP_LOGI(TAG, "电量: %d%%", info.percentage);
            ESP_LOGI(TAG, "状态: %s", info.status);
            ESP_LOGI(TAG, "========================================");
            
            // 低电量警告
            if (info.percentage >= 0 && info.percentage < 20) {
                ESP_LOGW(TAG, "⚠️  警告: 电量低于20%%，请及时充电！");
            }
        } else {
            ESP_LOGE(TAG, "读取电池信息失败");
        }
        
        vTaskDelay(pdMS_TO_TICKS(interval_ms));
    }
}

// ==================== 公共API实现 ====================

bool battery_monitor_init(void)
{
    if (is_initialized) {
        ESP_LOGW(TAG, "已经初始化，跳过");
        return true;
    }

#if !BATTERY_MONITOR_HAS_BATTERY
    is_initialized = true;
    ESP_LOGI(TAG, "当前配置未安装电池，跳过 ADC 测量");
    return true;
#endif
    
    esp_err_t ret;
    
    // 配置ADC单元
    adc_oneshot_unit_init_cfg_t init_config = {
        .unit_id = BATTERY_ADC_UNIT,
    };
    ret = adc_oneshot_new_unit(&init_config, &adc_handle);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "ADC单元初始化失败: %s", esp_err_to_name(ret));
        return false;
    }
    
    // 配置ADC通道
    adc_oneshot_chan_cfg_t config = {
        .bitwidth = BATTERY_ADC_BITWIDTH,
        .atten = BATTERY_ADC_ATTEN,
    };
    ret = adc_oneshot_config_channel(adc_handle, BATTERY_ADC_CHANNEL, &config);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "ADC通道配置失败: %s", esp_err_to_name(ret));
        adc_oneshot_del_unit(adc_handle);
        return false;
    }

#if BATTERY_ADC_ENABLE_INTERNAL_PULLDOWN
    // 防止电池断开/测点悬空导致ADC读到“残留电压/随机高电压”
    gpio_set_direction(BATTERY_ADC_GPIO, GPIO_MODE_INPUT);
    gpio_set_pull_mode(BATTERY_ADC_GPIO, GPIO_PULLDOWN_ONLY);
#endif
    
    // 初始化ADC校准
    adc_calibration_init();
    
    is_initialized = true;
    ESP_LOGI(TAG, "初始化成功 (GPIO%d, ADC1_CH%d)", BATTERY_ADC_GPIO, BATTERY_ADC_CHANNEL);
    
    return true;
}

int battery_read_voltage(void)
{
#if !BATTERY_MONITOR_HAS_BATTERY
    return -1;
#endif
    if (!is_initialized) {
        ESP_LOGE(TAG, "未初始化");
        return -1;
    }
    
    int adc_raw = 0;
    int voltage_mv = 0;
    int sum = 0;
    int sum_raw = 0;
    esp_err_t ret;
    
    for (int i = 0; i < BATTERY_DISCARD_COUNT; i++) {
        ret = adc_oneshot_read(adc_handle, BATTERY_ADC_CHANNEL, &adc_raw);
        if (ret != ESP_OK) {
            ESP_LOGE(TAG, "ADC读取失败: %s", esp_err_to_name(ret));
            return -1;
        }
        vTaskDelay(pdMS_TO_TICKS(BATTERY_SAMPLE_INTERVAL));
    }

    // 多次采样取平均值。你的分压等效阻抗较高，丢首样+平均能减小抖动。
    for (int i = 0; i < BATTERY_SAMPLE_COUNT; i++) {
        ret = adc_oneshot_read(adc_handle, BATTERY_ADC_CHANNEL, &adc_raw);
        if (ret != ESP_OK) {
            ESP_LOGE(TAG, "ADC读取失败: %s", esp_err_to_name(ret));
            return -1;
        }
        
        if (adc_calibrated) {
            // 使用校准值
            ret = adc_cali_raw_to_voltage(adc_cali_handle, adc_raw, &voltage_mv);
            if (ret != ESP_OK) {
                ESP_LOGE(TAG, "电压转换失败: %s", esp_err_to_name(ret));
                return -1;
            }
        } else {
            // 校准不可用时才采用量程对应的粗略线性换算。
            voltage_mv = (adc_raw * BATTERY_ADC_FALLBACK_FULL_SCALE_MV) / 4095;
        }
        
        sum += voltage_mv;
        sum_raw += adc_raw;
        
        if (i < BATTERY_SAMPLE_COUNT - 1) {
            vTaskDelay(pdMS_TO_TICKS(BATTERY_SAMPLE_INTERVAL));
        }
    }
    
    // 计算平均值
    int avg_voltage_mv = sum / BATTERY_SAMPLE_COUNT;
    int avg_raw = sum_raw / BATTERY_SAMPLE_COUNT;
    
    // 根据分压比计算实际电池电压，再套用软件校准系数。
    int battery_voltage_mv = (int)(avg_voltage_mv * VOLTAGE_DIVIDER_RATIO);
    battery_voltage_mv = (battery_voltage_mv * BATTERY_CALIBRATION_NUM) / BATTERY_CALIBRATION_DEN;

    static int filtered_voltage_mv = 0;
    if (filtered_voltage_mv == 0) {
        filtered_voltage_mv = battery_voltage_mv;
    } else {
        filtered_voltage_mv = ((filtered_voltage_mv * (BATTERY_FILTER_ALPHA_DEN - BATTERY_FILTER_ALPHA_NUM)) +
                               (battery_voltage_mv * BATTERY_FILTER_ALPHA_NUM)) /
                              BATTERY_FILTER_ALPHA_DEN;
    }

    ESP_LOGI(TAG, "ADC raw=%d, ADC输入=%dmV, 电池=%dmV, 平滑=%dmV, 分压比=%.2f, 校准=%d/%d",
             avg_raw, avg_voltage_mv, battery_voltage_mv, filtered_voltage_mv,
             VOLTAGE_DIVIDER_RATIO, BATTERY_CALIBRATION_NUM, BATTERY_CALIBRATION_DEN);
    
    return filtered_voltage_mv;
}

int battery_calculate_percentage(int voltage_mv)
{
    // 近似锂电曲线的分段表（静态负载下的粗略估计）
    static const struct {
        int mv;
        int pct;
    } lut[] = {
        {4200, 100},
        {4100, 90},
        {4000, 80},
        {3900, 70},
        {3800, 60},
        {3700, 50},
        {3600, 35},
        {3500, 20},
        {3400, 10},
        {3300, 5},
        {3200, 0},
    };

    if (voltage_mv >= lut[0].mv) {
        return 100;
    }
    if (voltage_mv <= lut[(int)(sizeof(lut) / sizeof(lut[0])) - 1].mv) {
        return 0;
    }

    for (int i = 0; i < (int)(sizeof(lut) / sizeof(lut[0])) - 1; i++) {
        int v_high = lut[i].mv;
        int v_low = lut[i + 1].mv;
        if (voltage_mv <= v_high && voltage_mv >= v_low) {
            int p_high = lut[i].pct;
            int p_low = lut[i + 1].pct;
            int pct = p_low + (voltage_mv - v_low) * (p_high - p_low) / (v_high - v_low);
            return pct;
        }
    }

    return 0;
}

const char* battery_get_status(int percentage)
{
    if (percentage >= 80) return "优秀";
    if (percentage >= 60) return "良好";
    if (percentage >= 40) return "中等";
    if (percentage >= 20) return "偏低";
    return "严重不足";
}

bool battery_get_info(battery_info_t *info)
{
    if (info == NULL) {
        ESP_LOGE(TAG, "参数为空");
        return false;
    }

#if !BATTERY_MONITOR_HAS_BATTERY
    info->voltage_mv = -1;
    info->percentage = -1;
    info->status = "未安装电池";
    return true;
#endif
    
    // 读取电压
    info->voltage_mv = battery_read_voltage();
    if (info->voltage_mv < 0) {
        return false;
    }
    
    // 计算电量
    info->percentage = battery_calculate_percentage(info->voltage_mv);
    
    // 获取状态
    info->status = battery_get_status(info->percentage);
    
    return true;
}

bool battery_start_monitor_task(uint32_t interval_ms)
{
    if (!is_initialized) {
        ESP_LOGE(TAG, "请先调用 battery_monitor_init()");
        return false;
    }
    
    // 使用默认值
    if (interval_ms == 0) {
        interval_ms = 5000;
    }
    
    BaseType_t ret = xTaskCreate(
        battery_monitor_task,
        "battery_monitor",
        4096,
        (void*)(uintptr_t)interval_ms,
        5,
        NULL
    );
    
    if (ret != pdPASS) {
        ESP_LOGE(TAG, "任务创建失败");
        return false;
    }
    
    return true;
}
