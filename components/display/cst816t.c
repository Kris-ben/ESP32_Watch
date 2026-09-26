#include "cst816t.h"

#include <string.h>

#include "esp_log.h"
#include "esp_check.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"

static const char *TAG = "cst816t";

static esp_err_t cst816t_i2c_init_if_needed(const cst816t_config_t *cfg)
{
    // 注意：这里默认使用legacy i2c driver API（driver/i2c.h）
    // 若你的工程已切换到i2c master new driver，可再做适配。
    i2c_config_t conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = cfg->pin_sda,
        .scl_io_num = cfg->pin_scl,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = cfg->i2c_freq_hz,
        .clk_flags = 0,
    };

    esp_err_t err = i2c_param_config(cfg->i2c_port, &conf);
    if (err != ESP_OK) {
        return err;
    }

    // 如果重复install会返回ESP_ERR_INVALID_STATE；这里视为OK
    err = i2c_driver_install(cfg->i2c_port, conf.mode, 0, 0, 0);
    if (err == ESP_ERR_INVALID_STATE) {
        return ESP_OK;
    }
    return err;
}

static esp_err_t cst816t_i2c_read(const cst816t_t *dev, uint8_t reg, uint8_t *data, size_t len)
{
    if (!dev || !data || len == 0) {
        return ESP_ERR_INVALID_ARG;
    }

    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (CST816T_I2C_ADDR << 1) | I2C_MASTER_WRITE, true);
    i2c_master_write_byte(cmd, reg, true);
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (CST816T_I2C_ADDR << 1) | I2C_MASTER_READ, true);
    if (len > 1) {
        i2c_master_read(cmd, data, len - 1, I2C_MASTER_ACK);
    }
    i2c_master_read_byte(cmd, data + len - 1, I2C_MASTER_NACK);
    i2c_master_stop(cmd);

    esp_err_t err = i2c_master_cmd_begin(dev->cfg.i2c_port, cmd, pdMS_TO_TICKS(50));
    i2c_cmd_link_delete(cmd);
    return err;
}

static esp_err_t cst816t_i2c_write(const cst816t_t *dev, uint8_t reg, uint8_t value)
{
    if (!dev) {
        return ESP_ERR_INVALID_ARG;
    }

    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (CST816T_I2C_ADDR << 1) | I2C_MASTER_WRITE, true);
    i2c_master_write_byte(cmd, reg, true);
    i2c_master_write_byte(cmd, value, true);
    i2c_master_stop(cmd);

    esp_err_t err = i2c_master_cmd_begin(dev->cfg.i2c_port, cmd, pdMS_TO_TICKS(50));
    i2c_cmd_link_delete(cmd);
    return err;
}

static inline void cst816t_apply_transform(const cst816t_t *dev, uint16_t *x, uint16_t *y)
{
    if (!dev || !x || !y) {
        return;
    }

    uint16_t tx = *x;
    uint16_t ty = *y;

    if (dev->cfg.swap_xy) {
        uint16_t tmp = tx;
        tx = ty;
        ty = tmp;
    }

    if (dev->cfg.mirror_x) {
        if (tx < dev->cfg.screen_width) {
            tx = (dev->cfg.screen_width - 1) - tx;
        }
    }

    if (dev->cfg.mirror_y) {
        if (ty < dev->cfg.screen_height) {
            ty = (dev->cfg.screen_height - 1) - ty;
        }
    }

    *x = tx;
    *y = ty;
}

esp_err_t cst816t_read_reg(cst816t_t *dev, uint8_t reg, uint8_t *data, size_t len)
{
    if (!dev) {
        return ESP_ERR_INVALID_ARG;
    }
    return cst816t_i2c_read(dev, reg, data, len);
}

esp_err_t cst816t_write_reg(cst816t_t *dev, uint8_t reg, uint8_t data)
{
    if (!dev) {
        return ESP_ERR_INVALID_ARG;
    }
    return cst816t_i2c_write(dev, reg, data);
}

