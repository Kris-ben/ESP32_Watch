/**
 * @file sd_card_fs.h
 * @brief SD卡文件系统模块（基于ESP-IDF FatFS + SDSPI）
 * 
 * 使用 sd_spi_config.h 中定义的引脚配置，
 * 通过 ESP-IDF VFS FAT 挂载SD卡为 /sdcard 文件系统。
 */

#ifndef SD_CARD_FS_H
#define SD_CARD_FS_H

#include <stdbool.h>
#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

/** SD卡挂载路径 */
#define SD_MOUNT_POINT  "/sdcard"
#define SD_MUSIC_NAME_MAX 256

/**
 * @brief 挂载SD卡文件系统
 * @return ESP_OK 成功, 其他值 失败
 */
esp_err_t sd_card_fs_mount(void);

/**
 * @brief 递归列出SD卡上所有文件（日志输出）
 * @param path 起始路径，NULL 表示根目录
 * @param depth 当前递归深度（外部调用传0）
 */
void sd_card_fs_list_all(const char *path, int depth);

/**
 * @brief 获取SD卡根目录中的歌曲文件名列表
 * @param names 输出文件名数组，每项可容纳 255 字节文件名及结尾零字节
 * @param max_count 最大返回数量
 * @return 实际返回数量
 */
int sd_card_fs_get_music_files(char names[][SD_MUSIC_NAME_MAX], int max_count);

#ifdef __cplusplus
}
#endif

#endif // SD_CARD_FS_H
