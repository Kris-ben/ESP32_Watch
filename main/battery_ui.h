/**
 * @file battery_ui.h
 * @brief 电池电量UI显示模块
 * 
 * 功能：定期读取电池电量，更新 screen_home 和 screen_weather
 *       上的 list_bettery 控件的图标和百分比文本
 * 
 * 图标分配：
 *   80-100%  → BATTERY_FULL
 *   60-79%   → BATTERY_3
 *   40-59%   → BATTERY_2
 *   20-39%   → BATTERY_1
 *   0-19%    → BATTERY_EMPTY
 * 
 * 充电指示：电压 > 4150mV 时按钮背景变为绿色
 */

#ifndef BATTERY_UI_H
#define BATTERY_UI_H

#include "gui_guider.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief 初始化电池UI模块并启动刷新任务
 * 需要在 LVGL UI 初始化完成后调用
 */
bool battery_ui_init(lv_ui *ui);

/**
 * @brief 立即刷新电池UI（不加锁，在 LVGL 线程中调用）
 */
void battery_ui_update_nolock(lv_ui *ui);

/**
 * @brief 立即刷新电池UI（加锁，可在任意任务中调用）
 */
void battery_ui_update(lv_ui *ui);

#ifdef __cplusplus
}
#endif

#endif // BATTERY_UI_H
