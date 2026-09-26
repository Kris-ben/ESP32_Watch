#pragma once

// 这个文件是“板级配置入口”。移植到其他工程/硬件时，通常只需要修改这里。

#include "driver/i2c.h"
#include "driver/spi_common.h"

// -------------------- LCD (ST7789V, SPI) --------------------
#define BOARD_LCD_SPI_HOST        SPI2_HOST

#define BOARD_LCD_PIN_SCLK        13
#define BOARD_LCD_PIN_MOSI        12
#define BOARD_LCD_PIN_CS          8
#define BOARD_LCD_PIN_DC          15
#define BOARD_LCD_PIN_RST         38
#define BOARD_LCD_PIN_BL          19

// 分辨率与偏移（P183B001-V4-CTP 1.83inch LCD Rev2）
// Rev2: 240x284 (ST7789P)
#define BOARD_LCD_WIDTH           240
#define BOARD_LCD_HEIGHT          284
#define BOARD_LCD_X_OFFSET        0
#define BOARD_LCD_Y_OFFSET        0

// SPI 时钟：本板实测 30 MHz 画面正常。
#define BOARD_LCD_SPI_FREQ_HZ     (30 * 1000 * 1000)

// ST7789 模块选项（按 Request_Spark 工程默认）
#define BOARD_LCD_INVERT_COLORS   1
#define BOARD_LCD_COLOR_ORDER_BGR 0

// 背光 PWM（LEDC）
#define BOARD_LCD_BL_USE_LEDC     1

// -------------------- Touch (CST816T, I2C) --------------------
#define BOARD_TOUCH_I2C_PORT      I2C_NUM_0
#define BOARD_TOUCH_PIN_SDA       10
#define BOARD_TOUCH_PIN_SCL       11
#define BOARD_TOUCH_PIN_RST       (-1)   // 与 LCD_RST 共用：触摸驱动不操作
#define BOARD_TOUCH_PIN_INT       39
#define BOARD_TOUCH_I2C_FREQ_HZ   400000

// 触摸坐标变换（按 Request_Spark 现状：驱动层不做转换）
#define BOARD_TOUCH_SWAP_XY       0
#define BOARD_TOUCH_MIRROR_X      0
#define BOARD_TOUCH_MIRROR_Y      0
