/**
 * @file ui_slider.h
 * @brief 通用滑动条UI组件
 *
 * 可复用的滑动条控件，支持触摸拖动
 */

#pragma once

#include <stdbool.h>
#include <stdint.h>
#include "st7789v.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    uint16_t x;
    uint16_t y;
    uint16_t width;
    uint16_t height;
    uint16_t knob_width;
    uint16_t touch_margin;

    uint16_t color_bg;
    uint16_t color_fill;
    uint16_t color_knob;
    uint16_t color_border;
    uint16_t color_highlight;
    uint16_t color_clear;
} ui_slider_config_t;

typedef struct {
    ui_slider_config_t cfg;
    uint8_t value;
    bool dragging;
    int16_t last_knob_x;
    uint16_t last_fill_width;
} ui_slider_t;

void ui_slider_init(ui_slider_t *slider, const ui_slider_config_t *cfg, uint8_t initial_value);

void ui_slider_draw(ui_slider_t *slider, st7789v_t *lcd);

bool ui_slider_hit_test(ui_slider_t *slider, uint16_t x, uint16_t y);

bool ui_slider_handle_touch(ui_slider_t *slider, uint16_t x, bool pressed);

uint8_t ui_slider_get_value(ui_slider_t *slider);

void ui_slider_set_value(ui_slider_t *slider, uint8_t value);

ui_slider_config_t ui_slider_brightness_default_config(uint16_t x, uint16_t y);

#ifdef __cplusplus
}
#endif
