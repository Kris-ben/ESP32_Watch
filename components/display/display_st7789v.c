#include "sdkconfig.h"

#include "display_st7789v.h"

#include "esp_check.h"
#include "esp_rom_sys.h"

#include "driver/gpio.h"

static const char *TAG = "DISPLAY_ST7789V";

static inline void delay_ms(uint32_t ms)
{
    if (ms == 0) {
        return;
    }
    esp_rom_delay_us(ms * 1000U);
}

static esp_err_t bl_init_ledc(display_st7789v_t *disp)
{
    const display_st7789v_config_t *cfg = &disp->cfg;

    ledc_timer_config_t timer = {
        .speed_mode = cfg->ledc_speed_mode,
        .duty_resolution = cfg->ledc_duty_resolution,
        .timer_num = cfg->ledc_timer,
        .freq_hz = cfg->ledc_freq_hz,
        .clk_cfg = LEDC_AUTO_CLK,
    };
    ESP_RETURN_ON_ERROR(ledc_timer_config(&timer), TAG, "ledc_timer_config failed");

    ledc_channel_config_t ch = {
        .gpio_num = cfg->pin_bl,
        .speed_mode = cfg->ledc_speed_mode,
        .channel = cfg->ledc_channel,
        .intr_type = LEDC_INTR_DISABLE,
        .timer_sel = cfg->ledc_timer,
        .duty = 0,
        .hpoint = 0,
    };
    ESP_RETURN_ON_ERROR(ledc_channel_config(&ch), TAG, "ledc_channel_config failed");

    disp->bl_max_duty = (1U << (uint32_t)cfg->ledc_duty_resolution) - 1U;
    disp->bl_duty_on = cfg->bl_active_high ? disp->bl_max_duty : 0U;
    disp->bl_duty_off = cfg->bl_active_high ? 0U : disp->bl_max_duty;

    disp->bl_inited = true;

    // Default: backlight OFF
    ESP_RETURN_ON_ERROR(ledc_set_duty(cfg->ledc_speed_mode, cfg->ledc_channel, disp->bl_duty_off), TAG,
                        "ledc_set_duty(off) failed");
    ESP_RETURN_ON_ERROR(ledc_update_duty(cfg->ledc_speed_mode, cfg->ledc_channel), TAG, "ledc_update_duty(off) failed");

    return ESP_OK;
}

static esp_err_t bl_init_gpio(display_st7789v_t *disp)
{
    const display_st7789v_config_t *cfg = &disp->cfg;

    gpio_config_t io_conf = {
        .intr_type = GPIO_INTR_DISABLE,
        .mode = GPIO_MODE_OUTPUT,
        .pin_bit_mask = (1ULL << (uint32_t)cfg->pin_bl),
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .pull_up_en = GPIO_PULLUP_DISABLE,
    };
    ESP_RETURN_ON_ERROR(gpio_config(&io_conf), TAG, "gpio_config failed");

    // Default: OFF
    gpio_set_level((gpio_num_t)cfg->pin_bl, cfg->bl_active_high ? 0 : 1);
    disp->bl_inited = true;
    return ESP_OK;
}

