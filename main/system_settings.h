/**
 * @file system_settings.h
 * @brief 系统设置模块 - 音量和亮度控制
 */

#ifndef SYSTEM_SETTINGS_H
#define SYSTEM_SETTINGS_H

#include <stdint.h>
#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief 初始化系统设置
 */
esp_err_t system_settings_init(void);

/**
 * @brief 设置屏幕亮度
 * @param percent 亮度百分比 (0-100)
 */
esp_err_t system_set_brightness(uint8_t percent);

/** @brief 拖动滑块时预览亮度，不写入 NVS。 */
esp_err_t system_preview_brightness(uint8_t percent);

/**
 * @brief 获取当前亮度
 * @return 亮度百分比 (0-100)
 */
uint8_t system_get_brightness(void);

/**
 * @brief 设置音量
 * @param percent 音量百分比 (0-100)
 */
esp_err_t system_set_volume(uint8_t percent);

/** @brief 拖动音量滑块时预览，不写入 NVS。 */
esp_err_t system_preview_volume(uint8_t percent);

/**
 * @brief 获取当前音量
 * @return 音量百分比 (0-100)
 */
uint8_t system_get_volume(void);

#ifdef __cplusplus
}
#endif

#endif // SYSTEM_SETTINGS_H
