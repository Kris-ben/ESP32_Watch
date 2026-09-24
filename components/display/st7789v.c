#include "sdkconfig.h"

#include "st7789v.h"

#include <string.h>

#include "esp_rom_sys.h"

#include "esp_check.h"
#include "esp_log.h"
#include "esp_heap_caps.h"

#include "ui_font5x7.h"

static const char *TAG = "ST7789V";

// ST7789V commands
#define ST7789_SWRESET  0x01
#define ST7789_SLPOUT   0x11
#define ST7789_NORON    0x13
#define ST7789_INVOFF   0x20
#define ST7789_INVON    0x21
#define ST7789_DISPON   0x29
#define ST7789_CASET    0x2A
#define ST7789_RASET    0x2B
#define ST7789_RAMWR    0x2C
#define ST7789_MADCTL   0x36
#define ST7789_COLMOD   0x3A

static inline void delay_ms(int ms)
{
    if (ms <= 0) {
        return;
    }
    esp_rom_delay_us((uint32_t)ms * 1000U);
}

static esp_err_t st7789v_spi_tx(st7789v_t *dev, bool is_data, const void *data, size_t len)
{
    if (!dev || !dev->spi || !data || len == 0) {
        return ESP_ERR_INVALID_ARG;
    }

    gpio_set_level(dev->pin_dc, is_data ? 1 : 0);

    spi_transaction_t t;
    memset(&t, 0, sizeof(t));
    t.length = len * 8;
    t.tx_buffer = data;

    return spi_device_transmit(dev->spi, &t);
}

static esp_err_t st7789v_write_cmd(st7789v_t *dev, uint8_t cmd)
{
    return st7789v_spi_tx(dev, false, &cmd, 1);
}

static esp_err_t st7789v_write_data(st7789v_t *dev, const void *data, size_t len)
{
    return st7789v_spi_tx(dev, true, data, len);
}

static esp_err_t st7789v_reset(st7789v_t *dev)
{
    if (dev->pin_rst < 0) {
        return ESP_OK;
    }

    gpio_set_level(dev->pin_rst, 0);
    delay_ms(20);
    gpio_set_level(dev->pin_rst, 1);
    delay_ms(120);
    return ESP_OK;
}

static uint8_t madctl_from_rotation(st7789v_rotation_t rot)
{
    // MADCTL bits: MY(0x80) MX(0x40) MV(0x20) ML(0x10) RGB(0x00/0x08) MH(0x04)
    // Use RGB=0 (RGB order). If colors are swapped, set RGB bit (0x08).
    switch (rot) {
    case ST7789V_ROTATION_0:
        return 0x00;
    case ST7789V_ROTATION_90:
        return 0x60;
    case ST7789V_ROTATION_180:
        return 0xC0;
    case ST7789V_ROTATION_270:
        return 0xA0;
    case ST7789V_ROTATION_0_FLIP_Y:
        return 0x80;
    default:
        return 0x00;
    }
}

esp_err_t st7789v_set_rotation(st7789v_t *dev, st7789v_rotation_t rotation)
{
    if (!dev) {
        return ESP_ERR_INVALID_ARG;
    }

    dev->rotation = rotation;
    uint8_t madctl = (uint8_t)(madctl_from_rotation(rotation) | (dev->madctl_color_order & 0x08));
    ESP_RETURN_ON_ERROR(st7789v_write_cmd(dev, ST7789_MADCTL), TAG, "MADCTL cmd failed");
    ESP_RETURN_ON_ERROR(st7789v_write_data(dev, &madctl, 1), TAG, "MADCTL data failed");
    return ESP_OK;
}

esp_err_t st7789v_set_color_order_bgr(st7789v_t *dev, bool bgr)
{
    if (!dev) {
        return ESP_ERR_INVALID_ARG;
    }

    dev->madctl_color_order = bgr ? 0x08 : 0x00;
    // Re-apply MADCTL with current rotation
    return st7789v_set_rotation(dev, dev->rotation);
}

esp_err_t st7789v_set_inversion(st7789v_t *dev, bool invert_on)
{
    if (!dev) {
        return ESP_ERR_INVALID_ARG;
    }

    dev->invert_colors = invert_on;
    return st7789v_write_cmd(dev, invert_on ? ST7789_INVON : ST7789_INVOFF);
}

esp_err_t st7789v_set_backlight(st7789v_t *dev, bool on)
{
    if (!dev) {
        return ESP_ERR_INVALID_ARG;
    }
    if (dev->pin_bl < 0) {
        return ESP_OK;
    }
    int level_on = (dev->bl_on_level != 0) ? 1 : 0;
    gpio_set_level(dev->pin_bl, on ? level_on : (level_on ^ 1));
    return ESP_OK;
}

