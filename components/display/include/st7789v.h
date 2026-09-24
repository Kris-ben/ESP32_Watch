#pragma once

#include <stdbool.h>
#include <stdint.h>

#include "esp_err.h"
#include "driver/spi_master.h"
#include "driver/gpio.h"

typedef enum {
    ST7789V_ROTATION_0 = 0,
    ST7789V_ROTATION_90,
    ST7789V_ROTATION_180,
    ST7789V_ROTATION_270,
    // Mirror vertically (up/down flip) while keeping X direction unchanged
    ST7789V_ROTATION_0_FLIP_Y,
} st7789v_rotation_t;

typedef struct {
    spi_device_handle_t spi;
    gpio_num_t pin_dc;
    gpio_num_t pin_rst;
    gpio_num_t pin_bl;
    uint8_t bl_on_level;

    uint16_t width;
    uint16_t height;
    uint16_t x_offset;
    uint16_t y_offset;
    st7789v_rotation_t rotation;

    // Panel quirks/config
    // Some modules require BGR color order (MADCTL bit 0x08) and/or inversion off.
    uint8_t madctl_color_order; // 0x00 (RGB) or 0x08 (BGR)
    bool invert_colors;
} st7789v_t;

esp_err_t st7789v_init(st7789v_t *dev,
                      spi_host_device_t host,
                      int pin_sclk,
                      int pin_mosi,
                      int pin_cs,
                      int pin_dc,
                      int pin_rst,
                      int pin_bl,
                      bool backlight_active_high,
                      uint16_t width,
                      uint16_t height,
                      uint16_t x_offset,
                      uint16_t y_offset,
                      st7789v_rotation_t rotation,
                      int clock_hz);

esp_err_t st7789v_set_backlight(st7789v_t *dev, bool on);

esp_err_t st7789v_set_rotation(st7789v_t *dev, st7789v_rotation_t rotation);

esp_err_t st7789v_set_color_order_bgr(st7789v_t *dev, bool bgr);

esp_err_t st7789v_set_inversion(st7789v_t *dev, bool invert_on);

esp_err_t st7789v_set_window(st7789v_t *dev, uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1);

esp_err_t st7789v_write_pixels(st7789v_t *dev, const void *pixels, size_t bytes);

esp_err_t st7789v_fill_color(st7789v_t *dev, uint16_t rgb565);

esp_err_t st7789v_fill_rect(st7789v_t *dev, uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t rgb565);

esp_err_t st7789v_draw_char_5x7(st7789v_t *dev,
                               uint16_t x,
                               uint16_t y,
                               char c,
                               uint16_t fg,
                               uint16_t bg,
                               uint8_t scale);

esp_err_t st7789v_draw_string_5x7(st7789v_t *dev,
                                 uint16_t x,
                                 uint16_t y,
                                 const char *s,
                                 uint16_t fg,
                                 uint16_t bg,
                                 uint8_t scale);

// Draw a 1bpp bitmap where each row is packed MSB-first.
// bytes_per_row must be >= (w+7)/8.
esp_err_t st7789v_draw_bitmap_1bpp(st7789v_t *dev,
                                  uint16_t x,
                                  uint16_t y,
                                  uint16_t w,
                                  uint16_t h,
                                  const uint8_t *bitmap,
                                  uint16_t fg,
                                  uint16_t bg,
                                  uint16_t bytes_per_row,
                                  uint8_t scale);

static inline uint16_t rgb565(uint8_t r, uint8_t g, uint8_t b)
{
    return (uint16_t)(((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3));
}
