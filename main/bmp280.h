/**
 * @file bmp280.h
 * @brief BMP280 温度和气压传感器驱动头文件
 * 
 * 支持I2C接口通信
 * 适用于ESP32-S3平台
 */

#ifndef BMP280_H
#define BMP280_H

#include <stdint.h>
#include <stdbool.h>
#include "driver/i2c.h"
#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

/* BMP280 I2C地址 */
#define BMP280_I2C_ADDR_PRIM    0x76  // SDO接GND
#define BMP280_I2C_ADDR_SEC     0x77  // SDO接VDDIO

/* BMP280 寄存器地址 */
#define BMP280_REG_TEMP_XLSB    0xFC
#define BMP280_REG_TEMP_LSB     0xFB
#define BMP280_REG_TEMP_MSB     0xFA
#define BMP280_REG_PRESS_XLSB   0xF9
#define BMP280_REG_PRESS_LSB    0xF8
#define BMP280_REG_PRESS_MSB    0xF7
#define BMP280_REG_CONFIG       0xF5
#define BMP280_REG_CTRL_MEAS    0xF4
#define BMP280_REG_STATUS       0xF3
#define BMP280_REG_RESET        0xE0
#define BMP280_REG_ID           0xD0
#define BMP280_REG_CALIB_START  0x88

/* BMP280 芯片ID */
#define BMP280_CHIP_ID          0x58

/* BMP280 复位命令 */
#define BMP280_RESET_CMD        0xB6

/* 过采样设置 */
typedef enum {
    BMP280_OVERSAMPLING_SKIP = 0x00,  // 跳过测量
    BMP280_OVERSAMPLING_1X   = 0x01,  // 过采样x1
    BMP280_OVERSAMPLING_2X   = 0x02,  // 过采样x2
    BMP280_OVERSAMPLING_4X   = 0x03,  // 过采样x4
    BMP280_OVERSAMPLING_8X   = 0x04,  // 过采样x8
    BMP280_OVERSAMPLING_16X  = 0x05   // 过采样x16
} bmp280_oversampling_t;

/* 工作模式 */
typedef enum {
    BMP280_MODE_SLEEP  = 0x00,  // 睡眠模式
    BMP280_MODE_FORCED = 0x01,  // 强制模式（单次测量）
    BMP280_MODE_NORMAL = 0x03   // 正常模式（连续测量）
} bmp280_mode_t;

/* 滤波器系数 */
typedef enum {
    BMP280_FILTER_OFF = 0x00,  // 滤波器关闭
    BMP280_FILTER_2   = 0x01,  // 滤波系数2
    BMP280_FILTER_4   = 0x02,  // 滤波系数4
    BMP280_FILTER_8   = 0x03,  // 滤波系数8
    BMP280_FILTER_16  = 0x04   // 滤波系数16
} bmp280_filter_t;

/* 待机时间（正常模式下） */
typedef enum {
    BMP280_STANDBY_0_5MS  = 0x00,  // 0.5ms
    BMP280_STANDBY_62_5MS = 0x01,  // 62.5ms
    BMP280_STANDBY_125MS  = 0x02,  // 125ms
    BMP280_STANDBY_250MS  = 0x03,  // 250ms
    BMP280_STANDBY_500MS  = 0x04,  // 500ms
    BMP280_STANDBY_1000MS = 0x05,  // 1000ms
    BMP280_STANDBY_2000MS = 0x06,  // 2000ms
    BMP280_STANDBY_4000MS = 0x07   // 4000ms
} bmp280_standby_t;

/* 校准参数结构体 */
typedef struct {
    uint16_t dig_T1;
    int16_t  dig_T2;
    int16_t  dig_T3;
    uint16_t dig_P1;
    int16_t  dig_P2;
    int16_t  dig_P3;
    int16_t  dig_P4;
    int16_t  dig_P5;
    int16_t  dig_P6;
    int16_t  dig_P7;
    int16_t  dig_P8;
    int16_t  dig_P9;
} bmp280_calib_param_t;

/* BMP280配置结构体 */
typedef struct {
    bmp280_oversampling_t osr_pressure;     // 气压过采样
    bmp280_oversampling_t osr_temperature;  // 温度过采样
    bmp280_filter_t       filter;           // 滤波器系数
    bmp280_standby_t      standby_time;     // 待机时间
    bmp280_mode_t         mode;             // 工作模式
} bmp280_config_t;

/* BMP280设备结构体 */
typedef struct {
    i2c_port_t            i2c_port;      // I2C端口号
    uint8_t               i2c_addr;      // I2C设备地址
    bmp280_config_t       config;        // 配置参数
    bmp280_calib_param_t  calib;         // 校准参数
    int32_t               t_fine;        // 温度补偿值（内部使用）
} bmp280_dev_t;

/**
 * @brief 初始化BMP280设备
 * 
 * @param dev BMP280设备结构体指针
 * @param i2c_port I2C端口号
 * @param i2c_addr I2C设备地址
 * @return esp_err_t ESP_OK表示成功
 */
esp_err_t bmp280_init(bmp280_dev_t *dev, i2c_port_t i2c_port, uint8_t i2c_addr);

/**
 * @brief 配置BMP280参数
 * 
 * @param dev BMP280设备结构体指针
 * @param config 配置参数指针
 * @return esp_err_t ESP_OK表示成功
 */
esp_err_t bmp280_configure(bmp280_dev_t *dev, const bmp280_config_t *config);

/**
 * @brief 获取默认配置
 * 
 * @param config 配置参数指针
 */
void bmp280_get_default_config(bmp280_config_t *config);

/**
 * @brief 软复位BMP280
 * 
 * @param dev BMP280设备结构体指针
 * @return esp_err_t ESP_OK表示成功
 */
esp_err_t bmp280_reset(bmp280_dev_t *dev);

/**
 * @brief 设置工作模式
 * 
 * @param dev BMP280设备结构体指针
 * @param mode 工作模式
 * @return esp_err_t ESP_OK表示成功
 */
esp_err_t bmp280_set_mode(bmp280_dev_t *dev, bmp280_mode_t mode);

/**
 * @brief 读取温度（单位：摄氏度）
 * 
 * @param dev BMP280设备结构体指针
 * @param temperature 温度值指针
 * @return esp_err_t ESP_OK表示成功
 */
esp_err_t bmp280_read_temperature(bmp280_dev_t *dev, float *temperature);

/**
 * @brief 读取气压（单位：帕斯卡Pa）
 * 
 * @param dev BMP280设备结构体指针
 * @param pressure 气压值指针
 * @return esp_err_t ESP_OK表示成功
 */
esp_err_t bmp280_read_pressure(bmp280_dev_t *dev, float *pressure);

/**
 * @brief 同时读取温度和气压
 * 
 * @param dev BMP280设备结构体指针
 * @param temperature 温度值指针（单位：摄氏度）
 * @param pressure 气压值指针（单位：帕斯卡Pa）
 * @return esp_err_t ESP_OK表示成功
 */
esp_err_t bmp280_read_data(bmp280_dev_t *dev, float *temperature, float *pressure);

/**
 * @brief 检查设备是否正在测量
 * 
 * @param dev BMP280设备结构体指针
 * @param measuring 测量状态指针
 * @return esp_err_t ESP_OK表示成功
 */
esp_err_t bmp280_is_measuring(bmp280_dev_t *dev, bool *measuring);

#ifdef __cplusplus
}
#endif

#endif // BMP280_H
