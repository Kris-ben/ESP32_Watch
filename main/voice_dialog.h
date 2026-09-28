/**
 * @file voice_dialog.h
 * @brief 语音对话管理模块
 * 
 * 功能：
 * - 在设备上持续监听唤醒词"你好小智"
 * - 唤醒后录制用户语音并识别
 * - 将识别结果发送给AI大模型
 * - AI回复通过TTS播放
 * - 麦克风和喇叭分别使用I2S0、I2S1
 * 
 * 工作流程：
 * 1. 麦克风持续录音检测唤醒词
 * 2. 检测到唤醒词后，播放提示音
 * 3. 录制用户语音（VAD检测结束）
 * 4. 保持麦克风开启，进行云端语音识别
 * 5. 调用百度ASR进行语音识别
 * 6. 将识别文本发送给Spark大模型
 * 7. 接收AI回复并通过TTS播放
 * 8. 播报中可喊“你好小智”打断；空闲超时后恢复唤醒词监听
 * 
 * 资源管理策略：
 * - PDM麦克风使用I2S_NUM_0
 * - 音频输出使用I2S_NUM_1
 * - 大缓冲区优先使用PSRAM
 * - 任务优先级合理分配
 */

#ifndef VOICE_DIALOG_H
#define VOICE_DIALOG_H

#include <stdint.h>
#include <stdbool.h>
#include "esp_err.h"
#include "spark_chat.h"
#include "max98357a.h"
#include "baidu_tts.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief 语音对话状态
 */
typedef enum {
    VOICE_DIALOG_IDLE,           /*!< 空闲（未启动） */
    VOICE_DIALOG_LISTENING,      /*!< 监听唤醒词中 */
    VOICE_DIALOG_WAKEUP,         /*!< 已唤醒，等待用户说话 */
    VOICE_DIALOG_RECORDING,      /*!< 正在录制用户语音 */
    VOICE_DIALOG_RECOGNIZING,    /*!< 正在语音识别 */
    VOICE_DIALOG_THINKING,       /*!< AI正在思考 */
    VOICE_DIALOG_SPEAKING,       /*!< AI正在说话（TTS） */
    VOICE_DIALOG_ERROR,          /*!< 错误状态 */
} voice_dialog_state_t;

/**
 * @brief 语音对话事件类型
 */
typedef enum {
    VOICE_DIALOG_EVENT_WAKEUP,           /*!< 唤醒事件 */
    VOICE_DIALOG_EVENT_SESSION_END,      /*!< 连续对话结束，返回唤醒词监听 */
    VOICE_DIALOG_EVENT_RECORD_START,     /*!< 开始录音 */
    VOICE_DIALOG_EVENT_RECORD_END,       /*!< 录音结束 */
    VOICE_DIALOG_EVENT_ASR_RESULT,       /*!< ASR识别结果 */
    VOICE_DIALOG_EVENT_AI_RESPONSE,      /*!< AI回复 */
    VOICE_DIALOG_EVENT_TTS_START,        /*!< TTS开始 */
    VOICE_DIALOG_EVENT_TTS_END,          /*!< TTS结束 */
    VOICE_DIALOG_EVENT_ERROR,            /*!< 错误事件 */
} voice_dialog_event_t;

/**
 * @brief 语音对话配置
 */
typedef struct {
    // 唤醒词配置
    const char *wakeup_word;             /*!< 唤醒词，例如"你好小智"或"小智" */
    
    // 麦克风配置
    int mic_data_pin;                    /*!< 麦克风DATA引脚 */
    int mic_clk_pin;                     /*!< 麦克风CLK引脚 */
    uint32_t sample_rate;                /*!< 采样率（16000） */
    
    // VAD配置
    uint16_t vad_threshold;              /*!< VAD能量阈值 */
    uint32_t wakeup_timeout_ms;          /*!< 唤醒词检测超时(毫秒) */
    uint32_t record_timeout_ms;          /*!< 录音最大时长(毫秒) */
    uint32_t silence_timeout_ms;         /*!< 静音超时(毫秒)，用于检测用户说完 */
    
    // 百度语音配置
    const char *baidu_api_key;           /*!< 百度API Key */
    const char *baidu_secret_key;        /*!< 百度Secret Key */
    
    // 外部句柄引用（由调用者管理生命周期）
    spark_chat_client_t *spark_client;   /*!< Spark对话客户端 */
    baidu_tts_handle_t *tts_handle;      /*!< TTS句柄 */
    max98357a_handle_t **audio_handle;   /*!< 音频输出句柄指针的指针（用于动态重建） */
    
    // 音频输出配置（句柄为空时用于初始化）
    int audio_bclk_pin;                  /*!< 音频BCLK引脚 */
    int audio_lrclk_pin;                 /*!< 音频LRCLK引脚 */
    int audio_din_pin;                   /*!< 音频DIN引脚 */
    int audio_sd_mode_pin;               /*!< 音频SD_MODE引脚（可为NC） */
} voice_dialog_config_t;