esp_err_t st7789v_set_window(st7789v_t *dev, uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1)
{
    if (!dev) {
        return ESP_ERR_INVALID_ARG;
    }
    if (x0 > x1 || y0 > y1) {
        return ESP_ERR_INVALID_ARG;
    }

    x0 += dev->x_offset;
    x1 += dev->x_offset;
    y0 += dev->y_offset;
    y1 += dev->y_offset;

    uint8_t data[4];

    ESP_RETURN_ON_ERROR(st7789v_write_cmd(dev, ST7789_CASET), TAG, "CASET failed");
    data[0] = (uint8_t)(x0 >> 8);
    data[1] = (uint8_t)(x0 & 0xFF);
    data[2] = (uint8_t)(x1 >> 8);
    data[3] = (uint8_t)(x1 & 0xFF);
    ESP_RETURN_ON_ERROR(st7789v_write_data(dev, data, 4), TAG, "CASET data failed");

    ESP_RETURN_ON_ERROR(st7789v_write_cmd(dev, ST7789_RASET), TAG, "RASET failed");
    data[0] = (uint8_t)(y0 >> 8);
    data[1] = (uint8_t)(y0 & 0xFF);
    data[2] = (uint8_t)(y1 >> 8);
    data[3] = (uint8_t)(y1 & 0xFF);
    ESP_RETURN_ON_ERROR(st7789v_write_data(dev, data, 4), TAG, "RASET data failed");

    ESP_RETURN_ON_ERROR(st7789v_write_cmd(dev, ST7789_RAMWR), TAG, "RAMWR failed");
    return ESP_OK;
}

esp_err_t st7789v_write_pixels(st7789v_t *dev, const void *pixels, size_t bytes)
{
    return st7789v_write_data(dev, pixels, bytes);
}

