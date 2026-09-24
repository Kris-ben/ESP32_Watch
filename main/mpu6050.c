#include "mpu6050.h"
#include "esp_log.h"
#include "esp_timer.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <string.h>
#include <math.h>

static const char *TAG = "MPU6050";

// 当前配置的量程
static uint8_t gyro_range = MPU6050_GYRO_FS_250;
static uint8_t accel_range = MPU6050_ACCEL_FS_2;

// 量程对应的灵敏度系数
static float gyro_sensitivity = 131.0f;   // LSB/(°/s) for ±250°/s
static float accel_sensitivity = 16384.0f; // LSB/g for ±2g

/**
 * @brief 写入单个寄存器
 */
static esp_err_t mpu6050_write_reg(uint8_t reg_addr, uint8_t data)
{
    uint8_t write_buf[2] = {reg_addr, data};
    return i2c_master_write_to_device(I2C_MASTER_NUM, MPU6050_ADDR, 
                                      write_buf, sizeof(write_buf), 
                                      pdMS_TO_TICKS(I2C_MASTER_TIMEOUT_MS));
}

/**
 * @brief 读取单个寄存器
 */
static esp_err_t mpu6050_read_reg(uint8_t reg_addr, uint8_t *data)
{
    return i2c_master_write_read_device(I2C_MASTER_NUM, MPU6050_ADDR,
                                        &reg_addr, 1, data, 1,
                                        pdMS_TO_TICKS(I2C_MASTER_TIMEOUT_MS));
}

/**
 * @brief 读取多个寄存器
 */
static esp_err_t mpu6050_read_regs(uint8_t reg_addr, uint8_t *data, size_t len)
{
    return i2c_master_write_read_device(I2C_MASTER_NUM, MPU6050_ADDR,
                                        &reg_addr, 1, data, len,
                                        pdMS_TO_TICKS(I2C_MASTER_TIMEOUT_MS));
}

/**
 * @brief 初始化I2C总线
 */
static esp_err_t i2c_master_init(void)
{
    // 先删除可能存在的I2C驱动
    i2c_driver_delete(I2C_MASTER_NUM);
    
    i2c_config_t conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = I2C_MASTER_SDA_IO,
        .scl_io_num = I2C_MASTER_SCL_IO,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = I2C_MASTER_FREQ_HZ,
        .clk_flags = 0,
    };

    esp_err_t err = i2c_param_config(I2C_MASTER_NUM, &conf);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "I2C参数配置失败: 0x%x", err);
        return err;
    }

    err = i2c_driver_install(I2C_MASTER_NUM, conf.mode,
                            I2C_MASTER_RX_BUF_DISABLE,
                            I2C_MASTER_TX_BUF_DISABLE, 0);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "I2C驱动安装失败: 0x%x", err);
        return err;
    }

    ESP_LOGI(TAG, "I2C初始化成功 (SDA: GPIO%d, SCL: GPIO%d, 频率: %dHz)",
             I2C_MASTER_SDA_IO, I2C_MASTER_SCL_IO, I2C_MASTER_FREQ_HZ);
    return ESP_OK;
}

/**
 * @brief 测试MPU6050连接
 */
esp_err_t mpu6050_test_connection(void)
{
    uint8_t who_am_i;
    esp_err_t err = mpu6050_read_reg(MPU6050_REG_WHO_AM_I, &who_am_i);
    
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "读取WHO_AM_I寄存器失败");
        return err;
    }

    ESP_LOGI(TAG, "WHO_AM_I: 0x%02X (期望值: 0x68)", who_am_i);
    
    if (who_am_i == 0x68) {
        ESP_LOGI(TAG, "MPU6050连接成功");
        return ESP_OK;
    } else {
        ESP_LOGE(TAG, "MPU6050连接失败，WHO_AM_I值不正确");
        return ESP_FAIL;
    }
}

/**
 * @brief 设置陀螺仪量程
 */
esp_err_t mpu6050_set_gyro_range(uint8_t range)
{
    esp_err_t err = mpu6050_write_reg(MPU6050_REG_GYRO_CONFIG, range);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "设置陀螺仪量程失败");
        return err;
    }

    gyro_range = range;
    
    // 更新灵敏度系数
    switch (range) {
        case MPU6050_GYRO_FS_250:
            gyro_sensitivity = 131.0f;
            ESP_LOGI(TAG, "陀螺仪量程设置为 ±250°/s");
            break;
        case MPU6050_GYRO_FS_500:
            gyro_sensitivity = 65.5f;
            ESP_LOGI(TAG, "陀螺仪量程设置为 ±500°/s");
            break;
        case MPU6050_GYRO_FS_1000:
            gyro_sensitivity = 32.8f;
            ESP_LOGI(TAG, "陀螺仪量程设置为 ±1000°/s");
            break;
        case MPU6050_GYRO_FS_2000:
            gyro_sensitivity = 16.4f;
            ESP_LOGI(TAG, "陀螺仪量程设置为 ±2000°/s");
            break;
        default:
            ESP_LOGW(TAG, "未知的陀螺仪量程");
            break;
    }

    return ESP_OK;
}

