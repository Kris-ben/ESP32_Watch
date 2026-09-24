/**
 * @file bmp280.c
 * @brief BMP280 温度和气压传感器驱动实现
 */

#include "bmp280.h"
#include <string.h>
#include <math.h>
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static const char *TAG = "BMP280";

/* I2C超时时间 */
#define BMP280_I2C_TIMEOUT_MS   1000

/**
 * @brief I2C写寄存器
 */
static esp_err_t bmp280_write_reg(bmp280_dev_t *dev, uint8_t reg, uint8_t data)
{
    uint8_t write_buf[2] = {reg, data};
    
    ESP_LOGD(TAG, "写寄存器: I2C地址=0x%02X, 寄存器=0x%02X, 数据=0x%02X",
             dev->i2c_addr, reg, data);
    
    esp_err_t ret = i2c_master_write_to_device(
        dev->i2c_port,
        dev->i2c_addr,
        write_buf,
        sizeof(write_buf),
        pdMS_TO_TICKS(BMP280_I2C_TIMEOUT_MS)
    );
    
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "写寄存器0x%02X失败: %s (I2C地址=0x%02X)",
                 reg, esp_err_to_name(ret), dev->i2c_addr);
    } else {
        ESP_LOGD(TAG, "写寄存器0x%02X成功", reg);
    }
    
    return ret;
}

/**
 * @brief I2C读寄存器
 */
static esp_err_t bmp280_read_reg(bmp280_dev_t *dev, uint8_t reg, uint8_t *data, size_t len)
{
    ESP_LOGD(TAG, "读寄存器: I2C地址=0x%02X, 寄存器=0x%02X, 长度=%d",
             dev->i2c_addr, reg, len);
    
    esp_err_t ret = i2c_master_write_read_device(
        dev->i2c_port,
        dev->i2c_addr,
        &reg,
        1,
        data,
        len,
        pdMS_TO_TICKS(BMP280_I2C_TIMEOUT_MS)
    );
    
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "读寄存器0x%02X失败: %s (I2C地址=0x%02X)",
                 reg, esp_err_to_name(ret), dev->i2c_addr);
    } else {
        ESP_LOGD(TAG, "读寄存器0x%02X成功, 数据[0]=0x%02X", reg, data[0]);
    }
    
    return ret;
}

/**
 * @brief 读取校准参数
 */
static esp_err_t bmp280_read_calibration(bmp280_dev_t *dev)
{
    uint8_t calib_data[24];
    esp_err_t ret;
    
    // 读取校准数据（0x88-0x9F）
    ret = bmp280_read_reg(dev, BMP280_REG_CALIB_START, calib_data, sizeof(calib_data));
    if (ret != ESP_OK) {
        return ret;
    }
    
    // 解析校准参数
    dev->calib.dig_T1 = (uint16_t)(calib_data[1] << 8) | calib_data[0];
    dev->calib.dig_T2 = (int16_t)(calib_data[3] << 8) | calib_data[2];
    dev->calib.dig_T3 = (int16_t)(calib_data[5] << 8) | calib_data[4];
    
    dev->calib.dig_P1 = (uint16_t)(calib_data[7] << 8) | calib_data[6];
    dev->calib.dig_P2 = (int16_t)(calib_data[9] << 8) | calib_data[8];
    dev->calib.dig_P3 = (int16_t)(calib_data[11] << 8) | calib_data[10];
    dev->calib.dig_P4 = (int16_t)(calib_data[13] << 8) | calib_data[12];
    dev->calib.dig_P5 = (int16_t)(calib_data[15] << 8) | calib_data[14];
    dev->calib.dig_P6 = (int16_t)(calib_data[17] << 8) | calib_data[16];
    dev->calib.dig_P7 = (int16_t)(calib_data[19] << 8) | calib_data[18];
    dev->calib.dig_P8 = (int16_t)(calib_data[21] << 8) | calib_data[20];
    dev->calib.dig_P9 = (int16_t)(calib_data[23] << 8) | calib_data[22];
    
    ESP_LOGI(TAG, "校准参数读取成功");
    ESP_LOGI(TAG, "温度校准: T1=%u, T2=%d, T3=%d", 
             dev->calib.dig_T1, dev->calib.dig_T2, dev->calib.dig_T3);
    ESP_LOGI(TAG, "气压校准: P1=%u, P2=%d, P3=%d, P4=%d, P5=%d", 
             dev->calib.dig_P1, dev->calib.dig_P2, dev->calib.dig_P3, 
             dev->calib.dig_P4, dev->calib.dig_P5);
    ESP_LOGI(TAG, "气压校准: P6=%d, P7=%d, P8=%d, P9=%d", 
             dev->calib.dig_P6, dev->calib.dig_P7, dev->calib.dig_P8, dev->calib.dig_P9);
    
    return ESP_OK;
}

