/**
 * @file battery_monitor.h
 * @brief ESP32-S3 电池电量监控模块头文件
 * 
 * 功能：通过ADC采样分压电路测量电池电压并计算电量百分比
 * 硬件：ESP32-S3 + 分压电路 (R19=200kΩ, R20=100kΩ)
 */

#ifndef BATTERY_MONITOR_H
#define BATTERY_MONITOR_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

// ==================== 配置参数 ====================

// 当前样机已安装电池，启用 ADC 电压采样和界面电量刷新。
#define BATTERY_MONITOR_HAS_BATTERY 1

/**
 * @brief 电池电压配置（请根据实际电池修改）
 */
#define BATTERY_MAX_VOLTAGE     4200    ///< 电池最大电压 (mV) - 满电电压
#define BATTERY_MIN_VOLTAGE     3000    ///< 电池最小电压 (mV) - 空电电压
#define BATTERY_NOMINAL_VOLTAGE 3700    ///< 电池标称电压 (mV)

/**
 * @brief ADC硬件配置
 */
#define BATTERY_ADC_GPIO        9       ///< ADC GPIO引脚号 (IO9)
#define BATTERY_ADC_UNIT        ADC_UNIT_1      ///< ADC单元
// ESP32-S3: GPIO9 -> ADC1_CH8
#define BATTERY_ADC_CHANNEL     ADC_CHANNEL_8   ///< ADC通道 (IO9对应ADC1_CH8)
#define BATTERY_ADC_ATTEN       ADC_ATTEN_DB_6  ///< BAT+ 最大4.2V时分压点约1.4V，适用6dB档
#define BATTERY_ADC_BITWIDTH    ADC_BITWIDTH_12 ///< 12位分辨率

/**
 * @brief 断开电池时的防浮空配置
 *
 * 仅靠ADC输入点悬空时会出现“拔掉电池仍读到高电压/随机值”的现象。
 * 开启内部下拉可让未连接时更接近0V（若测点被充电芯片/电源轨驱动，则仍可能读到高电压）。
 */
// 注意：你当前分压为 200k/100k，内部下拉阻值远小于分压电阻，会显著拉低测量点电压，导致“插上电池读数很低”。
// 如确需在“拔电池悬空”时让读数归零，建议硬件增加 1M 左右下拉，或把分压电阻整体降到更低阻值。
#define BATTERY_ADC_ENABLE_INTERNAL_PULLDOWN  0

/**
 * @brief 分压电路参数
 */
#define R19_RESISTANCE          200000  ///< 上拉电阻 (Ω)
#define R20_RESISTANCE          100000  ///< 下拉电阻 (Ω)

/**
 * @brief 采样配置
 */
#define BATTERY_SAMPLE_COUNT    16      ///< 多次采样取平均值的次数
#define BATTERY_DISCARD_COUNT   2       ///< 丢弃前几次采样，减小ADC采样电容带来的首样偏差
#define BATTERY_SAMPLE_INTERVAL 10      ///< 采样间隔 (ms)

/**
 * @brief 软件校准系数
 *
 * 用万用表量 BAT+ 后，如果日志里的“电池=”偏低/偏高，可按：
 * 新系数 = 旧系数 * 万用表电压 / 日志电池电压
 */
#define BATTERY_CALIBRATION_NUM 1000
#define BATTERY_CALIBRATION_DEN 1000

// ADC 校准不可用时的粗略满量程；正常路径优先使用芯片校准结果。
#define BATTERY_ADC_FALLBACK_FULL_SCALE_MV 1750

// ==================== 数据结构 ====================

/**
 * @brief 电池信息结构体
 */
typedef struct {
    int voltage_mv;         ///< 电池电压 (mV)
    int percentage;         ///< 电池电量百分比 (0-100)，-1 表示未安装电池
    const char *status;     ///< 电池状态描述
} battery_info_t;

// ==================== API函数 ====================

/**
 * @brief 初始化电池监控模块
 * 
 * 初始化ADC硬件和校准参数
 * 
 * @return 
 *     - true: 初始化成功
 *     - false: 初始化失败
 */
bool battery_monitor_init(void);

/**
 * @brief 读取电池电压
 * 
 * 通过ADC采样并计算实际电池电压
 * 
 * @return 电池电压 (mV)
 */
int battery_read_voltage(void);

/**
 * @brief 计算电池电量百分比
 * 
 * @param voltage_mv 电池电压 (mV)
 * @return 电池电量百分比 (0-100)
 */
int battery_calculate_percentage(int voltage_mv);

/**
 * @brief 获取电池状态描述
 * 
 * @param percentage 电池电量百分比
 * @return 状态描述字符串
 */
const char* battery_get_status(int percentage);

/**
 * @brief 获取完整的电池信息
 * 
 * 一次性获取电压、电量和状态
 * 
 * @param info 输出参数，存储电池信息
 * @return 
 *     - true: 获取成功
 *     - false: 获取失败
 */
bool battery_get_info(battery_info_t *info);

/**
 * @brief 启动电池监控任务
 * 
 * 创建FreeRTOS任务，定期监控电池状态并打印日志
 * 
 * @param interval_ms 监控周期 (ms)，0表示使用默认值5000ms
 * @return 
 *     - true: 任务创建成功
 *     - false: 任务创建失败
 */
bool battery_start_monitor_task(uint32_t interval_ms);

#ifdef __cplusplus
}
#endif

#endif // BATTERY_MONITOR_H