/**
 * @brief 设置加速度计量程
 */
esp_err_t mpu6050_set_accel_range(uint8_t range)
{
    esp_err_t err = mpu6050_write_reg(MPU6050_REG_ACCEL_CONFIG, range);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "设置加速度计量程失败");
        return err;
    }

    accel_range = range;
    
    // 更新灵敏度系数
    switch (range) {
        case MPU6050_ACCEL_FS_2:
            accel_sensitivity = 16384.0f;
            ESP_LOGI(TAG, "加速度计量程设置为 ±2g");
            break;
        case MPU6050_ACCEL_FS_4:
            accel_sensitivity = 8192.0f;
            ESP_LOGI(TAG, "加速度计量程设置为 ±4g");
            break;
        case MPU6050_ACCEL_FS_8:
            accel_sensitivity = 4096.0f;
            ESP_LOGI(TAG, "加速度计量程设置为 ±8g");
            break;
        case MPU6050_ACCEL_FS_16:
            accel_sensitivity = 2048.0f;
            ESP_LOGI(TAG, "加速度计量程设置为 ±16g");
            break;
        default:
            ESP_LOGW(TAG, "未知的加速度计量程");
            break;
    }

    return ESP_OK;
}

/**
 * @brief 初始化MPU6050（包含I2C初始化）
 */
esp_err_t mpu6050_init(void)
{
    esp_err_t err;

    ESP_LOGI(TAG, "开始初始化MPU6050...");
    
    // 初始化I2C总线
    err = i2c_master_init();
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "I2C初始化失败: %s", esp_err_to_name(err));
        return err;
    }

    // 等待MPU6050上电稳定
    vTaskDelay(pdMS_TO_TICKS(100));

    // 测试连接
    err = mpu6050_test_connection();
    if (err != ESP_OK) {
        return err;
    }

    // 复位设备
    err = mpu6050_write_reg(MPU6050_REG_PWR_MGMT_1, 0x80);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "复位MPU6050失败");
        return err;
    }
    vTaskDelay(pdMS_TO_TICKS(100));

    // 唤醒设备（退出睡眠模式）
    err = mpu6050_write_reg(MPU6050_REG_PWR_MGMT_1, 0x00);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "唤醒MPU6050失败");
        return err;
    }
    vTaskDelay(pdMS_TO_TICKS(10));

    // 设置采样率分频器 (1kHz / (1 + 7) = 125Hz)
    err = mpu6050_write_reg(MPU6050_REG_SMPLRT_DIV, 0x07);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "设置采样率失败");
        return err;
    }

    // 配置数字低通滤波器 (DLPF_CFG = 3, 带宽44Hz)
    err = mpu6050_write_reg(MPU6050_REG_CONFIG, 0x03);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "配置低通滤波器失败");
        return err;
    }

    // 设置陀螺仪量程为 ±250°/s
    err = mpu6050_set_gyro_range(MPU6050_GYRO_FS_250);
    if (err != ESP_OK) {
        return err;
    }

    // 设置加速度计量程为 ±2g
    err = mpu6050_set_accel_range(MPU6050_ACCEL_FS_2);
    if (err != ESP_OK) {
        return err;
    }

    ESP_LOGI(TAG, "MPU6050初始化完成");
    return ESP_OK;
}

/**
 * @brief 读取原始数据
 */
esp_err_t mpu6050_read_raw(mpu6050_raw_data_t *data)
{
    if (data == NULL) {
        ESP_LOGE(TAG, "数据指针为空");
        return ESP_ERR_INVALID_ARG;
    }

    uint8_t buffer[14];
    
    // 从0x3B开始连续读取14个字节
    esp_err_t err = mpu6050_read_regs(MPU6050_REG_ACCEL_XOUT_H, buffer, 14);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "读取传感器数据失败");
        return err;
    }

    // 组合高低字节（大端序）
    data->accel_x = (int16_t)((buffer[0] << 8) | buffer[1]);
    data->accel_y = (int16_t)((buffer[2] << 8) | buffer[3]);
    data->accel_z = (int16_t)((buffer[4] << 8) | buffer[5]);
    data->temp = (int16_t)((buffer[6] << 8) | buffer[7]);
    data->gyro_x = (int16_t)((buffer[8] << 8) | buffer[9]);
    data->gyro_y = (int16_t)((buffer[10] << 8) | buffer[11]);
    data->gyro_z = (int16_t)((buffer[12] << 8) | buffer[13]);

    return ESP_OK;
}