/**
 * @brief 温度补偿计算（根据数据手册）
 */
static int32_t bmp280_compensate_temperature(bmp280_dev_t *dev, int32_t adc_T)
{
    int32_t var1, var2, T;
    
    var1 = ((((adc_T >> 3) - ((int32_t)dev->calib.dig_T1 << 1))) * 
            ((int32_t)dev->calib.dig_T2)) >> 11;
    
    var2 = (((((adc_T >> 4) - ((int32_t)dev->calib.dig_T1)) * 
              ((adc_T >> 4) - ((int32_t)dev->calib.dig_T1))) >> 12) * 
            ((int32_t)dev->calib.dig_T3)) >> 14;
    
    dev->t_fine = var1 + var2;
    T = (dev->t_fine * 5 + 128) >> 8;
    
    return T;
}

/**
 * @brief 气压补偿计算（根据数据手册）
 */
static uint32_t bmp280_compensate_pressure(bmp280_dev_t *dev, int32_t adc_P)
{
    int64_t var1, var2, p;
    
    // 检查t_fine是否已经计算（必须先读取温度）
    if (dev->t_fine == 0) {
        ESP_LOGW(TAG, "警告: t_fine未初始化，气压计算可能不准确");
    }
    
    var1 = ((int64_t)dev->t_fine) - 128000;
    var2 = var1 * var1 * (int64_t)dev->calib.dig_P6;
    var2 = var2 + ((var1 * (int64_t)dev->calib.dig_P5) << 17);
    var2 = var2 + (((int64_t)dev->calib.dig_P4) << 35);
    var1 = ((var1 * var1 * (int64_t)dev->calib.dig_P3) >> 8) + 
           ((var1 * (int64_t)dev->calib.dig_P2) << 12);
    var1 = (((((int64_t)1) << 47) + var1)) * ((int64_t)dev->calib.dig_P1) >> 33;
    
    if (var1 == 0) {
        ESP_LOGE(TAG, "气压补偿计算错误: var1=0 (除零)");
        return 0; // 避免除零异常
    }
    
    p = 1048576 - adc_P;
    p = (((p << 31) - var2) * 3125) / var1;
    var1 = (((int64_t)dev->calib.dig_P9) * (p >> 13) * (p >> 13)) >> 25;
    var2 = (((int64_t)dev->calib.dig_P8) * p) >> 19;
    p = ((p + var1 + var2) >> 8) + (((int64_t)dev->calib.dig_P7) << 4);
    
    return (uint32_t)p;
}

