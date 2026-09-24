/**
 * @file battery_ui.h
 * @brief 电池电量UI显示模块
 * 
 * 功能：定期读取电池电压，在各页面显示估算电量的图标和百分比。
 * 当前硬件没有充电状态检测，充电时的百分比仅供参考。
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
 * @brief 按估算电量选择状态栏电池图标；负数表示读数不可用。
 */
const char *battery_ui_symbol_for_percentage(int percentage);

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