static esp_err_t st7789v_send_init_sequence(st7789v_t *dev)
{
    // ST7789V initialization sequence (16-bit RGB565 + inversion on)

    ESP_RETURN_ON_ERROR(st7789v_write_cmd(dev, ST7789_SWRESET), TAG, "SWRESET failed");
    delay_ms(150);

    ESP_RETURN_ON_ERROR(st7789v_write_cmd(dev, ST7789_SLPOUT), TAG, "SLPOUT failed");
    delay_ms(120);

    // 16-bit color
    ESP_RETURN_ON_ERROR(st7789v_write_cmd(dev, ST7789_COLMOD), TAG, "COLMOD failed");
    uint8_t colmod = 0x55;
    ESP_RETURN_ON_ERROR(st7789v_write_data(dev, &colmod, 1), TAG, "COLMOD data failed");
    delay_ms(10);

    // Porch control
    uint8_t porctrl[5] = { 0x0C, 0x0C, 0x00, 0x33, 0x33 };
    ESP_RETURN_ON_ERROR(st7789v_write_cmd(dev, 0xB2), TAG, "PORCTRL failed");
    ESP_RETURN_ON_ERROR(st7789v_write_data(dev, porctrl, sizeof(porctrl)), TAG, "PORCTRL data failed");

    // Gate control
    uint8_t gctrl = 0x35;
    ESP_RETURN_ON_ERROR(st7789v_write_cmd(dev, 0xB7), TAG, "GCTRL failed");
    ESP_RETURN_ON_ERROR(st7789v_write_data(dev, &gctrl, 1), TAG, "GCTRL data failed");

    // VCOM
    uint8_t vcom = 0x19;
    ESP_RETURN_ON_ERROR(st7789v_write_cmd(dev, 0xBB), TAG, "VCOMS failed");
    ESP_RETURN_ON_ERROR(st7789v_write_data(dev, &vcom, 1), TAG, "VCOMS data failed");

    // LCM control
    uint8_t lcmctrl = 0x2C;
    ESP_RETURN_ON_ERROR(st7789v_write_cmd(dev, 0xC0), TAG, "LCMCTRL failed");
    ESP_RETURN_ON_ERROR(st7789v_write_data(dev, &lcmctrl, 1), TAG, "LCMCTRL data failed");

    // VDV and VRH enable
    uint8_t vdvvrhen = 0x01;
    ESP_RETURN_ON_ERROR(st7789v_write_cmd(dev, 0xC2), TAG, "VDVVRHEN failed");
    ESP_RETURN_ON_ERROR(st7789v_write_data(dev, &vdvvrhen, 1), TAG, "VDVVRHEN data failed");

    // VRH set
    uint8_t vrhs = 0x12;
    ESP_RETURN_ON_ERROR(st7789v_write_cmd(dev, 0xC3), TAG, "VRHS failed");
    ESP_RETURN_ON_ERROR(st7789v_write_data(dev, &vrhs, 1), TAG, "VRHS data failed");

    // VDV set
    uint8_t vdvs = 0x20;
    ESP_RETURN_ON_ERROR(st7789v_write_cmd(dev, 0xC4), TAG, "VDVS failed");
    ESP_RETURN_ON_ERROR(st7789v_write_data(dev, &vdvs, 1), TAG, "VDVS data failed");

    // Frame rate control
    uint8_t frctrl2 = 0x0F;
    ESP_RETURN_ON_ERROR(st7789v_write_cmd(dev, 0xC6), TAG, "FRCTRL2 failed");
    ESP_RETURN_ON_ERROR(st7789v_write_data(dev, &frctrl2, 1), TAG, "FRCTRL2 data failed");

    // Power control
    uint8_t pwctrl1[2] = { 0xA4, 0xA1 };
    ESP_RETURN_ON_ERROR(st7789v_write_cmd(dev, 0xD0), TAG, "PWCTRL1 failed");
    ESP_RETURN_ON_ERROR(st7789v_write_data(dev, pwctrl1, sizeof(pwctrl1)), TAG, "PWCTRL1 data failed");

    // Gamma
    uint8_t pvgam[14] = { 0xD0, 0x04, 0x0D, 0x11, 0x13, 0x2B, 0x3F, 0x54, 0x4C, 0x18, 0x0D, 0x0B, 0x1F, 0x23 };
    uint8_t nvgam[14] = { 0xD0, 0x04, 0x0C, 0x11, 0x13, 0x2C, 0x3F, 0x44, 0x51, 0x2F, 0x1F, 0x1F, 0x20, 0x23 };
    ESP_RETURN_ON_ERROR(st7789v_write_cmd(dev, 0xE0), TAG, "PVGAMCTRL failed");
    ESP_RETURN_ON_ERROR(st7789v_write_data(dev, pvgam, sizeof(pvgam)), TAG, "PVGAMCTRL data failed");
    ESP_RETURN_ON_ERROR(st7789v_write_cmd(dev, 0xE1), TAG, "NVGAMCTRL failed");
    ESP_RETURN_ON_ERROR(st7789v_write_data(dev, nvgam, sizeof(nvgam)), TAG, "NVGAMCTRL data failed");

    // Rotation (also applies current color order bit)
    ESP_RETURN_ON_ERROR(st7789v_set_rotation(dev, dev->rotation), TAG, "set rotation failed");

    // Inversion (module dependent)
    ESP_RETURN_ON_ERROR(st7789v_set_inversion(dev, dev->invert_colors), TAG, "set inversion failed");

    ESP_RETURN_ON_ERROR(st7789v_write_cmd(dev, ST7789_NORON), TAG, "NORON failed");
    delay_ms(10);

    ESP_RETURN_ON_ERROR(st7789v_write_cmd(dev, ST7789_DISPON), TAG, "DISPON failed");
    delay_ms(100);

    return ESP_OK;
}

esp_err_t st7789v_fill_color(st7789v_t *dev, uint16_t rgb565_color)
{
    if (!dev) {
        return ESP_ERR_INVALID_ARG;
    }

    ESP_RETURN_ON_ERROR(st7789v_set_window(dev, 0, 0, dev->width - 1, dev->height - 1), TAG, "set window failed");

    // Big-endian: high byte first
    uint8_t hi = (uint8_t)(rgb565_color >> 8);
    uint8_t lo = (uint8_t)(rgb565_color & 0xFF);

    // Send in chunks to avoid large buffer
    enum { CHUNK = 4096 };
    uint8_t *buf = (uint8_t *)heap_caps_malloc(CHUNK, MALLOC_CAP_DMA);
    if (!buf) {
        return ESP_ERR_NO_MEM;
    }

    for (int i = 0; i < CHUNK; i += 2) {
        buf[i] = hi;
        buf[i + 1] = lo;
    }

    const size_t total = (size_t)dev->width * (size_t)dev->height * 2;
    size_t sent = 0;
    while (sent < total) {
        size_t n = total - sent;
        if (n > CHUNK) {
            n = CHUNK;
        }
        esp_err_t err = st7789v_write_data(dev, buf, n);
        if (err != ESP_OK) {
            heap_caps_free(buf);
            return err;
        }
        sent += n;
    }

    heap_caps_free(buf);
    return ESP_OK;
}

