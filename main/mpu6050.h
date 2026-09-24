#ifndef MPU6050_H
#define MPU6050_H

#include <stdint.h>
#include "driver/i2c.h"
#include "esp_err.h"

// MPU6050 I2C地址 (AD0引脚接地时为0x68)
#define MPU6050_ADDR            0x68

// MPU6050寄存器地址
#define MPU6050_REG_SMPLRT_DIV      0x19
#define MPU6050_REG_CONFIG          0x1A
#define MPU6050_REG_GYRO_CONFIG     0x1B
#define MPU6050_REG_ACCEL_CONFIG    0x1C
#define MPU6050_REG_INT_ENABLE      0x38
#define MPU6050_REG_ACCEL_XOUT_H    0x3B
#define MPU6050_REG_ACCEL_XOUT_L    0x3C
#define MPU6050_REG_ACCEL_YOUT_H    0x3D
#define MPU6050_REG_ACCEL_YOUT_L    0x3E
#define MPU6050_REG_ACCEL_ZOUT_H    0x3F
#define MPU6050_REG_ACCEL_ZOUT_L    0x40
#define MPU6050_REG_TEMP_OUT_H      0x41
#define MPU6050_REG_TEMP_OUT_L      0x42
#define MPU6050_REG_GYRO_XOUT_H     0x43
#define MPU6050_REG_GYRO_XOUT_L     0x44
#define MPU6050_REG_GYRO_YOUT_H     0x45
#define MPU6050_REG_GYRO_YOUT_L     0x46
#define MPU6050_REG_GYRO_ZOUT_H     0x47
#define MPU6050_REG_GYRO_ZOUT_L     0x48
#define MPU6050_REG_PWR_MGMT_1      0x6B
#define MPU6050_REG_WHO_AM_I        0x75

// 陀螺仪量程配置
#define MPU6050_GYRO_FS_250         0x00  // ±250°/s
#define MPU6050_GYRO_FS_500         0x08  // ±500°/s
#define MPU6050_GYRO_FS_1000        0x10  // ±1000°/s
#define MPU6050_GYRO_FS_2000        0x18  // ±2000°/s

// 加速度计量程配置
#define MPU6050_ACCEL_FS_2          0x00  // ±2g
#define MPU6050_ACCEL_FS_4          0x08  // ±4g
#define MPU6050_ACCEL_FS_8          0x10  // ±8g
#define MPU6050_ACCEL_FS_16         0x18  // ±16g

// I2C配置
#define I2C_MASTER_NUM              I2C_NUM_1  // 与RTC共用I2C_NUM_1
#define I2C_MASTER_FREQ_HZ          100000     // 100kHz（与RTC保持一致）
#define I2C_MASTER_TX_BUF_DISABLE   0
#define I2C_MASTER_RX_BUF_DISABLE   0
#define I2C_MASTER_TIMEOUT_MS       1000

// GPIO引脚定义（根据电路图，与RTC共用）
#define I2C_MASTER_SCL_IO           14      // SCL引脚
#define I2C_MASTER_SDA_IO           21      // SDA引脚

// MPU6050数据结构
typedef struct {
    int16_t accel_x;
    int16_t accel_y;
    int16_t accel_z;
    int16_t temp;
    int16_t gyro_x;
    int16_t gyro_y;
    int16_t gyro_z;
} mpu6050_raw_data_t;

typedef struct {
    float accel_x;
    float accel_y;
    float accel_z;
    float temp;
    float gyro_x;
    float gyro_y;
    float gyro_z;
} mpu6050_data_t;

// 步数计算相关配置
#define STEP_THRESHOLD          1.2f    // 步数检测阈值（g）
#define STEP_MIN_INTERVAL_MS    300     // 最小步间隔（毫秒）
#define STEP_MAX_INTERVAL_MS    2000    // 最大步间隔（毫秒）
#define STEP_FILTER_SIZE        5       // 滑动平均滤波器大小

// 步数计数器结构
typedef struct {
    uint32_t step_count;                // 总步数
    float last_magnitude;               // 上次加速度幅值
    int64_t last_step_time;             // 上次检测到步数的时间（毫秒）
    float accel_buffer[STEP_FILTER_SIZE]; // 加速度滤波缓冲区
    uint8_t buffer_index;               // 缓冲区索引
    bool buffer_filled;                 // 缓冲区是否已填满
    float threshold;                    // 动态阈值
} step_counter_t;

// 函数声明
esp_err_t mpu6050_init(void);
esp_err_t mpu6050_read_raw(mpu6050_raw_data_t *data);
esp_err_t mpu6050_read_data(mpu6050_data_t *data);
esp_err_t mpu6050_test_connection(void);
esp_err_t mpu6050_set_gyro_range(uint8_t range);
esp_err_t mpu6050_set_accel_range(uint8_t range);

// 步数计算函数
void step_counter_init(step_counter_t *counter);
uint32_t step_counter_update(step_counter_t *counter, const mpu6050_data_t *data);

#endif // MPU6050_H
