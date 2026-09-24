#pragma once

#include <stdbool.h>
#include <stdint.h>

#include "esp_err.h"
#include "driver/ledc.h"

#include "st7789v.h"

typedef enum {
    DISPLAY_BACKLIGHT_NONE = 0,
    DISPLAY_BACKLIGHT_GPIO,
    DISPLAY_BACKLIGHT_LEDC,
} display_backlight_mode_t;

typedef struct {
    // SPI + panel
    spi_host_device_t host;
    int pin_sclk;
    int pin_mosi;
    int pin_cs;
    int pin_dc;
    int pin_rst;

    uint16_t width;
    uint16_t height;
    uint16_t x_offset;
    uint16_t y_offset;
    st7789v_rotation_t rotation;
    int spi_clock_hz;

    // Panel options
    bool invert_colors;
    bool color_order_bgr;

    // Backlight
    display_backlight_mode_t backlight_mode;
    int pin_bl;                  // GPIO/LEDC output pin
    bool bl_active_high;          // true: high=on, false: low=on

    // LEDC settings (used only when backlight_mode == DISPLAY_BACKLIGHT_LEDC)
    ledc_mode_t ledc_speed_mode;
    ledc_timer_t ledc_timer;
    ledc_channel_t ledc_channel;
    ledc_timer_bit_t ledc_duty_resolution;
    uint32_t ledc_freq_hz;
} display_st7789v_config_t;

typedef struct {
    st7789v_t lcd;

    // Backlight state (LEDC)
    bool bl_inited;
    uint32_t bl_max_duty;
    uint32_t bl_duty_on;
    uint32_t bl_duty_off;

    display_st7789v_config_t cfg;
} display_st7789v_t;

esp_err_t display_st7789v_init(display_st7789v_t *disp, const display_st7789v_config_t *cfg);

esp_err_t display_st7789v_backlight_set_percent(display_st7789v_t *disp, uint8_t percent);

esp_err_t display_st7789v_backlight_ramp_to(display_st7789v_t *disp, uint8_t target_percent, uint8_t step_percent, uint32_t step_delay_ms);

static inline uint16_t display_rgb565(uint8_t r, uint8_t g, uint8_t b)
{
    return rgb565(r, g, b);
}