esp_err_t bmp280_init(bmp280_dev_t *dev, i2c_port_t i2c_port, uint8_t i2c_addr)
{
    if (dev == NULL) {
        return ESP_ERR_INVALID_ARG;
    }
    
    ESP_LOGI(TAG, "初始化BMP280: I2C端口=%d, I2C地址=0x%02X", i2c_port, i2c_addr);
    
    dev->i2c_port = i2c_port;
    dev->i2c_addr = i2c_addr;
    dev->t_fine = 0;
    
    // 等待I2C总线稳定
    vTaskDelay(pdMS_TO_TICKS(50));
    
    // 读取芯片ID
    ESP_LOGI(TAG, "读取芯片ID寄存器(0x%02X)...", BMP280_REG_ID);
    uint8_t chip_id;
    esp_err_t ret = bmp280_read_reg(dev, BMP280_REG_ID, &chip_id, 1);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "读取芯片ID失败: %s", esp_err_to_name(ret));
        ESP_LOGE(TAG, "可能的原因:");
        ESP_LOGE(TAG, "  1. I2C地址错误 (当前: 0x%02X)", i2c_addr);
        ESP_LOGE(TAG, "  2. 硬件连接问题");
        ESP_LOGE(TAG, "  3. 传感器未供电");
        ESP_LOGE(TAG, "  4. 上拉电阻问题");
        return ret;
    }
    
    ESP_LOGI(TAG, "读取到芯片ID: 0x%02X", chip_id);
    
    if (chip_id != BMP280_CHIP_ID) {
        ESP_LOGE(TAG, "芯片ID不匹配: 0x%02X (期望: 0x%02X)", chip_id, BMP280_CHIP_ID);
        if (chip_id == 0x60) {
            ESP_LOGW(TAG, "检测到BME280芯片 (ID=0x60)，该驱动仅支持BMP280");
        } else if (chip_id == 0xFF || chip_id == 0x00) {
            ESP_LOGE(TAG, "读取到无效ID，可能是硬件连接问题");
        }
        return ESP_ERR_NOT_FOUND;
    }
    
    ESP_LOGI(TAG, "BMP280芯片ID验证成功: 0x%02X", chip_id);
    
    // 软复位
    ret = bmp280_reset(dev);
    if (ret != ESP_OK) {
        return ret;
    }
    
    // 等待复位完成
    vTaskDelay(pdMS_TO_TICKS(10));
    
    // 读取校准参数
    ret = bmp280_read_calibration(dev);
    if (ret != ESP_OK) {
        return ret;
    }
    
    // 设置默认配置
    bmp280_config_t default_config;
    bmp280_get_default_config(&default_config);
    ret = bmp280_configure(dev, &default_config);
    
    if (ret == ESP_OK) {
        ESP_LOGI(TAG, "BMP280初始化成功");
    }
    
    return ret;
}

void bmp280_get_default_config(bmp280_config_t *config)
{
    if (config == NULL) {
        return;
    }
    
    config->osr_pressure = BMP280_OVERSAMPLING_16X;
    config->osr_temperature = BMP280_OVERSAMPLING_2X;
    config->filter = BMP280_FILTER_16;
    config->standby_time = BMP280_STANDBY_500MS;
    config->mode = BMP280_MODE_NORMAL;
}

esp_err_t bmp280_configure(bmp280_dev_t *dev, const bmp280_config_t *config)
{
    if (dev == NULL || config == NULL) {
        return ESP_ERR_INVALID_ARG;
    }
    
    esp_err_t ret;
    
    // 设置为睡眠模式以便配置
    ret = bmp280_set_mode(dev, BMP280_MODE_SLEEP);
    if (ret != ESP_OK) {
        return ret;
    }
    
    // 配置config寄存器（待机时间和滤波器）
    uint8_t config_reg = (config->standby_time << 5) | (config->filter << 2);
    ret = bmp280_write_reg(dev, BMP280_REG_CONFIG, config_reg);
    if (ret != ESP_OK) {
        return ret;
    }
    
    // 配置ctrl_meas寄存器（过采样和模式）
    uint8_t ctrl_meas = (config->osr_temperature << 5) | 
                        (config->osr_pressure << 2) | 
                        config->mode;
    ret = bmp280_write_reg(dev, BMP280_REG_CTRL_MEAS, ctrl_meas);
    if (ret != ESP_OK) {
        return ret;
    }
    
    // 保存配置
    memcpy(&dev->config, config, sizeof(bmp280_config_t));
    
    ESP_LOGI(TAG, "BMP280配置完成");
    
    return ESP_OK;
}