esp_err_t cst816t_reset(cst816t_t *dev)
{
    if (!dev) {
        return ESP_ERR_INVALID_ARG;
    }

    if (dev->cfg.pin_rst < 0) {
        // 没有RST引脚，跳过硬复位
        return ESP_OK;
    }

    gpio_config_t io_conf = {
        .pin_bit_mask = 1ULL << dev->cfg.pin_rst,
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE,
    };
    ESP_RETURN_ON_ERROR(gpio_config(&io_conf), TAG, "gpio rst config failed");

    gpio_set_level(dev->cfg.pin_rst, 0);
    vTaskDelay(pdMS_TO_TICKS(10));
    gpio_set_level(dev->cfg.pin_rst, 1);
    vTaskDelay(pdMS_TO_TICKS(50));

    return ESP_OK;
}

esp_err_t cst816t_init(cst816t_t *dev, const cst816t_config_t *cfg)
{
    if (!dev || !cfg) {
        return ESP_ERR_INVALID_ARG;
    }

    memset(dev, 0, sizeof(*dev));
    dev->cfg = *cfg;

    ESP_RETURN_ON_ERROR(cst816t_i2c_init_if_needed(cfg), TAG, "i2c init failed");

    if (cfg->pin_int >= 0) {
        gpio_config_t io_conf = {
            .pin_bit_mask = 1ULL << cfg->pin_int,
            .mode = GPIO_MODE_INPUT,
            .pull_up_en = GPIO_PULLUP_ENABLE,
            .pull_down_en = GPIO_PULLDOWN_DISABLE,
            .intr_type = GPIO_INTR_DISABLE,
        };
        ESP_RETURN_ON_ERROR(gpio_config(&io_conf), TAG, "gpio int config failed");
    }

    // 可选硬复位
    cst816t_reset(dev);

    uint8_t chip = 0;
    esp_err_t err = cst816t_i2c_read(dev, CST816T_REG_CHIP_ID, &chip, 1);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "read chip id failed: %s", esp_err_to_name(err));
        return err;
    }

    dev->chip_id = chip;
    (void)cst816t_i2c_read(dev, CST816T_REG_PROJ_ID, &dev->project_id, 1);
    (void)cst816t_i2c_read(dev, CST816T_REG_FW_VERSION, &dev->fw_version, 1);

    if (dev->chip_id != CST816T_CHIP_ID) {
        ESP_LOGW(TAG, "unexpected chip id: 0x%02X (expect 0x%02X)", dev->chip_id, CST816T_CHIP_ID);
        // 不强制失败，有些批次可能不一致；留给上层决定
    }

    dev->initialized = true;
    ESP_LOGI(TAG, "init ok: chip=0x%02X proj=0x%02X fw=0x%02X", dev->chip_id, dev->project_id, dev->fw_version);
    return ESP_OK;
}

esp_err_t cst816t_deinit(cst816t_t *dev)
{
    if (!dev) {
        return ESP_ERR_INVALID_ARG;
    }
    dev->initialized = false;
    return ESP_OK;
}

esp_err_t cst816t_get_info(cst816t_t *dev, uint8_t *chip_id, uint8_t *project_id, uint8_t *fw_version)
{
    if (!dev) {
        return ESP_ERR_INVALID_ARG;
    }
    if (chip_id) {
        *chip_id = dev->chip_id;
    }
    if (project_id) {
        *project_id = dev->project_id;
    }
    if (fw_version) {
        *fw_version = dev->fw_version;
    }
    return ESP_OK;
}

esp_err_t cst816t_set_irq_mode(cst816t_t *dev, uint8_t mode)
{
    if (!dev) {
        return ESP_ERR_INVALID_ARG;
    }
    return cst816t_i2c_write(dev, CST816T_REG_IRQ_CTL, mode);
}

esp_err_t cst816t_set_motion_mask(cst816t_t *dev, uint8_t mask)
{
    if (!dev) {
        return ESP_ERR_INVALID_ARG;
    }
    return cst816t_i2c_write(dev, CST816T_REG_MOTION_MASK, mask);
}