esp_err_t st7789v_fill_rect(st7789v_t *dev, uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t rgb565_color)
{
    if (!dev) {
        return ESP_ERR_INVALID_ARG;
    }
    if (w == 0 || h == 0) {
        return ESP_OK;
    }
    if (x >= dev->width || y >= dev->height) {
        return ESP_OK;
    }

    uint16_t x1 = (uint16_t)(x + w - 1);
    uint16_t y1 = (uint16_t)(y + h - 1);
    if (x1 >= dev->width) {
        x1 = (uint16_t)(dev->width - 1);
    }
    if (y1 >= dev->height) {
        y1 = (uint16_t)(dev->height - 1);
    }

    ESP_RETURN_ON_ERROR(st7789v_set_window(dev, x, y, x1, y1), TAG, "set window failed");

    uint8_t hi = (uint8_t)(rgb565_color >> 8);
    uint8_t lo = (uint8_t)(rgb565_color & 0xFF);

    enum { CHUNK = 4096 };
    uint8_t *buf = (uint8_t *)heap_caps_malloc(CHUNK, MALLOC_CAP_DMA);
    if (!buf) {
        return ESP_ERR_NO_MEM;
    }

    for (int i = 0; i < CHUNK; i += 2) {
        buf[i] = hi;
        buf[i + 1] = lo;
    }

    const size_t total = (size_t)(x1 - x + 1) * (size_t)(y1 - y + 1) * 2;
    size_t sent = 0;
    while (sent < total) {
        size_t n = total - sent;
        if (n > CHUNK) {
            n = CHUNK;
        }
        esp_err_t err = st7789v_write_data(dev, buf, n);
        if (err != ESP_OK) {
            heap_caps_free(buf);
            return err;
        }
        sent += n;
    }

    heap_caps_free(buf);
    return ESP_OK;
}

static esp_err_t st7789v_draw_pixel(st7789v_t *dev, uint16_t x, uint16_t y, uint16_t color)
{
    if (x >= dev->width || y >= dev->height) {
        return ESP_OK;
    }
    ESP_RETURN_ON_ERROR(st7789v_set_window(dev, x, y, x, y), TAG, "set window failed");

    uint8_t px[2] = { (uint8_t)(color >> 8), (uint8_t)(color & 0xFF) };
    return st7789v_write_data(dev, px, sizeof(px));
}

esp_err_t st7789v_draw_char_5x7(st7789v_t *dev,
                               uint16_t x,
                               uint16_t y,
                               char c,
                               uint16_t fg,
                               uint16_t bg,
                               uint8_t scale)
{
    if (!dev) {
        return ESP_ERR_INVALID_ARG;
    }
    if (scale == 0) {
        scale = 1;
    }

    const uint8_t *cols = ui_font5x7_get(c);

    // Each glyph: 5 columns, 7 rows (LSB top)
    for (uint8_t col = 0; col < 5; col++) {
        uint8_t bits = cols[col];
        for (uint8_t row = 0; row < 7; row++) {
            bool on = (bits >> row) & 0x01;
            uint16_t color = on ? fg : bg;
            for (uint8_t sx = 0; sx < scale; sx++) {
                for (uint8_t sy = 0; sy < scale; sy++) {
                    (void)st7789v_draw_pixel(dev,
                                            (uint16_t)(x + col * scale + sx),
                                            (uint16_t)(y + row * scale + sy),
                                            color);
                }
            }
        }
    }

    // one column spacing
    for (uint8_t row = 0; row < 7; row++) {
        for (uint8_t sx = 0; sx < scale; sx++) {
            for (uint8_t sy = 0; sy < scale; sy++) {
                (void)st7789v_draw_pixel(dev,
                                        (uint16_t)(x + 5 * scale + sx),
                                        (uint16_t)(y + row * scale + sy),
                                        bg);
            }
        }
    }

    return ESP_OK;
}

esp_err_t st7789v_draw_string_5x7(st7789v_t *dev,
                                 uint16_t x,
                                 uint16_t y,
                                 const char *s,
                                 uint16_t fg,
                                 uint16_t bg,
                                 uint8_t scale)
{
    if (!dev || !s) {
        return ESP_ERR_INVALID_ARG;
    }
    if (scale == 0) {
        scale = 1;
    }

    uint16_t cx = x;
    while (*s) {
        (void)st7789v_draw_char_5x7(dev, cx, y, *s, fg, bg, scale);
        cx = (uint16_t)(cx + (6U * scale));
        s++;
        if (cx >= dev->width) {
            break;
        }
    }

    return ESP_OK;
}