esp_err_t bmp280_reset(bmp280_dev_t *dev)
{
    if (dev == NULL) {
        return ESP_ERR_INVALID_ARG;
    }
    
    esp_err_t ret = bmp280_write_reg(dev, BMP280_REG_RESET, BMP280_RESET_CMD);
    
    if (ret == ESP_OK) {
        ESP_LOGI(TAG, "BMP280复位成功");
    }
    
    return ret;
}

esp_err_t bmp280_set_mode(bmp280_dev_t *dev, bmp280_mode_t mode)
{
    if (dev == NULL) {
        return ESP_ERR_INVALID_ARG;
    }
    
    // 读取当前ctrl_meas寄存器值
    uint8_t ctrl_meas;
    esp_err_t ret = bmp280_read_reg(dev, BMP280_REG_CTRL_MEAS, &ctrl_meas, 1);
    if (ret != ESP_OK) {
        return ret;
    }
    
    // 修改模式位
    ctrl_meas = (ctrl_meas & 0xFC) | mode;
    ret = bmp280_write_reg(dev, BMP280_REG_CTRL_MEAS, ctrl_meas);
    
    if (ret == ESP_OK) {
        dev->config.mode = mode;
    }
    
    return ret;
}

esp_err_t bmp280_is_measuring(bmp280_dev_t *dev, bool *measuring)
{
    if (dev == NULL || measuring == NULL) {
        return ESP_ERR_INVALID_ARG;
    }
    
    uint8_t status;
    esp_err_t ret = bmp280_read_reg(dev, BMP280_REG_STATUS, &status, 1);
    if (ret != ESP_OK) {
        return ret;
    }
    
    *measuring = (status & 0x08) != 0;
    
    return ESP_OK;
}

esp_err_t bmp280_read_temperature(bmp280_dev_t *dev, float *temperature)
{
    if (dev == NULL || temperature == NULL) {
        return ESP_ERR_INVALID_ARG;
    }
    
    // 如果是强制模式，触发一次测量
    if (dev->config.mode == BMP280_MODE_FORCED) {
        esp_err_t ret = bmp280_set_mode(dev, BMP280_MODE_FORCED);
        if (ret != ESP_OK) {
            return ret;
        }
        
        // 等待测量完成
        bool measuring = true;
        int timeout = 100;
        while (measuring && timeout > 0) {
            vTaskDelay(pdMS_TO_TICKS(10));
            bmp280_is_measuring(dev, &measuring);
            timeout--;
        }
        
        if (timeout == 0) {
            ESP_LOGE(TAG, "测量超时");
            return ESP_ERR_TIMEOUT;
        }
    }
    
    // 读取温度原始数据
    uint8_t data[3];
    esp_err_t ret = bmp280_read_reg(dev, BMP280_REG_TEMP_MSB, data, 3);
    if (ret != ESP_OK) {
        return ret;
    }
    
    int32_t adc_T = ((int32_t)data[0] << 12) | ((int32_t)data[1] << 4) | ((int32_t)data[2] >> 4);
    
    // 温度补偿
    int32_t T = bmp280_compensate_temperature(dev, adc_T);
    *temperature = T / 100.0f;
    
    return ESP_OK;
}

