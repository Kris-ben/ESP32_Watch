/**
 * @file em7028.h
 * @brief EM7028 心率传感器驱动头文件
 */

#ifndef EM7028_H
#define EM7028_H

#include <stdbool.h>
#include <stdint.h>
#include "driver/i2c.h"
#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

#define EM7028_I2C_ADDR     0x24
#define EM7028_PID_VALUE    0x36

typedef struct {
    i2c_port_t i2c_port;
    uint8_t i2c_addr;
    bool initialized;
} em7028_dev_t;

/**
 * @brief 初始化 EM7028，并开启 HRS1 连续采样
 */
esp_err_t em7028_init(em7028_dev_t *dev, i2c_port_t i2c_port);

/**
 * @brief 读取 EM7028 PID 寄存器
 */
esp_err_t em7028_read_pid(em7028_dev_t *dev, uint8_t *pid);

/**
 * @brief 读取 HRS1 原始波形值
 */
esp_err_t em7028_read_hrs1_raw(em7028_dev_t *dev, uint16_t *raw);

#ifdef __cplusplus
}
#endif

#endif // EM7028_H