esp_err_t cst816t_disable_auto_sleep(cst816t_t *dev, bool disable)
{
    if (!dev) {
        return ESP_ERR_INVALID_ARG;
    }
    return cst816t_i2c_write(dev, CST816T_REG_DIS_AUTO_SLEEP, disable ? 0x01 : 0x00);
}

esp_err_t cst816t_set_auto_sleep_time(cst816t_t *dev, uint8_t seconds)
{
    if (!dev) {
        return ESP_ERR_INVALID_ARG;
    }
    return cst816t_i2c_write(dev, CST816T_REG_AUTO_SLEEP_TIME, seconds);
}

esp_err_t cst816t_read_gesture(cst816t_t *dev, cst816t_gesture_t *gesture)
{
    if (!dev || !gesture) {
        return ESP_ERR_INVALID_ARG;
    }

    uint8_t g = 0;
    esp_err_t err = cst816t_i2c_read(dev, CST816T_REG_GESTURE_ID, &g, 1);
    if (err != ESP_OK) {
        return err;
    }

    *gesture = (cst816t_gesture_t)g;
    return ESP_OK;
}

esp_err_t cst816t_is_touched(cst816t_t *dev, bool *touched)
{
    if (!dev || !touched) {
        return ESP_ERR_INVALID_ARG;
    }

    uint8_t finger_num = 0;
    esp_err_t err = cst816t_i2c_read(dev, CST816T_REG_FINGER_NUM, &finger_num, 1);
    if (err != ESP_OK) {
        return err;
    }

    *touched = (finger_num > 0);
    return ESP_OK;
}

esp_err_t cst816t_read_touch(cst816t_t *dev, cst816t_touch_data_t *data)
{
    if (!dev || !data) {
        return ESP_ERR_INVALID_ARG;
    }
    if (!dev->initialized) {
        return ESP_ERR_INVALID_STATE;
    }

    uint8_t buf[6] = {0};
    esp_err_t err = cst816t_i2c_read(dev, CST816T_REG_GESTURE_ID, buf, sizeof(buf));
    if (err != ESP_OK) {
        return err;
    }

    // buf[0]=gesture, buf[1]=finger_num
    data->gesture = (cst816t_gesture_t)buf[0];
    data->finger_num = buf[1] & 0x0F;

    // x/y high bits contain event & high 4 bits
    uint8_t xh = buf[2];
    uint8_t xl = buf[3];
    uint8_t yh = buf[4];
    uint8_t yl = buf[5];

    data->event = (cst816t_event_t)((xh >> 6) & 0x03);

    uint16_t x = ((uint16_t)(xh & 0x0F) << 8) | xl;
    uint16_t y = ((uint16_t)(yh & 0x0F) << 8) | yl;

    cst816t_apply_transform(dev, &x, &y);

    data->x = x;
    data->y = y;
    /* 单点芯片只有按下和持续接触表示有效触摸，NONE 不能刷新待机计时。 */
    data->pressed = (data->finger_num == 1) &&
                    (data->event == CST816T_EVENT_PRESS_DOWN ||
                     data->event == CST816T_EVENT_CONTACT);
    return ESP_OK;
}

const char *cst816t_gesture_to_string(cst816t_gesture_t gesture)
{
    switch (gesture) {
    case CST816T_GESTURE_NONE:
        return "NONE";
    case CST816T_GESTURE_SLIDE_UP:
        return "SLIDE_UP";
    case CST816T_GESTURE_SLIDE_DOWN:
        return "SLIDE_DOWN";
    case CST816T_GESTURE_SLIDE_LEFT:
        return "SLIDE_LEFT";
    case CST816T_GESTURE_SLIDE_RIGHT:
        return "SLIDE_RIGHT";
    case CST816T_GESTURE_SINGLE_CLICK:
        return "SINGLE_CLICK";
    case CST816T_GESTURE_DOUBLE_CLICK:
        return "DOUBLE_CLICK";
    case CST816T_GESTURE_LONG_PRESS:
        return "LONG_PRESS";
    default:
        return "UNKNOWN";
    }
}