esp_err_t bmp280_read_pressure(bmp280_dev_t *dev, float *pressure)
{
    if (dev == NULL || pressure == NULL) {
        return ESP_ERR_INVALID_ARG;
    }
    
    // 先读取温度以更新t_fine
    float temp;
    esp_err_t ret = bmp280_read_temperature(dev, &temp);
    if (ret != ESP_OK) {
        return ret;
    }
    
    // 读取气压原始数据
    uint8_t data[3];
    ret = bmp280_read_reg(dev, BMP280_REG_PRESS_MSB, data, 3);
    if (ret != ESP_OK) {
        return ret;
    }
    
    int32_t adc_P = ((int32_t)data[0] << 12) | ((int32_t)data[1] << 4) | ((int32_t)data[2] >> 4);
    
    // 气压补偿
    uint32_t P = bmp280_compensate_pressure(dev, adc_P);
    *pressure = P / 256.0f;
    
    return ESP_OK;
}

esp_err_t bmp280_read_data(bmp280_dev_t *dev, float *temperature, float *pressure)
{
    if (dev == NULL || temperature == NULL || pressure == NULL) {
        return ESP_ERR_INVALID_ARG;
    }
    
    // 如果是强制模式，触发一次测量
    if (dev->config.mode == BMP280_MODE_FORCED) {
        esp_err_t ret = bmp280_set_mode(dev, BMP280_MODE_FORCED);
        if (ret != ESP_OK) {
            return ret;
        }
        
        // 等待测量完成
        bool measuring = true;
        int timeout = 100;
        while (measuring && timeout > 0) {
            vTaskDelay(pdMS_TO_TICKS(10));
            bmp280_is_measuring(dev, &measuring);
            timeout--;
        }
        
        if (timeout == 0) {
            ESP_LOGE(TAG, "测量超时");
            return ESP_ERR_TIMEOUT;
        }
    } else {
        // 正常模式下，等待一小段时间确保数据更新
        vTaskDelay(pdMS_TO_TICKS(10));
    }
    
    // 读取温度和气压原始数据（连续读取）
    uint8_t data[6];
    esp_err_t ret = bmp280_read_reg(dev, BMP280_REG_PRESS_MSB, data, 6);
    if (ret != ESP_OK) {
        return ret;
    }
    
    // 解析气压数据
    int32_t adc_P = ((int32_t)data[0] << 12) | ((int32_t)data[1] << 4) | ((int32_t)data[2] >> 4);
    
    // 解析温度数据
    int32_t adc_T = ((int32_t)data[3] << 12) | ((int32_t)data[4] << 4) | ((int32_t)data[5] >> 4);
    
    // 添加调试信息（仅在需要时启用）
    ESP_LOGD(TAG, "原始数据: [%02X %02X %02X] [%02X %02X %02X]", 
             data[0], data[1], data[2], data[3], data[4], data[5]);
    ESP_LOGD(TAG, "ADC值: adc_T=%d (0x%X), adc_P=%d (0x%X)", 
             adc_T, adc_T, adc_P, adc_P);
    
    // 检查ADC值是否有效（0x80000表示数据无效或未就绪）
    if (adc_T == 0x80000 || adc_P == 0x80000) {
        ESP_LOGW(TAG, "读取到无效的ADC值 (0x80000) - 传感器数据未就绪");
        return ESP_ERR_INVALID_RESPONSE;
    }
    
    // 检查ADC值是否为0（可能是读取错误）
    if (adc_T == 0 || adc_P == 0) {
        ESP_LOGW(TAG, "读取到零ADC值 - 可能是读取错误");
        return ESP_ERR_INVALID_RESPONSE;
    }
    
    // 温度补偿（必须先计算，因为气压补偿需要t_fine）
    int32_t T = bmp280_compensate_temperature(dev, adc_T);
    *temperature = T / 100.0f;
    
    ESP_LOGD(TAG, "t_fine=%d, 温度=%.2f°C", dev->t_fine, *temperature);
    
    // 气压补偿（依赖于t_fine）
    uint32_t P = bmp280_compensate_pressure(dev, adc_P);
    *pressure = P / 256.0f;
    
    ESP_LOGD(TAG, "气压原始值=%u, 气压=%.2f Pa (%.2f hPa)", P, *pressure, *pressure / 100.0f);
    
    return ESP_OK;
}

