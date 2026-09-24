/**
 * @file sd_spi_config.h
 * @brief SD卡SPI驱动配置文件
 * @note 移植时只需修改此文件中的配置参数
 * @date 2025-12-25
 * 
 * @par 移植说明:
 * 1. 修改GPIO引脚定义以匹配你的硬件连接
 * 2. 根据需要调整SPI主机和时钟速度
 * 3. SD卡挂载代码通过 sd_card_fs.c 使用本配置
 */

#ifndef SD_SPI_CONFIG_H
#define SD_SPI_CONFIG_H

#include "driver/gpio.h"
#include "driver/spi_master.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ==================== GPIO引脚配置 ==================== */
/* 根据你的硬件连接修改以下引脚定义 */

#define SD_PIN_MISO         GPIO_NUM_18     /**< MISO引脚 (SD卡 DAT0, pin7) */
#define SD_PIN_MOSI         GPIO_NUM_16     /**< MOSI引脚 (SD卡 CMD, pin3) */
#define SD_PIN_CLK          GPIO_NUM_17     /**< CLK引脚  (SD卡 CLK, pin5) */
#define SD_PIN_CS           GPIO_NUM_20     /**< CS引脚   (SD卡 CD/DAT3, pin2) */

/* ==================== SPI配置 ==================== */

/**
 * @brief SPI主机选择
 * @note ESP32系列通常有多个SPI主机:
 *       - SPI1_HOST: 通常用于Flash
 *       - SPI2_HOST: 通用SPI (HSPI)
 *       - SPI3_HOST: 通用SPI (VSPI) - 仅部分芯片支持
 */
#define SD_SPI_HOST         SPI3_HOST

/**
 * @brief SPI DMA通道
 * @note 使用自动分配可以避免DMA通道冲突
 */
#define SD_SPI_DMA_CHAN     SPI_DMA_CH_AUTO

/**
 * @brief 初始化时SPI时钟速度 (Hz)
 * @note SD卡规范要求初始化时使用100-400kHz
 */
#define SD_SPI_CLOCK_INIT   400000

/**
 * @brief 正常工作时SPI时钟速度 (Hz)
 * @note 可根据SD卡和硬件情况调整，最高可达25MHz
 *       如果通信不稳定，可以降低此值
 */
#define SD_SPI_CLOCK_FAST   20000000

/* ==================== 超时配置 ==================== */

/**
 * @brief 命令超时时间 (毫秒)
 */
#define SD_CMD_TIMEOUT_MS   1000

/**
 * @brief 初始化超时时间 (毫秒)
 */
#define SD_INIT_TIMEOUT_MS  5000

/**
 * @brief 写操作等待超时时间 (毫秒)
 */
#define SD_WRITE_TIMEOUT_MS 500

/**
 * @brief 读操作等待超时时间 (毫秒)
 */
#define SD_READ_TIMEOUT_MS  500

/* ==================== 调试配置 ==================== */

/**
 * @brief 启用调试日志
 * @note 设置为1启用详细调试信息，0关闭
 */
#define SD_DEBUG_ENABLE     0

#ifdef __cplusplus
}
#endif

#endif /* SD_SPI_CONFIG_H */