esp_err_t display_st7789v_init(display_st7789v_t *disp, const display_st7789v_config_t *cfg)
{
    if (!disp || !cfg) {
        return ESP_ERR_INVALID_ARG;
    }
    *disp = (display_st7789v_t){ 0 };
    disp->cfg = *cfg;

    // Backlight init first (keep OFF to reduce inrush)
    if (cfg->backlight_mode == DISPLAY_BACKLIGHT_LEDC) {
        ESP_RETURN_ON_ERROR(bl_init_ledc(disp), TAG, "bl_init_ledc failed");
    } else if (cfg->backlight_mode == DISPLAY_BACKLIGHT_GPIO) {
        ESP_RETURN_ON_ERROR(bl_init_gpio(disp), TAG, "bl_init_gpio failed");
    } else {
        disp->bl_inited = false;
    }

    // Init panel
    const int pin_bl_for_panel = (cfg->backlight_mode == DISPLAY_BACKLIGHT_GPIO) ? cfg->pin_bl : -1;

    esp_err_t err = st7789v_init(&disp->lcd,
                                cfg->host,
                                cfg->pin_sclk,
                                cfg->pin_mosi,
                                cfg->pin_cs,
                                cfg->pin_dc,
                                cfg->pin_rst,
                                pin_bl_for_panel,
                                cfg->bl_active_high,
                                cfg->width,
                                cfg->height,
                                cfg->x_offset,
                                cfg->y_offset,
                                cfg->rotation,
                                cfg->spi_clock_hz);

    ESP_RETURN_ON_ERROR(err, TAG, "st7789v_init failed");

    // Apply panel options (module dependent)
    ESP_RETURN_ON_ERROR(st7789v_set_color_order_bgr(&disp->lcd, cfg->color_order_bgr), TAG, "set color order failed");
    ESP_RETURN_ON_ERROR(st7789v_set_inversion(&disp->lcd, cfg->invert_colors), TAG, "set inversion failed");

    return ESP_OK;
}

esp_err_t display_st7789v_backlight_set_percent(display_st7789v_t *disp, uint8_t percent)
{
    if (!disp) {
        return ESP_ERR_INVALID_ARG;
    }

    if (percent > 100) {
        percent = 100;
    }

    const display_st7789v_config_t *cfg = &disp->cfg;

    if (cfg->backlight_mode == DISPLAY_BACKLIGHT_NONE) {
        return ESP_OK;
    }

    if (!disp->bl_inited) {
        if (cfg->backlight_mode == DISPLAY_BACKLIGHT_LEDC) {
            ESP_RETURN_ON_ERROR(bl_init_ledc(disp), TAG, "bl_init_ledc failed");
        } else if (cfg->backlight_mode == DISPLAY_BACKLIGHT_GPIO) {
            ESP_RETURN_ON_ERROR(bl_init_gpio(disp), TAG, "bl_init_gpio failed");
        }
    }

    if (cfg->backlight_mode == DISPLAY_BACKLIGHT_GPIO) {
        gpio_set_level((gpio_num_t)cfg->pin_bl, cfg->bl_active_high ? (percent > 0) : !(percent > 0));
        return ESP_OK;
    }

    // LEDC
    uint32_t duty;
    if (cfg->bl_active_high) {
        duty = (disp->bl_max_duty * (uint32_t)percent) / 100U;
    } else {
        duty = disp->bl_max_duty - ((disp->bl_max_duty * (uint32_t)percent) / 100U);
    }

    ESP_RETURN_ON_ERROR(ledc_set_duty(cfg->ledc_speed_mode, cfg->ledc_channel, duty), TAG, "ledc_set_duty failed");
    ESP_RETURN_ON_ERROR(ledc_update_duty(cfg->ledc_speed_mode, cfg->ledc_channel), TAG, "ledc_update_duty failed");

    return ESP_OK;
}

esp_err_t display_st7789v_backlight_ramp_to(display_st7789v_t *disp, uint8_t target_percent, uint8_t step_percent, uint32_t step_delay_ms)
{
    if (!disp) {
        return ESP_ERR_INVALID_ARG;
    }

    if (step_percent == 0) {
        step_percent = 5;
    }
    if (target_percent > 100) {
        target_percent = 100;
    }

    for (uint8_t p = 0; p <= target_percent; p = (uint8_t)(p + step_percent)) {
        ESP_RETURN_ON_ERROR(display_st7789v_backlight_set_percent(disp, p), TAG, "set backlight failed");
        delay_ms(step_delay_ms);
        if (p > (uint8_t)(100 - step_percent)) {
            break;
        }
    }

    return ESP_OK;
}
