/**
 * @file ui_graphics.h
 * @brief 基础图形绘制函数
 *
 * 提供圆形、椭圆等基础图形绘制
 */

#pragma once

#include <stdint.h>
#include "st7789v.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief RGB565颜色生成宏
 */
#define UI_RGB565(r, g, b) ((uint16_t)(((r) >> 3) << 11) | (((g) >> 2) << 5) | ((b) >> 3))

void ui_draw_filled_circle(st7789v_t *lcd, int cx, int cy, int r, uint16_t color);

void ui_draw_filled_ellipse(st7789v_t *lcd, int cx, int cy, int rx, int ry, uint16_t color);

void ui_draw_parabola_arc(st7789v_t *lcd, int cx, int cy, int half_w, int height, int thickness, uint16_t color);

void ui_draw_sun_icon(st7789v_t *lcd, int cx, int cy, int radius);

#ifdef __cplusplus
}
#endif
