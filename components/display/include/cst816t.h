/**
 * @file cst816t.h
 * @brief CST816T Capacitive Touch Controller Driver
 *
 * CST816T是一款高性能低功耗电容触摸芯片，支持手势识别
 * I2C接口，从地址0x15
 */

#pragma once

#include <stdbool.h>
#include <stdint.h>

#include "esp_err.h"
#include "driver/i2c.h"

#ifdef __cplusplus
extern "C" {
#endif

#define CST816T_I2C_ADDR            0x15

#define CST816T_REG_GESTURE_ID      0x01
#define CST816T_REG_FINGER_NUM      0x02
#define CST816T_REG_XPOS_H          0x03
#define CST816T_REG_XPOS_L          0x04
#define CST816T_REG_YPOS_H          0x05
#define CST816T_REG_YPOS_L          0x06
#define CST816T_REG_BPC0_H          0xB0
#define CST816T_REG_BPC0_L          0xB1
#define CST816T_REG_BPC1_H          0xB2
#define CST816T_REG_BPC1_L          0xB3
#define CST816T_REG_CHIP_ID         0xA7
#define CST816T_REG_PROJ_ID         0xA8
#define CST816T_REG_FW_VERSION      0xA9
#define CST816T_REG_MOTION_MASK     0xEC
#define CST816T_REG_IRQ_PULSE_WIDTH 0xED
#define CST816T_REG_NOR_SCAN_PER    0xEE
#define CST816T_REG_MOTION_SL_ANGLE 0xEF
#define CST816T_REG_LP_SCAN_RAW1_H  0xF0
#define CST816T_REG_LP_SCAN_RAW1_L  0xF1
#define CST816T_REG_LP_SCAN_RAW2_H  0xF2
#define CST816T_REG_LP_SCAN_RAW2_L  0xF3
#define CST816T_REG_LP_AUTO_WAKEUP  0xF4
#define CST816T_REG_LP_SCAN_TH      0xF5
#define CST816T_REG_LP_SCAN_WIN     0xF6
#define CST816T_REG_LP_SCAN_FREQ    0xF7
#define CST816T_REG_LP_SCAN_I_DAC   0xF8
#define CST816T_REG_AUTO_SLEEP_TIME 0xF9
#define CST816T_REG_IRQ_CTL         0xFA
#define CST816T_REG_AUTO_RESET      0xFB
#define CST816T_REG_LONG_PRESS_TIME 0xFC
#define CST816T_REG_IO_CTL          0xFD
#define CST816T_REG_DIS_AUTO_SLEEP  0xFE

#define CST816T_CHIP_ID             0xB5

typedef enum {
    CST816T_GESTURE_NONE        = 0x00,
    CST816T_GESTURE_SLIDE_UP    = 0x01,
    CST816T_GESTURE_SLIDE_DOWN  = 0x02,
    CST816T_GESTURE_SLIDE_LEFT  = 0x03,
    CST816T_GESTURE_SLIDE_RIGHT = 0x04,
    CST816T_GESTURE_SINGLE_CLICK = 0x05,
    CST816T_GESTURE_DOUBLE_CLICK = 0x0B,
    CST816T_GESTURE_LONG_PRESS  = 0x0C,
} cst816t_gesture_t;

typedef enum {
    CST816T_EVENT_PRESS_DOWN = 0x00,
    CST816T_EVENT_LIFT_UP    = 0x01,
    CST816T_EVENT_CONTACT    = 0x02,
    CST816T_EVENT_NONE       = 0x03,
} cst816t_event_t;

typedef enum {
    CST816T_IRQ_EN_TOUCH    = 0x40,
    CST816T_IRQ_EN_CHANGE   = 0x20,
    CST816T_IRQ_EN_MOTION   = 0x10,
    CST816T_IRQ_ONCE_WLP    = 0x80,
} cst816t_irq_mode_t;

typedef enum {
    CST816T_MOTION_EN_CON_UD     = 0x04,
    CST816T_MOTION_EN_CON_LR     = 0x02,
    CST816T_MOTION_EN_DCLICK     = 0x01,
} cst816t_motion_mask_t;

typedef struct {
    uint16_t x;
    uint16_t y;
    uint8_t finger_num;
    cst816t_gesture_t gesture;
    cst816t_event_t event;
    bool pressed;
} cst816t_touch_data_t;

typedef struct {
    i2c_port_t i2c_port;
    int pin_sda;
    int pin_scl;
    int pin_rst;
    int pin_int;
    uint32_t i2c_freq_hz;
    uint16_t screen_width;
    uint16_t screen_height;
    bool swap_xy;
    bool mirror_x;
    bool mirror_y;
} cst816t_config_t;

typedef struct {
    cst816t_config_t cfg;
    uint8_t chip_id;
    uint8_t project_id;
    uint8_t fw_version;
    bool initialized;
} cst816t_t;

esp_err_t cst816t_init(cst816t_t *dev, const cst816t_config_t *cfg);

esp_err_t cst816t_deinit(cst816t_t *dev);

esp_err_t cst816t_reset(cst816t_t *dev);

esp_err_t cst816t_read_touch(cst816t_t *dev, cst816t_touch_data_t *data);

esp_err_t cst816t_is_touched(cst816t_t *dev, bool *touched);

esp_err_t cst816t_read_gesture(cst816t_t *dev, cst816t_gesture_t *gesture);

esp_err_t cst816t_get_info(cst816t_t *dev, uint8_t *chip_id, uint8_t *project_id, uint8_t *fw_version);

esp_err_t cst816t_set_irq_mode(cst816t_t *dev, uint8_t mode);

esp_err_t cst816t_set_motion_mask(cst816t_t *dev, uint8_t mask);

esp_err_t cst816t_disable_auto_sleep(cst816t_t *dev, bool disable);

esp_err_t cst816t_set_auto_sleep_time(cst816t_t *dev, uint8_t seconds);

esp_err_t cst816t_read_reg(cst816t_t *dev, uint8_t reg, uint8_t *data, size_t len);

esp_err_t cst816t_write_reg(cst816t_t *dev, uint8_t reg, uint8_t data);

const char *cst816t_gesture_to_string(cst816t_gesture_t gesture);

#ifdef __cplusplus
}
#endif
