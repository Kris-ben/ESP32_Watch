/**
 * @file voice_input.h
 * @brief 语音输入模块（按需录音）
 * 
 * 功能：
 * - 按需录制语音（触发后录音）
 * - 调用百度语音识别
 * - PDM麦克风使用I2S0，音频输出使用I2S1
 */

#ifndef VOICE_INPUT_H
#define VOICE_INPUT_H

#include <stdint.h>
#include <stdbool.h>
#include "esp_err.h"
#include "max98357a.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief 语音输入状态
 */
typedef enum {
    VOICE_INPUT_IDLE,           /*!< 空闲状态 */
    VOICE_INPUT_RECORDING,      /*!< 正在录音 */
    VOICE_INPUT_RECOGNIZING,    /*!< 正在识别 */
    VOICE_INPUT_ERROR,          /*!< 错误状态 */
} voice_input_state_t;

/**
 * @brief 语音输入配置
 */
typedef struct {
    int mic_data_pin;            /*!< 麦克风数据引脚 */
    int mic_clk_pin;             /*!< 麦克风时钟引脚 */
    uint32_t sample_rate;        /*!< 采样率 (16000) */
    uint32_t max_record_ms;      /*!< 最大录音时长(毫秒) */
    uint32_t silence_timeout_ms; /*!< 静音超时，用于检测说话结束(毫秒) */
    uint16_t silence_threshold;  /*!< 静音阈值 */
    const char *api_key;         /*!< 百度API Key */
    const char *secret_key;      /*!< 百度Secret Key */
} voice_input_config_t;

/**
 * @brief 语音输入句柄
 */
typedef struct voice_input_handle *voice_input_handle_t;

/**
 * @brief 初始化语音输入模块
 * 
 * @param config 配置参数
 * @param audio_handle 音频输出句柄
 * @param handle 返回的句柄
 * @return 
 *     - ESP_OK: 成功
 *     - ESP_ERR_INVALID_ARG: 参数错误
 *     - ESP_ERR_NO_MEM: 内存不足
 */
esp_err_t voice_input_init(const voice_input_config_t *config,
                           max98357a_handle_t *audio_handle,
                           voice_input_handle_t *handle);

/**
 * @brief 开始录音并识别
 * 
 * 此函数会：
 * 1. 停止音频输出
 * 2. 初始化麦克风
 * 3. 录制语音直到静音或超时
 * 4. 调用百度ASR识别
 * 5. 恢复音频输出
 * 
 * @param handle 句柄
 * @param result 识别结果缓冲区
 * @param result_size 缓冲区大小
 * @return 
 *     - ESP_OK: 成功
 *     - ESP_FAIL: 失败
 */
esp_err_t voice_input_record_and_recognize(voice_input_handle_t handle,
                                            char *result,
                                            size_t result_size);

/**
 * @brief 获取当前状态
 * 
 * @param handle 句柄
 * @return 当前状态
 */
voice_input_state_t voice_input_get_state(voice_input_handle_t handle);

/**
 * @brief 设置外部停止录音标志
 * 
 * @param handle 句柄
 * @param stop 是否停止
 */
void voice_input_set_stop_flag(voice_input_handle_t handle, bool stop);

/**
 * @brief 释放资源
 * 
 * @param handle 句柄
 */
void voice_input_deinit(voice_input_handle_t handle);

/**
 * @brief 获取默认配置
 * 
 * @return 默认配置
 */
voice_input_config_t voice_input_get_default_config(void);

#ifdef __cplusplus
}
#endif

#endif /* VOICE_INPUT_H */
