#include "board_display.h"

#include "board_display_config.h"

#include "esp_check.h"
#include "esp_log.h"

static const char *TAG = "BOARD_DISPLAY";

static display_st7789v_t g_display;
static cst816t_t g_touch;
static bool g_touch_inited = false;

esp_err_t board_display_init(void)
{
    display_st7789v_config_t cfg = {
        .host = BOARD_LCD_SPI_HOST,
        .pin_sclk = BOARD_LCD_PIN_SCLK,
        .pin_mosi = BOARD_LCD_PIN_MOSI,
        .pin_cs = BOARD_LCD_PIN_CS,
        .pin_dc = BOARD_LCD_PIN_DC,
        .pin_rst = BOARD_LCD_PIN_RST,
        .pin_bl = BOARD_LCD_PIN_BL,

        .width = BOARD_LCD_WIDTH,
        .height = BOARD_LCD_HEIGHT,
        .x_offset = BOARD_LCD_X_OFFSET,
        .y_offset = BOARD_LCD_Y_OFFSET,
        .rotation = ST7789V_ROTATION_0,
        .spi_clock_hz = BOARD_LCD_SPI_FREQ_HZ,

        .invert_colors = (BOARD_LCD_INVERT_COLORS != 0),
        .color_order_bgr = (BOARD_LCD_COLOR_ORDER_BGR != 0),

#if BOARD_LCD_BL_USE_LEDC
        .backlight_mode = DISPLAY_BACKLIGHT_LEDC,
        .bl_active_high = true,
        .ledc_speed_mode = LEDC_LOW_SPEED_MODE,
        .ledc_timer = LEDC_TIMER_0,
        .ledc_channel = LEDC_CHANNEL_0,
        .ledc_duty_resolution = LEDC_TIMER_10_BIT,
        .ledc_freq_hz = 5000,
#else
        .backlight_mode = DISPLAY_BACKLIGHT_GPIO,
        .bl_active_high = true,
#endif
    };

    ESP_RETURN_ON_ERROR(display_st7789v_init(&g_display, &cfg), TAG, "display_st7789v_init failed");
    ESP_RETURN_ON_ERROR(display_st7789v_backlight_set_percent(&g_display, 100), TAG, "backlight set failed");

    ESP_LOGI(TAG, "LCD init ok (%ux%u)", (unsigned)BOARD_LCD_WIDTH, (unsigned)BOARD_LCD_HEIGHT);
    return ESP_OK;
}

st7789v_t *board_display_get_lcd(void)
{
    return &g_display.lcd;
}

display_st7789v_t *board_display_get_display(void)
{
    return &g_display;
}

esp_err_t board_touch_init(void)
{
    st7789v_t *lcd = board_display_get_lcd();

    cst816t_config_t touch_cfg = {
        .i2c_port = BOARD_TOUCH_I2C_PORT,
        .pin_sda = BOARD_TOUCH_PIN_SDA,
        .pin_scl = BOARD_TOUCH_PIN_SCL,
        .pin_rst = BOARD_TOUCH_PIN_RST,
        .pin_int = BOARD_TOUCH_PIN_INT,
        .i2c_freq_hz = BOARD_TOUCH_I2C_FREQ_HZ,
        .screen_width = lcd ? lcd->width : BOARD_LCD_WIDTH,
        .screen_height = lcd ? lcd->height : BOARD_LCD_HEIGHT,
        .swap_xy = (BOARD_TOUCH_SWAP_XY != 0),
        .mirror_x = (BOARD_TOUCH_MIRROR_X != 0),
        .mirror_y = (BOARD_TOUCH_MIRROR_Y != 0),
    };

    esp_err_t err = cst816t_init(&g_touch, &touch_cfg);
    if (err == ESP_OK) {
        // 界面持续使用触摸轮询；关闭芯片空闲后自动进入低功耗，避免首个点击/滑动丢失。
        esp_err_t sleep_err = cst816t_disable_auto_sleep(&g_touch, true);
        if (sleep_err != ESP_OK) {
            ESP_LOGW(TAG, "Touch disable auto sleep failed: %s", esp_err_to_name(sleep_err));
        } else {
            ESP_LOGI(TAG, "Touch auto sleep disabled");
        }
        g_touch_inited = true;
        ESP_LOGI(TAG, "Touch init ok");
    } else {
        g_touch_inited = false;
        ESP_LOGW(TAG, "Touch init failed: %s", esp_err_to_name(err));
    }

    return err;
}

bool board_touch_is_initialized(void)
{
    return g_touch_inited;
}

esp_err_t board_touch_read(cst816t_touch_data_t *data)
{
    if (!g_touch_inited) {
        return ESP_ERR_INVALID_STATE;
    }
    return cst816t_read_touch(&g_touch, data);
}
