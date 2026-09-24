#pragma once

#include "lvgl.h"

/* 整机暖琥珀配色；只定义颜色，不改变图片资源、控件几何或交互。 */
#define APP_THEME_BG       0x171a18
#define APP_THEME_SURFACE  0x24241c
#define APP_THEME_CARD     0x2b3028
#define APP_THEME_RAISED   0x3b4134
#define APP_THEME_BORDER   0x655038
#define APP_THEME_ACCENT   0xe5ac62
#define APP_THEME_TEXT     0xf8f3e8
#define APP_THEME_MUTED    0xb7b9aa
#define APP_THEME_ON_ACCENT APP_THEME_BG
#define APP_THEME_ERROR    0xf28585

/* LVGL 初始化后、创建页面前调用；在持有 LVGL 锁的 UI 上下文执行。 */
void app_theme_init(void);
