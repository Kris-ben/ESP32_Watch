/**
 * @file baidu_asr.h
 * @brief 百度语音识别（ASR）驱动
 * 
 * 功能：
 * - 支持在线语音识别
 * - 支持短语音识别（60秒内）
 * - 自动处理音频格式
 */

#ifndef BAIDU_ASR_H
#define BAIDU_ASR_H

#include <stdint.h>
#include <stdbool.h>
#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief 百度ASR语言模型ID
 */
typedef enum {
    BAIDU_ASR_PID_MANDARIN = 1537,       /*!< 普通话(纯中文识别，近场) - 推荐 */
    BAIDU_ASR_PID_MANDARIN_FAR = 1936,   /*!< 普通话远场 */
    BAIDU_ASR_PID_ENGLISH = 1737,        /*!< 英语 */
    BAIDU_ASR_PID_CANTONESE = 1637,      /*!< 粤语 */
    BAIDU_ASR_PID_SICHUAN = 1837,        /*!< 四川话 */
} baidu_asr_pid_t;

/**
 * @brief 百度ASR配置结构体
 */
typedef struct {
    const char *api_key;             /*!< 百度API Key */
    const char *secret_key;          /*!< 百度Secret Key */
    uint32_t sample_rate;            /*!< 采样率 (8000/16000) */
    uint32_t timeout_ms;             /*!< 请求超时时间(毫秒) */
    baidu_asr_pid_t dev_pid;         /*!< 语言模型ID，默认1537普通话近场 */
    bool enable_punctuation;         /*!< 是否启用标点符号 */
    uint8_t max_retries;             /*!< 最大重试次数，默认1 */
} baidu_asr_config_t;

/**
 * @brief 百度ASR客户端句柄
 */
typedef struct {
    baidu_asr_config_t config;       /*!< ASR配置 */
    char access_token[512];          /*!< 访问令牌 */
    int64_t token_expire_time;       /*!< 令牌过期时间(微秒) */
} baidu_asr_handle_t;

/**
 * @brief 初始化百度ASR
 * 
 * @param handle ASR句柄指针
 * @param config ASR配置
 * @return 
 *     - ESP_OK: 成功
 *     - ESP_ERR_INVALID_ARG: 参数错误
 */
esp_err_t baidu_asr_init(baidu_asr_handle_t *handle, const baidu_asr_config_t *config);

/**
 * @brief 获取访问令牌
 * 
 * @param handle ASR句柄
 * @return 
 *     - ESP_OK: 成功
 *     - ESP_FAIL: 失败
 */
esp_err_t baidu_asr_get_token(baidu_asr_handle_t *handle);

/**
 * @brief 语音识别
 * 
 * @param handle ASR句柄
 * @param audio_data 音频数据(16bit PCM)
 * @param audio_len 音频数据长度(字节)
 * @param result 识别结果缓冲区
 * @param result_size 结果缓冲区大小
 * @return 
 *     - ESP_OK: 成功
 *     - ESP_ERR_INVALID_ARG: 参数错误
 *     - ESP_FAIL: 识别失败
 */
esp_err_t baidu_asr_recognize(baidu_asr_handle_t *handle, 
                               const int16_t *audio_data, 
                               size_t audio_len,
                               char *result, 
                               size_t result_size);

#ifdef __cplusplus
}
#endif

#endif /* BAIDU_ASR_H */
