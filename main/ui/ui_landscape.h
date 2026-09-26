#pragma once

#include "gui_guider.h"

// 将现有 240x284 页面布局调整到 284x240 的横屏画布；每页只处理一次。
void ui_landscape_apply(lv_ui *ui, lv_obj_t *screen);

// 动态控件在页面转换后刷新坐标时使用相同的换算比例。
int32_t ui_landscape_scale_x(int32_t value);
int32_t ui_landscape_scale_y(int32_t value);

// 用于非全屏弹窗，连同弹窗自身的位置与尺寸一起调整。
void ui_landscape_apply_widget(lv_obj_t *widget);
