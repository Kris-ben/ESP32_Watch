/**
 * @file bm8563.c
 * @brief BM8563 RTC驱动实现文件
 */

#include "bm8563.h"
#include "esp_log.h"
#include <string.h>

static const char *TAG = "BM8563";

/**
 * @brief I2C写寄存器
 */
static esp_err_t bm8563_write_reg(bm8563_handle_t *handle, uint8_t reg_addr, uint8_t data)
{
    uint8_t write_buf[2] = {reg_addr, data};
    
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (handle->dev_addr << 1) | I2C_MASTER_WRITE, true);
    i2c_master_write(cmd, write_buf, 2, true);
    i2c_master_stop(cmd);
    
    esp_err_t ret = i2c_master_cmd_begin(handle->i2c_port, cmd, pdMS_TO_TICKS(1000));
    i2c_cmd_link_delete(cmd);
    
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "I2C write failed: %s", esp_err_to_name(ret));
    }
    
    return ret;
}

/**
 * @brief I2C读寄存器
 */
static esp_err_t bm8563_read_reg(bm8563_handle_t *handle, uint8_t reg_addr, uint8_t *data)
{
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (handle->dev_addr << 1) | I2C_MASTER_WRITE, true);
    i2c_master_write_byte(cmd, reg_addr, true);
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (handle->dev_addr << 1) | I2C_MASTER_READ, true);
    i2c_master_read_byte(cmd, data, I2C_MASTER_NACK);
    i2c_master_stop(cmd);
    
    esp_err_t ret = i2c_master_cmd_begin(handle->i2c_port, cmd, pdMS_TO_TICKS(1000));
    i2c_cmd_link_delete(cmd);
    
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "I2C read failed: %s", esp_err_to_name(ret));
    }
    
    return ret;
}

/**
 * @brief I2C连续读寄存器
 */
static esp_err_t bm8563_read_regs(bm8563_handle_t *handle, uint8_t reg_addr, uint8_t *data, size_t len)
{
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (handle->dev_addr << 1) | I2C_MASTER_WRITE, true);
    i2c_master_write_byte(cmd, reg_addr, true);
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (handle->dev_addr << 1) | I2C_MASTER_READ, true);
    
    if (len > 1) {
        i2c_master_read(cmd, data, len - 1, I2C_MASTER_ACK);
    }
    i2c_master_read_byte(cmd, data + len - 1, I2C_MASTER_NACK);
    i2c_master_stop(cmd);
    
    esp_err_t ret = i2c_master_cmd_begin(handle->i2c_port, cmd, pdMS_TO_TICKS(1000));
    i2c_cmd_link_delete(cmd);
    
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "I2C read multiple failed: %s", esp_err_to_name(ret));
    }
    
    return ret;
}

/**
 * @brief I2C连续写寄存器
 */
static esp_err_t bm8563_write_regs(bm8563_handle_t *handle, uint8_t reg_addr, const uint8_t *data, size_t len)
{
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (handle->dev_addr << 1) | I2C_MASTER_WRITE, true);
    i2c_master_write_byte(cmd, reg_addr, true);
    i2c_master_write(cmd, (uint8_t *)data, len, true);
    i2c_master_stop(cmd);
    
    esp_err_t ret = i2c_master_cmd_begin(handle->i2c_port, cmd, pdMS_TO_TICKS(1000));
    i2c_cmd_link_delete(cmd);
    
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "I2C write multiple failed: %s", esp_err_to_name(ret));
    }
    
    return ret;
}

static uint8_t bm8563_bcd_to_dec(uint8_t bcd)
{
    return ((bcd >> 4) * 10) + (bcd & 0x0F);
}

static uint8_t bm8563_dec_to_bcd(uint8_t dec)
{
    return ((dec / 10) << 4) | (dec % 10);
}

