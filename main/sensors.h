/**
 * @file sensors.h
 * @brief 传感器管理模块 - MPU6050步数计数、BMP280气压传感器、BM8563 RTC
 */

#ifndef SENSORS_H
#define SENSORS_H

#include "esp_err.h"
#include "gui_guider.h"
#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief 初始化传感器模块并启动传感器任务
 * @return esp_err_t ESP_OK表示成功
 */
esp_err_t sensors_start(void);

/**
 * @brief 读取设备计步器当前累计值
 * @param out_steps 输出步数
 * @return true 已成功读取过 MPU6050；false 传感器数据尚未就绪
 */
bool sensors_get_step_count(uint32_t *out_steps);

/**
 * @brief 更新主界面传感器数据显示（外部调用）
 * @param ui UI结构体指针
 */
void update_home_sensors(lv_ui *ui);

/**
 * @brief 从RTC读取时间并设置系统时间
 * @return esp_err_t ESP_OK表示成功
 */
esp_err_t rtc_sync_to_system(void);

/**
 * @brief 将系统时间写入RTC
 * @return esp_err_t ESP_OK表示成功
 */
esp_err_t rtc_sync_from_system(void);

/**
 * @brief 检查RTC是否已初始化
 * @return bool true=已初始化
 */
bool rtc_is_initialized(void);

#ifdef __cplusplus
}
#endif

#endif // SENSORS_H