/**
 * @brief 语音对话事件回调
 * 
 * @param event 事件类型
 * @param data 事件数据（根据事件类型解释）
 * @param user_ctx 用户上下文
 */
typedef void (*voice_dialog_event_cb_t)(voice_dialog_event_t event, 
                                        const char *data, 
                                        void *user_ctx);

/**
 * @brief 语音对话句柄
 */
typedef struct voice_dialog_handle *voice_dialog_handle_t;

/**
 * @brief 初始化语音对话模块
 * 
 * @param config 配置参数
 * @param handle 返回的句柄
 * @return 
 *     - ESP_OK: 成功
 *     - ESP_ERR_INVALID_ARG: 参数错误
 *     - ESP_ERR_NO_MEM: 内存不足
 */
esp_err_t voice_dialog_init(const voice_dialog_config_t *config,
                            voice_dialog_handle_t *handle);

/**
 * @brief 设置事件回调
 * 
 * @param handle 句柄
 * @param callback 回调函数
 * @param user_ctx 用户上下文
 * @return ESP_OK: 成功
 */
esp_err_t voice_dialog_set_callback(voice_dialog_handle_t handle,
                                    voice_dialog_event_cb_t callback,
                                    void *user_ctx);

/**
 * @brief 启动语音对话（开始监听唤醒词）
 * 
 * @param handle 句柄
 * @return ESP_OK: 成功
 */
esp_err_t voice_dialog_start(voice_dialog_handle_t handle);

/**
 * @brief 停止语音对话
 * 
 * @param handle 句柄
 * @return ESP_OK: 成功
 */
esp_err_t voice_dialog_stop(voice_dialog_handle_t handle);

/**
 * @brief 手动触发唤醒（跳过唤醒词检测）
 * 
 * @param handle 句柄
 * @return ESP_OK: 成功
 */
esp_err_t voice_dialog_trigger(voice_dialog_handle_t handle);

/**
 * @brief 获取当前状态
 * 
 * @param handle 句柄
 * @return 当前状态
 */
voice_dialog_state_t voice_dialog_get_state(voice_dialog_handle_t handle);

/**
 * @brief 释放资源
 * 
 * @param handle 句柄
 */
void voice_dialog_deinit(voice_dialog_handle_t handle);

/**
 * @brief 获取默认配置
 * 
 * @return 默认配置
 */
voice_dialog_config_t voice_dialog_get_default_config(void);

/**
 * @brief 设置音乐播放状态
 * 
 * 当检测到音乐播放命令时，设置此标志防止释放音频输出
 * 
 * @param handle 句柄
 * @param playing 是否正在播放音乐
 */
esp_err_t voice_dialog_set_music_playing(voice_dialog_handle_t handle, bool playing);

/** @brief 取消当前录音/对话，返回等待下一次唤醒。 */
esp_err_t voice_dialog_cancel_current_input(voice_dialog_handle_t handle);

/**
 * @brief 设置跳过AI回复的TTS播放
 * 
 * 当回调中已经处理了特定意图（如天气查询）并播放了TTS时，
 * 调用此函数跳过AI回复的TTS播放，避免重复播放
 * 
 * @param handle 句柄
 * @param skip 是否跳过
 */
void voice_dialog_set_skip_tts(voice_dialog_handle_t handle, bool skip);

#ifdef __cplusplus
}
#endif

#endif /* VOICE_DIALOG_H */