esp_err_t st7789v_draw_bitmap_1bpp(st7789v_t *dev,
                                  uint16_t x,
                                  uint16_t y,
                                  uint16_t w,
                                  uint16_t h,
                                  const uint8_t *bitmap,
                                  uint16_t fg,
                                  uint16_t bg,
                                  uint16_t bytes_per_row,
                                  uint8_t scale)
{
    if (!dev || !bitmap) {
        return ESP_ERR_INVALID_ARG;
    }
    if (w == 0 || h == 0 || bytes_per_row == 0) {
        return ESP_OK;
    }
    if (scale == 0) {
        scale = 1;
    }

    for (uint16_t row = 0; row < h; row++) {
        const uint8_t *src = bitmap + (size_t)row * bytes_per_row;
        for (uint16_t col = 0; col < w; col++) {
            uint8_t b = src[col >> 3];
            bool on = (b & (0x80 >> (col & 7))) != 0;
            uint16_t color = on ? fg : bg;

            uint16_t px = (uint16_t)(x + col * scale);
            uint16_t py = (uint16_t)(y + row * scale);
            for (uint8_t sx = 0; sx < scale; sx++) {
                for (uint8_t sy = 0; sy < scale; sy++) {
                    (void)st7789v_draw_pixel(dev,
                                            (uint16_t)(px + sx),
                                            (uint16_t)(py + sy),
                                            color);
                }
            }
        }
    }

    return ESP_OK;
}

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
                      int clock_hz)
{
    if (!dev) {
        return ESP_ERR_INVALID_ARG;
    }

    memset(dev, 0, sizeof(*dev));
    dev->pin_dc = (gpio_num_t)pin_dc;
    dev->pin_rst = (gpio_num_t)pin_rst;
    dev->pin_bl = (gpio_num_t)pin_bl;
    dev->bl_on_level = backlight_active_high ? 1 : 0;

    dev->width = width;
    dev->height = height;
    dev->x_offset = x_offset;
    dev->y_offset = y_offset;
    dev->rotation = rotation;

    // Defaults (can be changed after init)
    dev->madctl_color_order = 0x00; // RGB
    dev->invert_colors = true;

    gpio_config_t io_conf = {
        .intr_type = GPIO_INTR_DISABLE,
        .mode = GPIO_MODE_OUTPUT,
        .pin_bit_mask = (1ULL << (uint32_t)pin_dc) | (1ULL << (uint32_t)pin_rst),
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .pull_up_en = GPIO_PULLUP_DISABLE,
    };
    if (pin_bl >= 0) {
        io_conf.pin_bit_mask |= (1ULL << (uint32_t)pin_bl);
    }
    ESP_RETURN_ON_ERROR(gpio_config(&io_conf), TAG, "gpio_config failed");

    // SPI bus init
    spi_bus_config_t buscfg = {
        .mosi_io_num = pin_mosi,
        .miso_io_num = -1,
        .sclk_io_num = pin_sclk,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        // Must be >= the largest transaction size used by this driver.
        // We send pixel data in 4096-byte chunks in fill_rect/fill_color.
        .max_transfer_sz = 4096,
    };

    // In minimal build, SPI2_HOST might already be initialized by other components.
    // Try init but ignore "already initialized".
    esp_err_t err = spi_bus_initialize(host, &buscfg, SPI_DMA_CH_AUTO);
    if (err != ESP_OK && err != ESP_ERR_INVALID_STATE) {
        return err;
    }

    spi_device_interface_config_t devcfg = {
        .clock_speed_hz = clock_hz,
        .mode = 0,
        .spics_io_num = pin_cs,
        .queue_size = 1,
        .flags = SPI_DEVICE_NO_DUMMY,
    };

    ESP_RETURN_ON_ERROR(spi_bus_add_device(host, &devcfg, &dev->spi), TAG, "spi_bus_add_device failed");

    // Reset + init seq
    ESP_RETURN_ON_ERROR(st7789v_reset(dev), TAG, "reset failed");
    ESP_RETURN_ON_ERROR(st7789v_send_init_sequence(dev), TAG, "init sequence failed");

    // Default backlight ON for GPIO BL
    if (pin_bl >= 0) {
        (void)st7789v_set_backlight(dev, true);
    }

    ESP_LOGI(TAG, "Initialized (w=%u h=%u xoff=%u yoff=%u)", (unsigned)width, (unsigned)height, (unsigned)x_offset, (unsigned)y_offset);
    return ESP_OK;
}