/**
 * @brief 读取转换后的数据
 */
esp_err_t mpu6050_read_data(mpu6050_data_t *data)
{
    if (data == NULL) {
        ESP_LOGE(TAG, "数据指针为空");
        return ESP_ERR_INVALID_ARG;
    }

    mpu6050_raw_data_t raw_data;
    esp_err_t err = mpu6050_read_raw(&raw_data);
    if (err != ESP_OK) {
        return err;
    }

    // 转换加速度数据 (单位: g)
    data->accel_x = (float)raw_data.accel_x / accel_sensitivity;
    data->accel_y = (float)raw_data.accel_y / accel_sensitivity;
    data->accel_z = (float)raw_data.accel_z / accel_sensitivity;

    // 转换温度数据 (单位: °C)
    // 公式: Temperature = (TEMP_OUT / 340) + 36.53
    data->temp = ((float)raw_data.temp / 340.0f) + 36.53f;

    // 转换陀螺仪数据 (单位: °/s)
    data->gyro_x = (float)raw_data.gyro_x / gyro_sensitivity;
    data->gyro_y = (float)raw_data.gyro_y / gyro_sensitivity;
    data->gyro_z = (float)raw_data.gyro_z / gyro_sensitivity;

    return ESP_OK;
}

/**
 * @brief 初始化步数计数器
 */
void step_counter_init(step_counter_t *counter)
{
    if (counter == NULL) {
        return;
    }
    
    counter->step_count = 0;
    counter->last_magnitude = 0.0f;
    counter->last_step_time = 0;
    counter->buffer_index = 0;
    counter->buffer_filled = false;
    counter->threshold = STEP_THRESHOLD;
    
    // 初始化滤波缓冲区
    for (int i = 0; i < STEP_FILTER_SIZE; i++) {
        counter->accel_buffer[i] = 0.0f;
    }
    
    ESP_LOGI(TAG, "步数计数器初始化完成");
}

/**
 * @brief 计算加速度幅值（合成加速度）
 */
static float calculate_magnitude(float x, float y, float z)
{
    return sqrtf(x * x + y * y + z * z);
}

/**
 * @brief 滑动平均滤波
 */
static float apply_moving_average(step_counter_t *counter, float new_value)
{
    // 将新值添加到缓冲区
    counter->accel_buffer[counter->buffer_index] = new_value;
    counter->buffer_index = (counter->buffer_index + 1) % STEP_FILTER_SIZE;
    
    // 检查缓冲区是否已填满
    if (counter->buffer_index == 0) {
        counter->buffer_filled = true;
    }
    
    // 计算平均值
    float sum = 0.0f;
    int count = counter->buffer_filled ? STEP_FILTER_SIZE : counter->buffer_index;
    
    for (int i = 0; i < count; i++) {
        sum += counter->accel_buffer[i];
    }
    
    return sum / count;
}

/**
 * @brief 更新步数计数器
 * @param counter 步数计数器指针
 * @param data MPU6050数据
 * @return 当前总步数
 */
uint32_t step_counter_update(step_counter_t *counter, const mpu6050_data_t *data)
{
    if (counter == NULL || data == NULL) {
        return 0;
    }
    
    // 计算加速度幅值
    float magnitude = calculate_magnitude(data->accel_x, data->accel_y, data->accel_z);
    
    // 应用滑动平均滤波，减少噪声
    float filtered_magnitude = apply_moving_average(counter, magnitude);
    
    // 获取当前时间（毫秒）
    int64_t current_time = esp_timer_get_time() / 1000;
    
    // 计算与上次步数的时间间隔
    int64_t time_diff = current_time - counter->last_step_time;
    
    // 检测步数：
    // 1. 加速度幅值超过阈值
    // 2. 与上次检测的时间间隔在合理范围内
    // 3. 检测到从低于阈值到高于阈值的变化（上升沿）
    if (filtered_magnitude > counter->threshold &&
        counter->last_magnitude <= counter->threshold &&
        time_diff >= STEP_MIN_INTERVAL_MS) {
        
        // 如果时间间隔过长，可能是静止后重新开始行走
        if (time_diff > STEP_MAX_INTERVAL_MS && counter->step_count > 0) {
            ESP_LOGI(TAG, "检测到长时间静止，继续计步");
        }
        
        counter->step_count++;
        counter->last_step_time = current_time;
        
        ESP_LOGI(TAG, "检测到步数！总步数: %lu, 幅值: %.3f g",
                 counter->step_count, filtered_magnitude);
    }
    
    // 更新上次幅值
    counter->last_magnitude = filtered_magnitude;
    
    return counter->step_count;
}

