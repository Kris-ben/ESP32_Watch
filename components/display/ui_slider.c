/**
 * @file ui_slider.c
 * @brief 通用滑动条UI组件实现
 */

#include "ui_slider.h"
#include <string.h>

/**
 * @brief RGB565颜色辅助宏
 */
#define RGB565(r, g, b) ((uint16_t)(((r) >> 3) << 11) | (((g) >> 2) << 5) | ((b) >> 3))

void ui_slider_init(ui_slider_t *slider, const ui_slider_config_t *cfg, uint8_t initial_value)
{
    if (!slider || !cfg) return;

    memcpy(&slider->cfg, cfg, sizeof(ui_slider_config_t));
    slider->value = (initial_value > 100) ? 100 : initial_value;
    slider->dragging = false;
    slider->last_knob_x = -1;      // 未初始化
    slider->last_fill_width = 0;
}

void ui_slider_draw(ui_slider_t *slider, st7789v_t *lcd)
{
    if (!slider || !lcd) return;

    const ui_slider_config_t *c = &slider->cfg;

    // 计算填充宽度
    uint16_t fill_width = (uint16_t)((c->width * slider->value) / 100);

    // 计算手柄位置
    int knob_x = c->x + fill_width - c->knob_width / 2;
    if (knob_x < (int)c->x) knob_x = c->x;
    if (knob_x > (int)(c->x + c->width - c->knob_width)) {
        knob_x = c->x + c->width - c->knob_width;
    }

    // 首次绘制：完整绘制
    if (slider->last_knob_x < 0) {
        // 绘制滑动条边框
        st7789v_fill_rect(lcd, c->x - 1, c->y - 1,
                          c->width + 2, c->height + 2, c->color_border);

        // 绘制已填充部分(蓝色)
        if (fill_width > 0) {
            st7789v_fill_rect(lcd, c->x, c->y, fill_width, c->height, c->color_fill);
        }

        // 绘制未填充部分(灰色背景)
        uint16_t remain_width = c->width - fill_width;
        if (remain_width > 0) {
            st7789v_fill_rect(lcd, c->x + fill_width, c->y,
                              remain_width, c->height, c->color_bg);
        }
    } else {
        // 增量更新：只更新变化的区域
        int old_knob_x = slider->last_knob_x;

        // 计算需要更新的区域 (旧手柄位置到新手柄位置之间)
        int update_left = (old_knob_x < knob_x) ? old_knob_x - 1 : knob_x - 1;
        int update_right = (old_knob_x > knob_x) ?
                           (old_knob_x + c->knob_width + 1) : (knob_x + c->knob_width + 1);

        // 确保边界
        if (update_left < (int)(c->x - 1)) update_left = c->x - 1;
        if (update_right > (int)(c->x + c->width + 1)) update_right = c->x + c->width + 1;

        uint16_t update_width = (uint16_t)(update_right - update_left);

        // 清除旧手柄上下区域
        st7789v_fill_rect(lcd, (uint16_t)update_left, c->y - 3,
                          update_width, 2, c->color_clear);
        st7789v_fill_rect(lcd, (uint16_t)update_left, c->y + c->height + 1,
                          update_width, 2, c->color_clear);

        // 重绘边框（只更新区域）
        st7789v_fill_rect(lcd, (uint16_t)update_left, c->y - 1,
                          update_width, 1, c->color_border);
        st7789v_fill_rect(lcd, (uint16_t)update_left, c->y + c->height,
                          update_width, 1, c->color_border);

        // 重绘填充部分和背景（只在变化区域）
        // 填充部分
        int fill_left = update_left;
        if (fill_left < (int)c->x) fill_left = c->x;
        int fill_right = (int)(c->x + fill_width);
        if (fill_right > update_right) fill_right = update_right;
        if (fill_right > fill_left) {
            st7789v_fill_rect(lcd, (uint16_t)fill_left, c->y,
                              (uint16_t)(fill_right - fill_left), c->height, c->color_fill);
        }

        // 背景部分
        int bg_left = (int)(c->x + fill_width);
        if (bg_left < update_left) bg_left = update_left;
        int bg_right = update_right;
        if (bg_right > (int)(c->x + c->width)) bg_right = c->x + c->width;
        if (bg_right > bg_left) {
            st7789v_fill_rect(lcd, (uint16_t)bg_left, c->y,
                              (uint16_t)(bg_right - bg_left), c->height, c->color_bg);
        }
    }

    // 绘制新手柄
    st7789v_fill_rect(lcd, (uint16_t)knob_x - 1, c->y - 3,
                      c->knob_width + 2, c->height + 6, c->color_border);
    st7789v_fill_rect(lcd, (uint16_t)knob_x, c->y - 2,
                      c->knob_width, c->height + 4, c->color_knob);
    st7789v_fill_rect(lcd, (uint16_t)knob_x + 2, c->y,
                      2, c->height, c->color_highlight);

    // 保存当前状态
    slider->last_knob_x = (int16_t)knob_x;
    slider->last_fill_width = fill_width;
}

bool ui_slider_hit_test(ui_slider_t *slider, uint16_t x, uint16_t y)
{
    if (!slider) return false;

    const ui_slider_config_t *c = &slider->cfg;

    return (x >= c->x - c->knob_width &&
            x <= c->x + c->width + c->knob_width &&
            y >= c->y - c->touch_margin &&
            y <= c->y + c->height + c->touch_margin);
}

bool ui_slider_handle_touch(ui_slider_t *slider, uint16_t x, bool pressed)
{
    if (!slider) return false;

    if (!pressed) {
        slider->dragging = false;
        return false;
    }

    slider->dragging = true;

    // 计算新值
    uint8_t new_value;
    if (x <= slider->cfg.x) {
        new_value = 0;
    } else if (x >= slider->cfg.x + slider->cfg.width) {
        new_value = 100;
    } else {
        new_value = (uint8_t)(((x - slider->cfg.x) * 100) / slider->cfg.width);
    }

    if (new_value != slider->value) {
        slider->value = new_value;
        return true;  // 值变化
    }

    return false;
}

uint8_t ui_slider_get_value(ui_slider_t *slider)
{
    return slider ? slider->value : 0;
}

void ui_slider_set_value(ui_slider_t *slider, uint8_t value)
{
    if (!slider) return;
    slider->value = (value > 100) ? 100 : value;
}

ui_slider_config_t ui_slider_brightness_default_config(uint16_t x, uint16_t y)
{
    return (ui_slider_config_t) {
        .x = x,
        .y = y,
        .width = 180,
        .height = 20,
        .knob_width = 16,
        .touch_margin = 15,
        .color_bg = RGB565(200, 200, 200),
        .color_fill = RGB565(80, 180, 255),
        .color_knob = RGB565(50, 130, 220),
        .color_border = RGB565(100, 100, 100),
        .color_highlight = RGB565(255, 255, 255),
        .color_clear = RGB565(255, 255, 255),
    };
}
