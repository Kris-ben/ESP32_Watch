/**
 * @file bm8563.h
 * @brief BM8563 RTC驱动头文件
 * @details BM8563是一个低功耗的实时时钟芯片，支持I2C接口
 * 
 * 特性:
 * - I2C地址: 读0xA3, 写0xA2
 * - 支持年月日时分秒
 * - 支持闹钟功能
 * - 支持定时器功能
 * - 可编程时钟输出 (32.768kHz, 1.024kHz, 32Hz, 1Hz)
 * - 低电压检测
 */

#ifndef __BM8563_H__
#define __BM8563_H__

#include <stdint.h>
#include <stdbool.h>
#include "driver/i2c.h"

#ifdef __cplusplus
extern "C" {
#endif

/* I2C地址定义 */
#define BM8563_I2C_ADDR_WRITE   0xA2    ///< BM8563写地址
#define BM8563_I2C_ADDR_READ    0xA3    ///< BM8563读地址
#define BM8563_I2C_ADDR         0x51    ///< BM8563 7位地址

/* 寄存器地址定义 */
#define BM8563_REG_CTRL_STATUS1     0x00    ///< 控制/状态寄存器1
#define BM8563_REG_CTRL_STATUS2     0x01    ///< 控制/状态寄存器2
#define BM8563_REG_SECONDS          0x02    ///< 秒寄存器
#define BM8563_REG_MINUTES          0x03    ///< 分寄存器
#define BM8563_REG_HOURS            0x04    ///< 时寄存器
#define BM8563_REG_DAYS             0x05    ///< 日寄存器
#define BM8563_REG_WEEKDAYS         0x06    ///< 星期寄存器
#define BM8563_REG_MONTHS           0x07    ///< 月寄存器
#define BM8563_REG_YEARS            0x08    ///< 年寄存器
#define BM8563_REG_MINUTE_ALARM     0x09    ///< 分钟闹钟
#define BM8563_REG_HOUR_ALARM       0x0A    ///< 小时闹钟
#define BM8563_REG_DAY_ALARM        0x0B    ///< 日闹钟
#define BM8563_REG_WEEKDAY_ALARM    0x0C    ///< 星期闹钟
#define BM8563_REG_CLKOUT_CTRL      0x0D    ///< 时钟输出控制
#define BM8563_REG_TIMER_CTRL       0x0E    ///< 定时器控制
#define BM8563_REG_TIMER            0x0F    ///< 定时器值

/* 控制/状态寄存器1位定义 */
#define BM8563_CTRL1_TEST1          0x80    ///< 测试模式位
#define BM8563_CTRL1_STOP           0x20    ///< 停止RTC
#define BM8563_CTRL1_TESTC          0x08    ///< POR覆盖使能

/* 控制/状态寄存器2位定义 */
#define BM8563_CTRL2_TI_TP          0x10    ///< 定时器中断模式
#define BM8563_CTRL2_AF             0x08    ///< 闹钟标志
#define BM8563_CTRL2_TF             0x04    ///< 定时器标志
#define BM8563_CTRL2_AIE            0x02    ///< 闹钟中断使能
#define BM8563_CTRL2_TIE            0x01    ///< 定时器中断使能

/* VL_seconds寄存器位定义 */
#define BM8563_VL_FLAG              0x80    ///< 电压低标志

/* 闹钟使能位定义 */
#define BM8563_ALARM_ENABLE         0x00    ///< 闹钟使能
#define BM8563_ALARM_DISABLE        0x80    ///< 闹钟禁用

/* 时钟输出频率定义 */
#define BM8563_CLKOUT_32768HZ       0x00    ///< 32.768kHz
#define BM8563_CLKOUT_1024HZ        0x01    ///< 1.024kHz
#define BM8563_CLKOUT_32HZ          0x02    ///< 32Hz
#define BM8563_CLKOUT_1HZ           0x03    ///< 1Hz
#define BM8563_CLKOUT_ENABLE        0x80    ///< 时钟输出使能

/* 定时器时钟源定义 */
#define BM8563_TIMER_CLK_4096HZ     0x00    ///< 4.096kHz
#define BM8563_TIMER_CLK_64HZ       0x01    ///< 64Hz
#define BM8563_TIMER_CLK_1HZ        0x02    ///< 1Hz
#define BM8563_TIMER_CLK_1_60HZ     0x03    ///< 1/60Hz
#define BM8563_TIMER_ENABLE         0x80    ///< 定时器使能

/* 星期定义 */
typedef enum {
    BM8563_WEEKDAY_SUNDAY    = 0,
    BM8563_WEEKDAY_MONDAY    = 1,
    BM8563_WEEKDAY_TUESDAY   = 2,
    BM8563_WEEKDAY_WEDNESDAY = 3,
    BM8563_WEEKDAY_THURSDAY  = 4,
    BM8563_WEEKDAY_FRIDAY    = 5,
    BM8563_WEEKDAY_SATURDAY  = 6
} bm8563_weekday_t;

/**
 * @brief 时间结构体
 */
typedef struct {
    uint8_t second;     ///< 秒 (0-59)
    uint8_t minute;     ///< 分 (0-59)
    uint8_t hour;       ///< 时 (0-23)
    uint8_t day;        ///< 日 (1-31)
    uint8_t weekday;    ///< 星期 (0-6, 0=周日)
    uint8_t month;      ///< 月 (1-12)
    uint8_t year;       ///< 年 (0-99, 表示2000-2099)
} bm8563_time_t;

/**
 * @brief 闹钟结构体
 */
typedef struct {
    uint8_t minute;         ///< 分钟 (0-59)
    uint8_t hour;           ///< 小时 (0-23)
    uint8_t day;            ///< 日 (1-31)
    uint8_t weekday;        ///< 星期 (0-6)
    bool minute_enable;     ///< 分钟闹钟使能
    bool hour_enable;       ///< 小时闹钟使能
    bool day_enable;        ///< 日闹钟使能
    bool weekday_enable;    ///< 星期闹钟使能
} bm8563_alarm_t;

/**
 * @brief BM8563设备句柄结构体
 */
typedef struct {
    i2c_port_t i2c_port;    ///< I2C端口号
    uint8_t dev_addr;       ///< 设备地址
} bm8563_handle_t;

/**
 * @brief 初始化BM8563
 * 
 * @param handle BM8563设备句柄指针
 * @param i2c_port I2C端口号
 * @return esp_err_t 
 *         - ESP_OK: 成功
 *         - ESP_FAIL: 失败
 */
esp_err_t bm8563_init(bm8563_handle_t *handle, i2c_port_t i2c_port);

/**
 * @brief 设置时间
 * 
 * @param handle BM8563设备句柄指针
 * @param time 时间结构体指针
 * @return esp_err_t 
 *         - ESP_OK: 成功
 *         - ESP_FAIL: 失败
 */
esp_err_t bm8563_set_time(bm8563_handle_t *handle, const bm8563_time_t *time);

/**
 * @brief 获取时间
 * 
 * @param handle BM8563设备句柄指针
 * @param time 时间结构体指针
 * @return esp_err_t 
 *         - ESP_OK: 成功
 *         - ESP_FAIL: 失败
 */
esp_err_t bm8563_get_time(bm8563_handle_t *handle, bm8563_time_t *time);

#ifdef __cplusplus
}
#endif

#endif /* __BM8563_H__ */
