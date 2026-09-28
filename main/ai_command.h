/**
 * @file ai_command.h
 * @brief AI指令解析和控制模块
 * @details 解析大模型回复中的控制指令，执行设备控制
 * 
 * 指令格式: [CMD:命令:参数1:参数2:...]
 * 示例:
 *   [CMD:BRIGHTNESS:80]       - 设置亮度80%
 *   [CMD:VOLUME:50]           - 设置音量50%
 *   [CMD:ALARM:0:07:30:ON]    - 设置闹钟0为7:30并启用
 *   [CMD:ALARM:1:OFF]         - 关闭闹钟1
 *   [CMD:SCREEN:home]         - 切换到主界面
 *   [CMD:SCREEN:weather]      - 切换到天气界面
 *   [CMD:SCREEN:wifi]         - 切换到WiFi界面
 *   [CMD:SCREEN:clock]        - 切换到闹钟界面
 *   [CMD:WEATHER]             - 播报当前天气
 */

#ifndef AI_COMMAND_H
#define AI_COMMAND_H

#include <stdint.h>
#include <stdbool.h>
#include "esp_err.h"
#include "gui_guider.h"

#ifdef __cplusplus
extern "C" {
#endif

// 支持的命令类型
typedef enum {
    AI_CMD_UNKNOWN = 0,
    AI_CMD_BRIGHTNESS,      // 亮度控制
    AI_CMD_VOLUME,          // 音量控制
    AI_CMD_ALARM_SET,       // 设置闹钟
    AI_CMD_ALARM_ENABLE,    // 启用/禁用闹钟
    AI_CMD_SCREEN,          // 切换界面
    AI_CMD_WEATHER,         // 查询天气
    AI_CMD_TIME,            // 报时
} ai_cmd_type_t;

// 命令结构
typedef struct {
    ai_cmd_type_t type;
    union {
        struct {
            uint8_t value;      // 0-100
        } brightness;
        struct {
            uint8_t value;      // 0-100
        } volume;
        struct {
            uint8_t index;      // 0-3
            uint8_t hour;       // 0-23
            uint8_t minute;     // 0-59
            bool enabled;
        } alarm;
        struct {
            char name[32];      // 界面名称
        } screen;
    } params;
} ai_command_t;

/**
 * @brief 初始化AI指令模块
 * @param ui UI句柄
 * @return ESP_OK成功
 */
esp_err_t ai_command_init(lv_ui *ui);

/**
 * @brief 解析并执行AI回复中的指令
 * @param response AI回复文本
 * @param clean_response 输出去除指令后的纯文本回复（可为NULL，也可以与response指向同一缓冲区）
 * @param max_len clean_response缓冲区大小
 * @return 执行的指令数量
 */
int ai_command_parse_and_execute(const char *response, char *clean_response, size_t max_len);

/**
 * @brief 手动执行单个指令
 * @param cmd 指令结构
 * @return ESP_OK成功
 */
esp_err_t ai_command_execute(const ai_command_t *cmd);

/**
 * @brief 获取当前系统状态描述（用于发送给AI）
 * @param buffer 输出缓冲区
 * @param max_len 缓冲区大小
 * @return 实际写入长度
 */
int ai_command_get_system_prompt(char *buffer, size_t max_len);

#ifdef __cplusplus
}
#endif

#endif // AI_COMMAND_H
