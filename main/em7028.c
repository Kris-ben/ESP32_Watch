/**
 * @file em7028.c
 * @brief EM7028 心率传感器驱动实现
 */

#include "em7028.h"
#include <string.h>
#include "esp_log.h"
#include "freertos/FreeRTOS.h"

static const char *TAG = "EM7028";

#define EM7028_I2C_TIMEOUT_MS       1000

#define EM7028_REG_PID              0x00
#define EM7028_REG_CONFIG           0x01
#define EM7028_REG_HRS1_DATA0_L     0x28
#define EM7028_HRS1_DATA_BYTES      8

#define EM7028_CONFIG_HRS1_EN       BIT3

static esp_err_t em7028_write_reg(em7028_dev_t *dev, uint8_t reg, uint8_t data)
{
    uint8_t write_buf[2] = {reg, data};

    esp_err_t ret = i2c_master_write_to_device(
        dev->i2c_port,
        dev->i2c_addr,
        write_buf,
        sizeof(write_buf),
        pdMS_TO_TICKS(EM7028_I2C_TIMEOUT_MS)
    );

    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "写寄存器0x%02X失败: %s", reg, esp_err_to_name(ret));
    }

    return ret;
}

static esp_err_t em7028_read_reg(em7028_dev_t *dev, uint8_t reg, uint8_t *data, size_t len)
{
    esp_err_t ret = i2c_master_write_read_device(
        dev->i2c_port,
        dev->i2c_addr,
        &reg,
        1,
        data,
        len,
        pdMS_TO_TICKS(EM7028_I2C_TIMEOUT_MS)
    );

    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "读寄存器0x%02X失败: %s", reg, esp_err_to_name(ret));
    }

    return ret;
}

esp_err_t em7028_read_pid(em7028_dev_t *dev, uint8_t *pid)
{
    if (!dev || !pid) {
        return ESP_ERR_INVALID_ARG;
    }

    return em7028_read_reg(dev, EM7028_REG_PID, pid, 1);
}

esp_err_t em7028_init(em7028_dev_t *dev, i2c_port_t i2c_port)
{
    if (!dev) {
        return ESP_ERR_INVALID_ARG;
    }

    memset(dev, 0, sizeof(*dev));
    dev->i2c_port = i2c_port;
    dev->i2c_addr = EM7028_I2C_ADDR;

    uint8_t pid = 0;
    esp_err_t ret = em7028_read_pid(dev, &pid);
    if (ret != ESP_OK) {
        return ret;
    }

    if (pid != EM7028_PID_VALUE) {
        ESP_LOGE(TAG, "PID不匹配: 读到0x%02X，期望0x%02X", pid, EM7028_PID_VALUE);
        return ESP_ERR_NOT_FOUND;
    }

    uint8_t config = 0;
    ret = em7028_read_reg(dev, EM7028_REG_CONFIG, &config, 1);
    if (ret != ESP_OK) {
        return ret;
    }

    // 只打开 HRS1 连续采样位，保留芯片上电后的其它默认配置。
    config |= EM7028_CONFIG_HRS1_EN;
    ret = em7028_write_reg(dev, EM7028_REG_CONFIG, config);
    if (ret != ESP_OK) {
        return ret;
    }

    dev->initialized = true;
    ESP_LOGI(TAG, "初始化成功: I2C地址=0x%02X, PID=0x%02X", dev->i2c_addr, pid);
    return ESP_OK;
}

esp_err_t em7028_read_hrs1_raw(em7028_dev_t *dev, uint16_t *raw)
{
    if (!dev || !raw) {
        return ESP_ERR_INVALID_ARG;
    }

    if (!dev->initialized) {
        return ESP_ERR_INVALID_STATE;
    }

    uint8_t data[EM7028_HRS1_DATA_BYTES] = {0};
    esp_err_t ret = em7028_read_reg(dev, EM7028_REG_HRS1_DATA0_L, data, sizeof(data));
    if (ret != ESP_OK) {
        return ret;
    }

    uint32_t sum = 0;
    uint8_t count = 0;
    for (uint8_t i = 0; i < EM7028_HRS1_DATA_BYTES; i += 2) {
        uint16_t sample = ((uint16_t)data[i + 1] << 8) | data[i];
        if (sample != 0) {
            sum += sample;
            count++;
        }
    }

    *raw = (count > 0) ? (uint16_t)(sum / count) : 0;
    return ESP_OK;
}

esp_err_t em7028_set_hrs1_enabled(em7028_dev_t *dev, bool enabled)
{
    if (!dev) return ESP_ERR_INVALID_ARG;
    if (!dev->initialized) return ESP_ERR_INVALID_STATE;

    uint8_t config = 0;
    esp_err_t err = em7028_read_reg(dev, EM7028_REG_CONFIG, &config, 1);
    if (err != ESP_OK) return err;
    uint8_t next = enabled ? (config | EM7028_CONFIG_HRS1_EN) :
                             (config & (uint8_t)~EM7028_CONFIG_HRS1_EN);
    if (next == config) return ESP_OK;
    return em7028_write_reg(dev, EM7028_REG_CONFIG, next);
}
