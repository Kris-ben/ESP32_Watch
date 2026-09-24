/**
 * @file ui_graphics.c
 * @brief 基础图形绘制函数实现
 */

#include "ui_graphics.h"

void ui_draw_filled_circle(st7789v_t *lcd, int cx, int cy, int r, uint16_t color)
{
    if (!lcd || r <= 0) return;

    const int r2 = r * r;
    for (int dy = -r; dy <= r; dy++) {
        const int y = cy + dy;
        int x_extent = 0;

        while ((x_extent + 1) * (x_extent + 1) + dy * dy <= r2) {
            x_extent++;
        }

        const int x0 = cx - x_extent;
        const int w = x_extent * 2 + 1;
        if (w > 0) {
            st7789v_fill_rect(lcd, (uint16_t)x0, (uint16_t)y, (uint16_t)w, 1, color);
        }
    }
}

void ui_draw_filled_ellipse(st7789v_t *lcd, int cx, int cy, int rx, int ry, uint16_t color)
{
    if (!lcd || rx <= 0 || ry <= 0) return;

    const int rx2 = rx * rx;
    const int ry2 = ry * ry;

    for (int dy = -ry; dy <= ry; dy++) {
        const int y = cy + dy;
        int x_extent = 0;

        const int left = dy * dy * rx2;
        const int limit = rx2 * ry2;
        while ((x_extent + 1) * (x_extent + 1) * ry2 + left <= limit) {
            x_extent++;
        }

        const int x0 = cx - x_extent;
        const int w = x_extent * 2 + 1;
        if (w > 0) {
            st7789v_fill_rect(lcd, (uint16_t)x0, (uint16_t)y, (uint16_t)w, 1, color);
        }
    }
}

void ui_draw_parabola_arc(st7789v_t *lcd, int cx, int cy, int half_w, int height, int thickness, uint16_t color)
{
    if (!lcd || half_w <= 0 || height <= 0 || thickness <= 0) return;

    const int w2 = half_w * half_w;
    for (int dx = -half_w; dx <= half_w; dx++) {
        const int y = cy - height + (dx * dx * height) / w2;
        st7789v_fill_rect(lcd, (uint16_t)(cx + dx), (uint16_t)y, 1, (uint16_t)thickness, color);
    }
}

void ui_draw_sun_icon(st7789v_t *lcd, int cx, int cy, int radius)
{
    if (!lcd || radius <= 0) return;

    uint16_t sun_color = UI_RGB565(255, 220, 0);
    uint16_t core_color = UI_RGB565(255, 200, 0);

    // 太阳中心
    ui_draw_filled_circle(lcd, cx, cy, radius, sun_color);
    ui_draw_filled_circle(lcd, cx, cy, radius - 1, core_color);

    // 太阳光芒 (8个方向)
    const int ray_dist = radius + 3;
    const int dx_table[] = {0, 1, 1, 1, 0, -1, -1, -1};
    const int dy_table[] = {-1, -1, 0, 1, 1, 1, 0, -1};

    for (int i = 0; i < 8; i++) {
        int ray_x = cx + dx_table[i] * ray_dist;
        int ray_y = cy + dy_table[i] * ray_dist;
        st7789v_fill_rect(lcd, (uint16_t)ray_x, (uint16_t)ray_y, 2, 2, sun_color);
    }
}