esp_err_t bm8563_init(bm8563_handle_t *handle, i2c_port_t i2c_port)
{
    if (handle == NULL) {
        ESP_LOGE(TAG, "Handle is NULL");
        return ESP_ERR_INVALID_ARG;
    }
    
    handle->i2c_port = i2c_port;
    handle->dev_addr = BM8563_I2C_ADDR;
    
    // 读取控制寄存器1，检查设备是否存在
    uint8_t ctrl1;
    esp_err_t ret = bm8563_read_reg(handle, BM8563_REG_CTRL_STATUS1, &ctrl1);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "BM8563 not found");
        return ret;
    }
    
    // 清除TEST1和TESTC位，确保正常模式
    ctrl1 &= ~(BM8563_CTRL1_TEST1 | BM8563_CTRL1_TESTC);
    ret = bm8563_write_reg(handle, BM8563_REG_CTRL_STATUS1, ctrl1);
    if (ret != ESP_OK) {
        return ret;
    }
    
    // 清除控制寄存器2的所有标志
    ret = bm8563_write_reg(handle, BM8563_REG_CTRL_STATUS2, 0x00);
    if (ret != ESP_OK) {
        return ret;
    }
    
    ESP_LOGI(TAG, "BM8563 initialized successfully");
    return ESP_OK;
}

esp_err_t bm8563_set_time(bm8563_handle_t *handle, const bm8563_time_t *time)
{
    if (handle == NULL || time == NULL) {
        return ESP_ERR_INVALID_ARG;
    }
    
    // 验证时间参数
    if (time->second > 59 || time->minute > 59 || time->hour > 23 ||
        time->day < 1 || time->day > 31 || time->month < 1 || time->month > 12 ||
        time->weekday > 6 || time->year > 99) {
        ESP_LOGE(TAG, "Invalid time parameters");
        return ESP_ERR_INVALID_ARG;
    }
    
    uint8_t time_buf[7];
    
    // 转换为BCD格式并清除VL标志
    time_buf[0] = bm8563_dec_to_bcd(time->second) & 0x7F;  // 清除VL位
    time_buf[1] = bm8563_dec_to_bcd(time->minute) & 0x7F;
    time_buf[2] = bm8563_dec_to_bcd(time->hour) & 0x3F;
    time_buf[3] = bm8563_dec_to_bcd(time->day) & 0x3F;
    time_buf[4] = time->weekday & 0x07;
    time_buf[5] = bm8563_dec_to_bcd(time->month) & 0x1F;  // 保留世纪位为0
    time_buf[6] = bm8563_dec_to_bcd(time->year);
    
    // 一次性写入所有时间寄存器
    esp_err_t ret = bm8563_write_regs(handle, BM8563_REG_SECONDS, time_buf, 7);
    
    if (ret == ESP_OK) {
        ESP_LOGI(TAG, "Time set: 20%02d-%02d-%02d %02d:%02d:%02d Week:%d",
                 time->year, time->month, time->day,
                 time->hour, time->minute, time->second, time->weekday);
    }
    
    return ret;
}

esp_err_t bm8563_get_time(bm8563_handle_t *handle, bm8563_time_t *time)
{
    if (handle == NULL || time == NULL) {
        return ESP_ERR_INVALID_ARG;
    }
    
    uint8_t time_buf[7];
    
    // 一次性读取所有时间寄存器
    esp_err_t ret = bm8563_read_regs(handle, BM8563_REG_SECONDS, time_buf, 7);
    if (ret != ESP_OK) {
        return ret;
    }
    
    // 转换为十进制格式
    time->second = bm8563_bcd_to_dec(time_buf[0] & 0x7F);
    time->minute = bm8563_bcd_to_dec(time_buf[1] & 0x7F);
    time->hour = bm8563_bcd_to_dec(time_buf[2] & 0x3F);
    time->day = bm8563_bcd_to_dec(time_buf[3] & 0x3F);
    time->weekday = time_buf[4] & 0x07;
    time->month = bm8563_bcd_to_dec(time_buf[5] & 0x1F);
    time->year = bm8563_bcd_to_dec(time_buf[6]);
    
    return ESP_OK;
}

