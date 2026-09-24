/**
 * @file alarm_clock.h
 * @brief 闹钟功能模块
 */

#ifndef ALARM_CLOCK_H
#define ALARM_CLOCK_H

#include <stdint.h>
#include <stdbool.h>
#include "esp_err.h"
#include "gui_guider.h"

#ifdef __cplusplus
extern "C" {
#endif

#define ALARM_COUNT 4  // 支持4个闹钟

// 闹钟结构
typedef struct {
    uint8_t hour;      // 小时 (0-23)
    uint8_t minute;    // 分钟 (0-59)
    bool enabled;      // 是否启用
} alarm_time_t;

/**
 * @brief 初始化闹钟模块
 */
esp_err_t alarm_clock_init(void);

/**
 * @brief 启动闹钟检查任务
 */
esp_err_t alarm_clock_start(void);

/**
 * @brief 设置当前正在编辑的闹钟索引
 * @param index 闹钟索引 (0-3)
 */
void alarm_set_editing_index(int index);

/**
 * @brief 获取当前正在编辑的闹钟索引
 * @return 闹钟索引 (0-3)
 */
int alarm_get_editing_index(void);

/**
 * @brief 设置闹钟时间
 * @param index 闹钟索引 (0-3)
 * @param hour 小时 (0-23)
 * @param minute 分钟 (0-59)
 */
esp_err_t alarm_set_time(int index, uint8_t hour, uint8_t minute);

/**
 * @brief 获取闹钟时间
 * @param index 闹钟索引 (0-3)
 * @param hour 返回小时
 * @param minute 返回分钟
 */
esp_err_t alarm_get_time(int index, uint8_t *hour, uint8_t *minute);

/**
 * @brief 启用/禁用闹钟
 * @param index 闹钟索引 (0-3)
 * @param enabled 是否启用
 */
esp_err_t alarm_set_enabled(int index, bool enabled);

/**
 * @brief 获取闹钟启用状态
 * @param index 闹钟索引 (0-3)
 * @return 是否启用
 */
bool alarm_is_enabled(int index);

/**
 * @brief 更新screen_clock界面上的闹钟时间显示
 * @param ui UI句柄
 */
void alarm_update_ui(lv_ui *ui);

/**
 * @brief 从roller获取时间并保存到当前编辑的闹钟
 * @param ui UI句柄
 */
void alarm_save_from_roller(lv_ui *ui);

/**
 * @brief 停止当前正在响铃的闹钟
 */
void alarm_stop_ringing(void);

/**
 * @brief 注册screen_clock界面的闹钟事件
 * @param ui UI句柄
 */
void alarm_register_clock_events(lv_ui *ui);

/**
 * @brief 注册screen_set_clock界面的确定按钮事件
 * @param ui UI句柄
 */
void alarm_register_set_clock_events(lv_ui *ui);

#ifdef __cplusplus
}
#endif

#endif // ALARM_CLOCK_H
