/**
 * @file voice_dialog.cpp
 * @brief 语音对话管理模块实现
 * 
 * 实现"你好小智"唤醒词检测，语音录制，ASR识别，AI对话和TTS播放的完整流程。
 * 
 * 资源管理策略：
 * 1. PDM麦克风使用I2S0，喇叭使用I2S1
 * 2. 播报时另起监听任务，在确认唤醒词后停止播报
 * 3. 音频输出句柄由调用者管理，缺失时按需初始化
 * 4. 大缓冲区优先使用PSRAM
 */

#include "voice_dialog.h"
#include "baidu_asr.h"
#include "spark_chat.h"
#include "voice_local_intent.h"
#include "voice_barge_in.h"
#include "MSM261D4030H1CPM_Driver_V5.hpp"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "esp_log.h"
#include "esp_timer.h"
#include "esp_heap_caps.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"

static const char *TAG = "VoiceDialog";

// ==================== 音频参数配置 ====================
#define SAMPLE_RATE             16000       // 采样率16kHz
#define BUFFER_SIZE             2048        // 每次读取的字节数
#define SAMPLE_COUNT            (BUFFER_SIZE / 2)  // 样本数(16bit)

// 录音缓冲区：最大10秒 @ 16kHz = 320000字节
#define MAX_RECORD_SAMPLES      (SAMPLE_RATE * 10)
#define MAX_RECORD_BYTES        (MAX_RECORD_SAMPLES * 2)

// 唤醒词检测缓冲区：3秒 @ 16kHz
#define WAKEUP_AUDIO_SAMPLES    (SAMPLE_RATE * 3)

// VAD参数
#define VAD_DEFAULT_THRESHOLD   1200        // 安静桌面环境下提高远距收音灵敏度
#define SILENCE_DEFAULT_MS      1500        // 默认静音超时
#define RECORD_DEFAULT_MS       10000       // 默认最大录音时长
#define WAKEUP_DEFAULT_MS       3000        // 默认唤醒检测超时
#define DIALOG_IDLE_TIMEOUT_MS  30000       // 唤醒后无语音输入则回到等待唤醒词
#define MIN_SPEECH_DURATION_MS  800         // 最小有效语音时长（毫秒）
#define RECORD_MIN_SPEECH_MS    300         // 对话中的短句也应在静音后及时送去识别

// 音频增益
#define AUDIO_GAIN              5.0f        // 软件增益；硬件 PDM 增益见麦克风驱动
                                            // 说明：apply_gain() 内部有削顶保护（夹在±32767），不会爆音；
                                            // 阈值仍需结合设备静音 RMS 校准。

// ==================== 内部结构定义 ====================

struct voice_dialog_handle {
    voice_dialog_config_t config;
    
    // 百度ASR
    baidu_asr_handle_t asr;
    
    // 状态管理
    voice_dialog_state_t state;
    bool running;
    bool triggered;     // 手动触发标志
    bool music_playing; // 音乐播放标志，防止释放音频输出
    bool skip_tts;      // 跳过AI回复的TTS播放（回调中已处理）
    MSM261D4030H1CPM::MicDriver *active_mic;
    bool barge_requested;
    int64_t spark_retry_after_us;
    
    // 回调
    voice_dialog_event_cb_t callback;
    void *user_ctx;
    
    // 任务句柄
    TaskHandle_t task_handle;
    
    // 音频缓冲区（优先使用PSRAM）
    int16_t *audio_buffer;
    size_t audio_buffer_size;   // 样本数
    size_t audio_pos;           // 当前位置
    
    // 临时读取缓冲区
    int16_t *chunk_buffer;
    
    // 唤醒词检测缓冲区
    int16_t *wakeup_buffer;
    size_t wakeup_pos;
    
    // ASR结果
    char asr_result[512];
    
    // 内部TTS（当外部未提供时自行创建）
    baidu_tts_handle_t *internal_tts;
    bool own_tts;  // 是否拥有TTS handle（需要自行释放）
    
    // 互斥锁
    SemaphoreHandle_t mutex;
};

extern "C" {

// ==================== 辅助函数 ====================

/**
 * @brief 计算音频RMS能量
 */
static float calculate_rms(const int16_t* data, size_t len)
{
    if (len == 0) return 0.0f;
    
    double sum = 0.0;
    for (size_t i = 0; i < len; i++) {
        double sample = (double)data[i];
        sum += sample * sample;
    }
    
    return (float)sqrt(sum / len);
}

/**
 * @brief 应用音频增益
 */
static void apply_gain(int16_t* data, size_t len, float gain)
{
    for (size_t i = 0; i < len; i++) {
        int32_t sample = (int32_t)(data[i] * gain);
        if (sample > 32767) sample = 32767;
        if (sample < -32768) sample = -32768;
        data[i] = (int16_t)sample;
    }
}

/**
 * @brief 去除直流偏置（DC offset）
 *
 * PDM 麦克风经硬件 PDM2PCM 转换后常带有固定直流残余（本机实测约 1640）。
 * 若不去除，RMS 会被直流垫高到长期高于 VAD 阈值，程序就会一直认为"有人在说话"，
 * 于是静音计时永远为 0，永远等不到"停顿若干毫秒后再送识别"的条件 —— 表现为
 * "怎么喊都不响应"。这里用整块均值近似直流并减掉。
 */
static void remove_dc_offset(int16_t *data, size_t len)
{
    if (len == 0) {
        return;
    }

    int32_t sum = 0;
    for (size_t i = 0; i < len; i++) {
        sum += data[i];
    }
    int32_t mean = sum / (int32_t)len;

    for (size_t i = 0; i < len; i++) {
        int32_t v = (int32_t)data[i] - mean;
        if (v > 32767) v = 32767;
        if (v < -32768) v = -32768;
        data[i] = (int16_t)v;
    }
}

/**
 * @brief 简单降噪滤波（3点移动平均）
 */
static void apply_noise_reduction(int16_t* data, size_t len)
{
    if (len < 3) return;
    
    int16_t prev = data[0];
    int16_t curr = data[1];
    
    for (size_t i = 1; i < len - 1; i++) {
        int16_t next = data[i + 1];
        int32_t avg = ((int32_t)prev + (int32_t)curr + (int32_t)next) / 3;
        prev = curr;
        curr = next;
        data[i] = (int16_t)avg;
    }
}

/**
 * @brief 检查唤醒词是否匹配
 * 支持: "小智小智", "你好小智", "您好小智", "嗨小智" 等变体
 */
static bool check_wakeup_word(const char *text, const char *wakeup_word)
{
    if (text == NULL || wakeup_word == NULL) {
        return false;
    }
    
    // 检查是否包含唤醒词
    if (strstr(text, wakeup_word) != NULL) {
        return true;
    }
    
    // 检查 "小智小智" (重复两次)
    const char *first = strstr(text, "小智");
    if (first != NULL) {
        const char *second = strstr(first + 4, "小智");  // UTF-8中"小智"占4字节
        if (second != NULL) {
            return true;  // 找到两次"小智"
        }
    }
    
    // 检查 "你好小智" 及变体
    if (strstr(text, "小智") != NULL) {
        if (strstr(text, "你好") != NULL || 
            strstr(text, "您好") != NULL ||
            strstr(text, "嗨") != NULL ||
            strstr(text, "喂") != NULL ||
            strstr(text, "hi") != NULL ||
            strstr(text, "Hi") != NULL ||
            strstr(text, "hello") != NULL ||
            strstr(text, "Hello") != NULL) {
            return true;
        }
    }
    
    // 兼容语音识别的常见变体
    // "小智" 可能被识别为 "小知"、"小志"、"小之" 等
    const char *variants[] = {
        "小知", "小志", "小之", "小枝", "小芝",
        "晓智", "晓知", "晓志",
        NULL
    };
    
    for (int i = 0; variants[i] != NULL; i++) {
        if (strstr(text, variants[i]) != NULL) {
            // 检查是否重复两次或有问候语
            const char *v_first = strstr(text, variants[i]);
            if (v_first != NULL) {
                const char *v_second = strstr(v_first + 4, variants[i]);
                if (v_second != NULL) {
                    return true;  // 重复两次
                }
            }
            // 有问候语
            if (strstr(text, "你好") != NULL || 
                strstr(text, "您好") != NULL ||
                strstr(text, "嗨") != NULL) {
                return true;
            }
        }
    }
    
    return false;
}

/**
 * @brief 确保音频输出已初始化
 */
static esp_err_t ensure_audio_output(voice_dialog_handle_t h)
{
    if (h->config.audio_handle == NULL) {
        return ESP_ERR_INVALID_STATE;
    }

    // 喇叭独占I2S1，录音不会占用它；保留已有句柄供闹钟、音乐和TTS使用。
    if (*(h->config.audio_handle) != NULL) {
        if (h->config.tts_handle != NULL) {
            h->config.tts_handle->audio_handle = *(h->config.audio_handle);
        }
        return ESP_OK;
    }

    ESP_LOGI(TAG, "初始化音频输出...");
    
    max98357a_config_t audio_config = max98357a_get_default_config();
    audio_config.i2s_port = I2S_NUM_1;
    audio_config.bclk_pin = (gpio_num_t)h->config.audio_bclk_pin;
    audio_config.lrclk_pin = (gpio_num_t)h->config.audio_lrclk_pin;
    audio_config.din_pin = (gpio_num_t)h->config.audio_din_pin;
    audio_config.sd_mode_pin = (gpio_num_t)h->config.audio_sd_mode_pin;
    audio_config.sample_rate = 16000;
    audio_config.bits_per_sample = I2S_DATA_BIT_WIDTH_16BIT;
    audio_config.gain = MAX98357A_GAIN_9DB;
    audio_config.channel = MAX98357A_CHANNEL_LEFT;
    
    esp_err_t ret = max98357a_init(&audio_config, h->config.audio_handle);
    if (ret == ESP_OK) {
        ESP_LOGI(TAG, "音频输出初始化成功");
        
        // 更新TTS句柄的音频输出引用
        if (h->config.tts_handle != NULL && *(h->config.audio_handle) != NULL) {
            h->config.tts_handle->audio_handle = *(h->config.audio_handle);
        }
    } else {
        ESP_LOGE(TAG, "音频输出初始化失败: %s", esp_err_to_name(ret));
    }
    
    return ret;
}

/**
 * @brief 过滤命令标签
 * 移除 [PLAY_MUSIC:xxx], [STOP_MUSIC], [WEATHER_QUERY], [QUERY_WEATHER] 等标签
 * @param text 原始文本
 * @param filtered 过滤后的文本缓冲区
 * @param max_len 缓冲区最大长度
 * @return 过滤后的文本长度
 */
static size_t filter_command_tags(const char *text, char *filtered, size_t max_len)
{
    if (text == NULL || filtered == NULL || max_len == 0) {
        return 0;
    }
    
    size_t src_pos = 0;
    size_t dst_pos = 0;
    size_t text_len = strlen(text);
    
    while (src_pos < text_len && dst_pos < max_len - 1) {
        // 检测到 '[' 开始的标签
        if (text[src_pos] == '[') {
            // 查找匹配的 ']'
            const char *end = strchr(text + src_pos, ']');
            if (end != NULL) {
                // 检查是否是命令标签
                size_t tag_len = end - (text + src_pos) + 1;
                if (strncmp(text + src_pos, "[PLAY_MUSIC:", 12) == 0 ||
                    strncmp(text + src_pos, "[STOP_MUSIC]", 12) == 0 ||
                    strncmp(text + src_pos, "[WEATHER_QUERY]", 15) == 0 ||
                    strncmp(text + src_pos, "[QUERY_WEATHER]", 15) == 0 ||
                    strncmp(text + src_pos, "[GET_WEATHER", 12) == 0 ||  // 匹配 [GET_WEATHER] 或 [GET_WEATHER:xxx]
                    strncmp(text + src_pos, "[TIME_QUERY]", 12) == 0 ||
                    strncmp(text + src_pos, "[DATE_QUERY]", 12) == 0) {
                    // 跳过整个标签
                    src_pos += tag_len;
                    continue;
                }
            }
        }
        
        // 复制普通字符
        filtered[dst_pos++] = text[src_pos++];
    }
    
    filtered[dst_pos] = '\0';
    
    // 去除首尾空白
    while (dst_pos > 0 && (filtered[dst_pos - 1] == ' ' || filtered[dst_pos - 1] == '\n')) {
        filtered[--dst_pos] = '\0';
    }
    
    char *start = filtered;
    while (*start == ' ' || *start == '\n') {
        start++;
    }
    
    if (start != filtered) {
        size_t new_len = strlen(start);
        memmove(filtered, start, new_len + 1);
        dst_pos = new_len;
    }
    
    return dst_pos;
}

/**
 * @brief 播放TTS回复
 */
static void play_tts_response(voice_dialog_handle_t h, const char *text)
{
    if (h->config.tts_handle == NULL || text == NULL || strlen(text) == 0) {
        return;
    }
    
    // 音乐和TTS共用喇叭，避免两个播放任务同时写入。
    if (h->music_playing) {
        ESP_LOGI(TAG, "音乐正在播放，跳过TTS");
        return;
    }
    
    // 过滤命令标签
    char filtered_text[512];
    size_t filtered_len = filter_command_tags(text, filtered_text, sizeof(filtered_text));
    
    // 如果过滤后为空，跳过TTS
    if (filtered_len == 0) {
        ESP_LOGI(TAG, "过滤后文本为空，跳过TTS");
        return;
    }
    
    h->state = VOICE_DIALOG_SPEAKING;
    if (h->callback) {
        h->callback(VOICE_DIALOG_EVENT_TTS_START, filtered_text, h->user_ctx);
    }
    
    ESP_LOGI(TAG, "开始TTS播放: %s", filtered_text);
    
    // I2S0 麦克风在播报期间保持开启，独立任务监听“小智”并可停止 I2S1 播报。
    baidu_tts_reset_cancel(h->config.tts_handle);
    voice_barge_in *monitor = voice_barge_in_start(h->active_mic, &h->asr,
                                                    h->config.tts_handle,
                                                    h->config.wakeup_word,
                                                    h->config.vad_threshold);
    esp_err_t speak_err = baidu_tts_speak(h->config.tts_handle, filtered_text);
    if (voice_barge_in_stop(monitor)) {
        h->barge_requested = true;
        ESP_LOGI(TAG, "播报已被唤醒词打断，准备接收新问题");
    } else if (speak_err != ESP_OK && !h->barge_requested) {
        ESP_LOGW(TAG, "TTS 播报失败: %s", esp_err_to_name(speak_err));
    }
    baidu_tts_reset_cancel(h->config.tts_handle);
    
    if (h->callback) {
        h->callback(VOICE_DIALOG_EVENT_TTS_END, NULL, h->user_ctx);
    }
    
    ESP_LOGI(TAG, "TTS播放完成");
}

/**
 * @brief 本地意图快速回答（不走大模型）
 *
 * 天气这类本机已经拿到数据的问题，直接本地组织回答并播报：
 * 响应快、内容准，也不消耗大模型的额度。
 *
 * @param h 语音对话句柄
 * @param user_text ASR识别出的用户原话
 * @return true 已本地处理并播报（调用方不要再发给大模型）
 */
static bool try_local_intent(voice_dialog_handle_t h, const char *user_text)
{
    if (user_text == NULL || user_text[0] == '\0') {
        return false;
    }

    // 本地规则优先：天气/报时/音量/亮度/闹钟/切界面这些本机就能完成，
    // 命中就直接执行并播报，不必等大模型（更快、不消耗额度）。
    // 具体规则都在 voice_local_intent.c 里，这里只负责播报。
    char reply[256];
    if (!voice_local_intent_try_handle(user_text, reply, sizeof(reply))) {
        return false;   // 未命中，交给大模型
    }

    ESP_LOGI(TAG, "本地意图回答: %s", reply);
    printf("小智: %s\n", reply);
    fflush(stdout);

    if (h->callback) {
        h->callback(VOICE_DIALOG_EVENT_AI_RESPONSE, reply, h->user_ctx);
    }
    play_tts_response(h, reply);
    return true;
}

/**
 * @brief 发送文本到Spark大模型并获取回复
 */
static bool send_to_spark_and_respond(voice_dialog_handle_t h, const char *user_text)
{
    if (h->config.spark_client == NULL) {
        ESP_LOGW(TAG, "Spark客户端未配置");
        return false;
    }
    if (esp_timer_get_time() < h->spark_retry_after_us) {
        ESP_LOGW(TAG, "星火接口处于 429 冷却期，暂不重复请求");
        const char *message = "星火服务请求太频繁，请稍后再试。";
        if (h->callback) h->callback(VOICE_DIALOG_EVENT_AI_RESPONSE, message, h->user_ctx);
        play_tts_response(h, message);
        return false;
    }
    
    h->state = VOICE_DIALOG_THINKING;
    
    ESP_LOGI(TAG, "发送到Spark: %s", user_text);
    printf("\n用户(语音): %s\n", user_text);
    fflush(stdout);
    
    // 清除之前的对话历史，只保留system prompt（第一条消息）
    // 这样可以避免历史对话影响当前回复
    spark_chat_client_t *client = h->config.spark_client;
    if (client->history_len > 1) {
        // 保存system prompt
        spark_chat_message_t system_msg;
        memcpy(&system_msg, &client->history[0], sizeof(spark_chat_message_t));
        
        // 清除所有历史
        spark_chat_clear_history(client);
        
        // 恢复system prompt
        spark_chat_add_message(client, system_msg.role, system_msg.content);
    }
    
    // 添加用户消息
    spark_chat_add_message(client, "user", user_text);
    
    printf("星火: ");
    fflush(stdout);
    
    // 发送请求
    if (spark_chat_request(client)) {
        const char *response = spark_chat_get_last_response(client);
        
        printf("\n");
        
        // 关闭Spark连接释放TLS内存，供TTS使用
        spark_chat_close_connection(client);
        vTaskDelay(pdMS_TO_TICKS(100));
        
        // 重置skip_tts标志（回调中可能会设置）
        h->skip_tts = false;
        
        // 回调必须在Spark连接关闭后调用，否则回调中的TTS会因内存不足失败
        if (h->callback) {
            h->callback(VOICE_DIALOG_EVENT_AI_RESPONSE, response, h->user_ctx);
        }
        
        // 播放TTS回复（如果回调中没有设置跳过标志）
        if (!h->skip_tts && response != NULL && strlen(response) > 0) {
            play_tts_response(h, response);
        } else if (h->skip_tts) {
            ESP_LOGI(TAG, "跳过AI回复的TTS播放（回调中已处理）");
        }
        
        return true;
    } else {
        printf("获取回复失败\n");
        ESP_LOGE(TAG, "Spark请求失败");
        if (client->last_http_status == 429) {
            h->spark_retry_after_us = esp_timer_get_time() + 60LL * 1000000LL;
            const char *message = "星火服务请求太频繁，请一分钟后再试。";
            if (h->callback) h->callback(VOICE_DIALOG_EVENT_AI_RESPONSE, message, h->user_ctx);
            play_tts_response(h, message);
        }
        spark_chat_close_connection(client);
        return false;
    }
}

/**
 * @brief 执行语音识别
 */
static bool do_asr_recognize(voice_dialog_handle_t h, int16_t *audio, size_t samples)
{
    if (samples < SAMPLE_RATE / 2) {
        ESP_LOGW(TAG, "录音太短(%u样本)，跳过识别", samples);
        return false;
    }
    
    // 最后检查：计算整体RMS，确保不是噪音
    float total_rms = calculate_rms(audio, samples);
    if (total_rms < h->config.vad_threshold * 0.6f) {
        ESP_LOGW(TAG, "整体RMS太低 %.0f (阈值: %d)，跳过识别（可能是噪音）", 
                 total_rms, h->config.vad_threshold);
        return false;
    }
    
    // 检查动态范围
    int16_t min_val = 32767, max_val = -32768;
    for (size_t i = 0; i < samples; i++) {
        if (audio[i] < min_val) min_val = audio[i];
        if (audio[i] > max_val) max_val = audio[i];
    }
    int dynamic_range = max_val - min_val;
    if (dynamic_range < 1500) {
        ESP_LOGW(TAG, "动态范围太小 %d，跳过识别（可能是噪音）", dynamic_range);
        return false;
    }
    
    h->state = VOICE_DIALOG_RECOGNIZING;
    
    if (h->callback) {
        h->callback(VOICE_DIALOG_EVENT_RECORD_END, NULL, h->user_ctx);
    }
    
    ESP_LOGI(TAG, "开始语音识别，音频长度: %u样本 (%.1fs)", 
             samples, (float)samples / SAMPLE_RATE);
    
    memset(h->asr_result, 0, sizeof(h->asr_result));
    
    esp_err_t ret = baidu_asr_recognize(&h->asr, audio, samples * sizeof(int16_t),
                                         h->asr_result, sizeof(h->asr_result));
    
    if (ret == ESP_OK && strlen(h->asr_result) > 0) {
        ESP_LOGI(TAG, "识别结果: %s", h->asr_result);
        
        if (h->callback) {
            h->callback(VOICE_DIALOG_EVENT_ASR_RESULT, h->asr_result, h->user_ctx);
        }
        
        return true;
    } else {
        ESP_LOGW(TAG, "识别失败或结果为空");
        return false;
    }
}

// ==================== 主任务 ====================

/**
 * @brief 语音对话主任务
 * 
 * 工作流程：
 * 1. 初始化麦克风
 * 2. 持续监听，检测唤醒词
 * 3. 检测到唤醒词或手动触发后，录制用户语音
 * 4. 停止麦克风，进行ASR识别
 * 5. 将识别结果发送给AI，获取回复
 * 6. 重新初始化音频输出，播放TTS
 * 7. 释放音频输出，回到步骤1
 */
static void voice_dialog_task(void *arg)
{
    voice_dialog_handle_t h = (voice_dialog_handle_t)arg;
    
    ESP_LOGI(TAG, "语音对话任务启动");
    ESP_LOGI(TAG, "唤醒词: %s", h->config.wakeup_word);
    
    // 创建麦克风驱动实例
    MSM261D4030H1CPM::MicDriver mic;
    MSM261D4030H1CPM::mic_config_t mic_cfg = MSM261D4030H1CPM::get_default_config();
    mic_cfg.pin_clk = (gpio_num_t)h->config.mic_clk_pin;
    mic_cfg.pin_data = (gpio_num_t)h->config.mic_data_pin;
    mic_cfg.sample_rate = h->config.sample_rate;
    mic_cfg.channel = MSM261D4030H1CPM::MIC_CHANNEL_LEFT;
    mic_cfg.mode = MSM261D4030H1CPM::MIC_MODE_STANDARD_PERFORMANCE;
    mic_cfg.i2s_port = I2S_NUM_0;  // ESP32-S3的PDM接收只能使用I2S0
    
    char wakeup_asr_result[256];
    
    while (h->running) {
        // ============ 阶段0: 等待音乐播放结束 ============
        // 如果音乐正在播放，等待直到播放结束（最多等待5分钟）
        if (h->music_playing) {
            int wait_count = 0;
            const int MAX_WAIT_COUNT = 150;  // 150 * 2秒 = 5分钟
            
            ESP_LOGI(TAG, "音乐正在播放，暂停语音对话...");
            printf("\n[语音助手] 音乐播放中，暂停语音对话。\n");
            fflush(stdout);
            
            while (h->running && h->music_playing && wait_count < MAX_WAIT_COUNT) {
                // 每次循环都打印日志，方便调试
                ESP_LOGI(TAG, "等待音乐播放结束... (已等待%d秒, music_playing=%d)", 
                         wait_count * 2, h->music_playing);
                vTaskDelay(pdMS_TO_TICKS(2000));
                wait_count++;
            }
            
            // 如果超时，强制重置音乐播放标志
            if (wait_count >= MAX_WAIT_COUNT) {
                ESP_LOGW(TAG, "等待音乐播放超时，强制重置");
                h->music_playing = false;
            }
            
            // 音乐播放结束
            if (!h->music_playing) {
                ESP_LOGI(TAG, "音乐播放结束，恢复语音对话");
                printf("\n[语音助手] 音乐播放结束，恢复语音对话。\n");
                fflush(stdout);
                
                // 等待一下让音频资源完全释放
                vTaskDelay(pdMS_TO_TICKS(500));
            }
            
            if (!h->running) break;
        }
        
        // ============ 阶段1: 初始化麦克风 ============
        ESP_LOGI(TAG, "初始化麦克风...");
        if (!mic.is_initialized() && (mic.config(mic_cfg) != ESP_OK || mic.begin() != ESP_OK)) {
            ESP_LOGE(TAG, "麦克风初始化失败");
            h->state = VOICE_DIALOG_ERROR;
            vTaskDelay(pdMS_TO_TICKS(1000));
            continue;
        }
        h->active_mic = &mic;
        
        // 预读取几帧清空缓冲区
        for (int i = 0; i < 5; i++) {
            size_t bytes_read = 0;
            mic.read(h->chunk_buffer, BUFFER_SIZE, &bytes_read, 100);
            vTaskDelay(pdMS_TO_TICKS(10));
        }
        
        // ============ 阶段2: 监听唤醒词 ============
        h->state = VOICE_DIALOG_LISTENING;
        h->wakeup_pos = 0;
        int64_t last_voice_time = 0;
        int64_t speech_start_time = 0;
        int64_t total_speech_duration = 0;
        int64_t last_debug_time = 0;
        bool wakeup_detected = false;
        
        ESP_LOGI(TAG, "开始监听唤醒词...");
        printf("\n[语音助手] 正在监听，请说\"小智小智\"或\"你好小智\"...\n");
        fflush(stdout);
        
        while (h->running && !wakeup_detected) {
            // 检查手动触发
            if (h->triggered) {
                h->triggered = false;
                ESP_LOGI(TAG, "手动触发唤醒");
                wakeup_detected = true;
                break;
            }
            
            size_t bytes_read = 0;
            esp_err_t err = mic.read(h->chunk_buffer, BUFFER_SIZE, &bytes_read, 100);
            
            if (err != ESP_OK || bytes_read == 0) {
                vTaskDelay(pdMS_TO_TICKS(10));
                continue;
            }
            
            size_t samples = bytes_read / sizeof(int16_t);
            float rms_before = calculate_rms(h->chunk_buffer, samples);
            int64_t now = esp_timer_get_time();
            
            // 首次读取时诊断原始数据
            static bool first_read = true;
            static int read_count = 0;
            if (first_read && samples >= 10) {
                ESP_LOGI(TAG, "麦克风原始数据前10样本: %d, %d, %d, %d, %d, %d, %d, %d, %d, %d",
                         h->chunk_buffer[0], h->chunk_buffer[1], h->chunk_buffer[2],
                         h->chunk_buffer[3], h->chunk_buffer[4], h->chunk_buffer[5],
                         h->chunk_buffer[6], h->chunk_buffer[7], h->chunk_buffer[8],
                         h->chunk_buffer[9]);
                
                // 检查是否全为同一值
                bool all_same = true;
                for (size_t i = 1; i < samples; i++) {
                    if (h->chunk_buffer[i] != h->chunk_buffer[0]) {
                        all_same = false;
                        break;
                    }
                }
                if (all_same) {
                    ESP_LOGW(TAG, "警告：麦克风数据全为相同值 %d！", h->chunk_buffer[0]);
                } else {
                    first_read = false;
                    ESP_LOGI(TAG, "麦克风数据正常，原始RMS: %.0f", rms_before);
                }
            }
            
            // 每100次读取打印一次RMS（调试用）
            if (++read_count % 100 == 0) {
                ESP_LOGD(TAG, "原始RMS: %.0f (读取%d次)", rms_before, read_count);
            }
            
            // 应用降噪和增益
            // 处理顺序：先去直流（否则直流会被一起放大）→ 再降噪 → 最后放大
            remove_dc_offset(h->chunk_buffer, samples);
            apply_noise_reduction(h->chunk_buffer, samples);
            apply_gain(h->chunk_buffer, samples, AUDIO_GAIN);
            float rms_after = calculate_rms(h->chunk_buffer, samples);
            
            // 检测到声音（高于阈值才累积）
            if (rms_after > h->config.vad_threshold) {
                last_voice_time = now;
                
                // 累积到唤醒词缓冲区
                size_t space = WAKEUP_AUDIO_SAMPLES - h->wakeup_pos;
                size_t to_copy = (samples < space) ? samples : space;
                if (to_copy > 0) {
                    memcpy(h->wakeup_buffer + h->wakeup_pos, h->chunk_buffer, 
                           to_copy * sizeof(int16_t));
                    h->wakeup_pos += to_copy;
                }
            } else if (h->wakeup_pos > 0) {
                // RMS低于阈值但缓冲区有数据，也累积（捕获完整语音）
                size_t space = WAKEUP_AUDIO_SAMPLES - h->wakeup_pos;
                size_t to_copy = (samples < space) ? samples : space;
                if (to_copy > 0) {
                    memcpy(h->wakeup_buffer + h->wakeup_pos, h->chunk_buffer, 
                           to_copy * sizeof(int16_t));
                    h->wakeup_pos += to_copy;
                }
            }
            
            // 检查是否收集了足够的音频进行唤醒词检测
            int64_t silence_duration = (now - last_voice_time) / 1000;  // ms
            
            // 计算累积的有效语音时长（RMS高于阈值的时长）
            if (rms_after > h->config.vad_threshold) {
                if (speech_start_time == 0) {
                    speech_start_time = now;
                }
            } else if (speech_start_time > 0 && silence_duration > 300) {
                // 累积语音时长
                total_speech_duration += (last_voice_time - speech_start_time) / 1000;
                speech_start_time = 0;
            }
            
            // 调试：每秒打印一次状态
            if (now - last_debug_time > 1000000) {  // 1秒
                last_debug_time = now;
                ESP_LOGI(TAG, "唤醒监听: 缓冲=%u样本, RMS=%.0f, 静音=%lldms, 语音时长=%lldms, 阈值=%d",
                         h->wakeup_pos, rms_after, silence_duration, total_speech_duration, h->config.vad_threshold);
            }
            
            // 要求：至少1.2秒音频，且有一段静音（说完了），且累积语音时长足够
            if (h->wakeup_pos >= SAMPLE_RATE * 6 / 5 &&  // 至少1.2秒（19200样本）
                total_speech_duration >= MIN_SPEECH_DURATION_MS &&  // 至少800ms有效语音
                silence_duration >= 500 && silence_duration < 3000) {  // 停顿500ms-3秒后识别
                
                // 计算平均RMS
                float avg_rms = 0.0f;
                int rms_samples = h->wakeup_pos / 10;
                for (int i = 0; i < 10 && i * rms_samples < (int)h->wakeup_pos; i++) {
                    float seg_rms = calculate_rms(h->wakeup_buffer + i * rms_samples, rms_samples);
                    avg_rms += seg_rms;
                }
                avg_rms /= 10.0f;
                
                // 检查样本值范围
                int16_t min_val = 32767, max_val = -32768;
                for (size_t i = 0; i < h->wakeup_pos; i++) {
                    if (h->wakeup_buffer[i] < min_val) min_val = h->wakeup_buffer[i];
                    if (h->wakeup_buffer[i] > max_val) max_val = h->wakeup_buffer[i];
                }
                
                // 动态范围太小（小于2000），可能只是背景噪音
                int dynamic_range = max_val - min_val;
                if (dynamic_range < 2000) {
                    ESP_LOGW(TAG, "音频动态范围太小 %d，跳过识别（可能是噪音）", dynamic_range);
                    h->wakeup_pos = 0;
                    total_speech_duration = 0;
                    speech_start_time = 0;
                    continue;
                }
                
                // 检查平均RMS是否足够高
                if (avg_rms < h->config.vad_threshold * 0.8f) {
                    ESP_LOGW(TAG, "平均RMS太低 %.0f (阈值: %d)，跳过识别", 
                             avg_rms, h->config.vad_threshold);
                    h->wakeup_pos = 0;
                    total_speech_duration = 0;
                    speech_start_time = 0;
                    continue;
                }
                
                ESP_LOGI(TAG, "检测唤醒词，音频: %u样本, RMS: %.0f, 范围: [%d, %d], 动态: %d",
                         h->wakeup_pos, avg_rms, min_val, max_val, dynamic_range);
                
                // 打印前10个样本用于诊断
                ESP_LOGI(TAG, "音频样本前10个: %d, %d, %d, %d, %d, %d, %d, %d, %d, %d",
                         h->wakeup_buffer[0], h->wakeup_buffer[1], h->wakeup_buffer[2],
                         h->wakeup_buffer[3], h->wakeup_buffer[4], h->wakeup_buffer[5],
                         h->wakeup_buffer[6], h->wakeup_buffer[7], h->wakeup_buffer[8],
                         h->wakeup_buffer[9]);
                
                err = baidu_asr_recognize(&h->asr, h->wakeup_buffer, 
                                          h->wakeup_pos * sizeof(int16_t),
                                          wakeup_asr_result, sizeof(wakeup_asr_result));
                
                if (err == ESP_OK && strlen(wakeup_asr_result) > 0) {
                    ESP_LOGI(TAG, "唤醒检测ASR结果: %s", wakeup_asr_result);
                    printf("[唤醒检测] 听到: %s\n", wakeup_asr_result);
                    fflush(stdout);
                    
                    if (check_wakeup_word(wakeup_asr_result, h->config.wakeup_word)) {
                        ESP_LOGI(TAG, "*** 唤醒成功 ***");
                        printf("\n*** 唤醒成功！***\n");
                        fflush(stdout);
                        wakeup_detected = true;
                        
                        if (h->callback) {
                            h->callback(VOICE_DIALOG_EVENT_WAKEUP, h->config.wakeup_word, h->user_ctx);
                        }
                        
                        // 确保喇叭句柄可用
                        esp_err_t audio_err = ensure_audio_output(h);
                        if (audio_err == ESP_OK) {
                            // 播放唤醒回复
                            ESP_LOGI(TAG, "播放唤醒回复...");
                            const char* wakeup_response = "在，请说。";
                            printf("小智: %s\n", wakeup_response);
                            fflush(stdout);
                            play_tts_response(h, wakeup_response);
                            ESP_LOGI(TAG, "唤醒回复播放完成");
                            
                            // 播放接口返回时已经结束，直接开始接收用户问题。
                        } else {
                            ESP_LOGW(TAG, "音频输出初始化失败，跳过TTS");
                        }
                        
                        // 麦克风与喇叭使用独立 I2S，播报后继续沿用当前麦克风通道。
                    }
                } else if (err == ESP_OK) {
                    ESP_LOGD(TAG, "ASR识别为空，可能是噪音");
                }
                
                // 清空唤醒词缓冲区和语音时长计数器
                h->wakeup_pos = 0;
                total_speech_duration = 0;
                speech_start_time = 0;
            }
            
            // 如果缓冲区满了但没有持续语音活动，也清空（避免死循环）
            if (h->wakeup_pos >= WAKEUP_AUDIO_SAMPLES - 1024) {
                if (silence_duration > 500 || total_speech_duration < MIN_SPEECH_DURATION_MS) {
                    ESP_LOGD(TAG, "缓冲区满但无足够有效语音（%lldms），清空", total_speech_duration);
                    h->wakeup_pos = 0;
                    total_speech_duration = 0;
                    speech_start_time = 0;
                }
            }
            
            // 静音超时清空缓冲区
            if (h->wakeup_pos > 0 && last_voice_time > 0 &&
                (now - last_voice_time) > (int64_t)h->config.wakeup_timeout_ms * 1000) {
                h->wakeup_pos = 0;
                total_speech_duration = 0;
                speech_start_time = 0;
            }
            
            vTaskDelay(pdMS_TO_TICKS(20));
        }
        
        if (!h->running) {
            mic.end();
            break;
        }
        
        // ============ 对话模式循环 ============
        // 唤醒后持续对话，空闲一段时间再返回唤醒词监听。
        int64_t dialog_start_time = esp_timer_get_time();
        const int64_t DIALOG_TIMEOUT_US = DIALOG_IDLE_TIMEOUT_MS * 1000LL;
        bool dialog_active = true;
        
        while (h->running && dialog_active) {
            // 检查对话模式超时
            int64_t dialog_elapsed = esp_timer_get_time() - dialog_start_time;
            if (dialog_elapsed > DIALOG_TIMEOUT_US) {
                ESP_LOGI(TAG, "对话模式空闲 %d 秒，返回唤醒词监听", DIALOG_IDLE_TIMEOUT_MS / 1000);
                printf("\n[语音助手] 暂无新问题，返回待机监听。\n");
                fflush(stdout);
                break;
            }
            
            // ============ 阶段3: 录制用户语音 ============
            h->state = VOICE_DIALOG_RECORDING;
            h->audio_pos = 0;
            int64_t record_start = esp_timer_get_time();
            last_voice_time = 0;  // 重置为0，等待检测到第一个语音
            bool has_speech = false;
            int64_t total_speech_ms = 0;
            uint8_t recent_voice = 0;
            
            if (h->callback) {
                h->callback(VOICE_DIALOG_EVENT_RECORD_START, NULL, h->user_ctx);
            }
            
            int remaining_sec = (int)((DIALOG_TIMEOUT_US - dialog_elapsed) / 1000000);
            ESP_LOGI(TAG, "开始录制用户语音（剩余%d秒）...", remaining_sec);
            printf("[录音中] 请说话（%d秒后返回待机）...\n", remaining_sec);
            fflush(stdout);
            
            while (h->running) {
                size_t bytes_read = 0;
                esp_err_t err = mic.read(h->chunk_buffer, BUFFER_SIZE, &bytes_read, 100);
                
                if (err != ESP_OK || bytes_read == 0) {
                    vTaskDelay(pdMS_TO_TICKS(10));
                    continue;
                }
                
                size_t samples = bytes_read / sizeof(int16_t);
                int64_t now = esp_timer_get_time();
                
                // 处理顺序：先去直流 → 再降噪 → 最后放大
                remove_dc_offset(h->chunk_buffer, samples);
                apply_noise_reduction(h->chunk_buffer, samples);
                apply_gain(h->chunk_buffer, samples, AUDIO_GAIN);
                float rms = calculate_rms(h->chunk_buffer, samples);
                bool above_threshold = rms > h->config.vad_threshold;
                recent_voice = (uint8_t)(((recent_voice << 1) | (above_threshold ? 1 : 0)) & 0x0F);
                bool confirmed_voice = __builtin_popcount((unsigned)recent_voice) >= 2;
                
                // 连续数帧内至少两帧超过阈值，避免单个噪声尖峰延长录音。
                if (confirmed_voice) {
                    if (!has_speech) {
                        has_speech = true;
                        ESP_LOGI(TAG, "检测到语音，RMS: %.0f", rms);
                    }
                    last_voice_time = now;
                    total_speech_ms += (int64_t)samples * 1000 / SAMPLE_RATE;
                }
                if (above_threshold || has_speech || h->audio_pos > 0) {
                    // 第一帧疑似语音先留在缓冲区，确认后不会丢句首。
                    size_t space = h->audio_buffer_size - h->audio_pos;
                    size_t to_copy = (samples < space) ? samples : space;
                    if (to_copy > 0) {
                        memcpy(h->audio_buffer + h->audio_pos, h->chunk_buffer, 
                               to_copy * sizeof(int16_t));
                        h->audio_pos += to_copy;
                    }
                }
                
                // 检查结束条件
                int64_t elapsed_ms = (now - record_start) / 1000;
                int64_t silence_ms = (last_voice_time > 0) ? (now - last_voice_time) / 1000 : 0;
            
            // 超时
            if (elapsed_ms > (int64_t)h->config.record_timeout_ms) {
                ESP_LOGI(TAG, "录音超时");
                break;
            }
            
            // 静音超时（需要先检测到语音，且有足够的语音时长）
            if (has_speech && total_speech_ms >= RECORD_MIN_SPEECH_MS &&
                silence_ms > (int64_t)h->config.silence_timeout_ms) {
                ESP_LOGI(TAG, "检测到静音，停止录音（语音时长: %lldms）", total_speech_ms);
                break;
            }
            
            // 如果没有检测到足够的语音，但已经等待很久，也退出
            if (elapsed_ms > 5000 && (!has_speech || total_speech_ms < RECORD_MIN_SPEECH_MS)) {
                ESP_LOGW(TAG, "5秒内未检测到有效语音（时长: %lldms），退出录音", total_speech_ms);
                h->audio_pos = 0;  // 清空缓冲区
                break;
            }
            
            // 缓冲区满
            if (h->audio_pos >= h->audio_buffer_size) {
                ESP_LOGI(TAG, "录音缓冲区已满");
                break;
            }
            
            vTaskDelay(pdMS_TO_TICKS(20));
        }
        
        if (!h->running) {
            mic.end();
            break;
        }

        ESP_LOGI(TAG, "录音统计: 有效语音=%lldms, 总音频=%.1fs, 尾部静音=%lldms",
                 total_speech_ms, (float)h->audio_pos / SAMPLE_RATE,
                 last_voice_time > 0 ? (esp_timer_get_time() - last_voice_time) / 1000 : 0);
        
        // ============ 阶段4: 保持麦克风在线，网络与播报阶段交给打断监听任务读取 ============
        printf("[录音结束] 共 %.1f 秒\n", (float)h->audio_pos / SAMPLE_RATE);
        fflush(stdout);
        
        // ============ 阶段5: ASR识别 ============
        bool asr_success = do_asr_recognize(h, h->audio_buffer, h->audio_pos);
        
        // ============ 阶段6: 确保音频输出可用 ============
        // 音乐播放器已占用喇叭时，继续沿用音乐播放策略。
        if (!h->music_playing) {
            if (ensure_audio_output(h) != ESP_OK) {
                ESP_LOGE(TAG, "音频输出初始化失败");
                continue;
            }
        }
        
        // ============ 阶段7: AI对话和TTS播放 ============
        if (asr_success && strlen(h->asr_result) > 0) {
            // 本地意图优先：天气这类本机已有数据的问题直接本地回答，不走大模型
            if (!try_local_intent(h, h->asr_result)) {
                send_to_spark_and_respond(h, h->asr_result);
            }
            
            // 如果音乐开始播放，立即退出对话模式
            if (h->music_playing) {
                ESP_LOGI(TAG, "音乐播放已启动，退出对话模式");
                printf("\n[语音助手] 音乐播放中，退出对话模式。\n");
                fflush(stdout);
                dialog_active = false;
                continue;
            }
            
            // 有有效输入，重置对话超时计时器
            dialog_start_time = esp_timer_get_time();
        } else {
            printf("[语音助手] 未识别到有效内容，继续监听...\n");
            fflush(stdout);
        }
        
        // 云端请求期间没有读取麦克风，清掉 DMA 中的旧片段再听新问题。
        for (int i = 0; i < 5; ++i) {
            size_t discarded = 0;
            mic.read(h->chunk_buffer, BUFFER_SIZE, &discarded, 20);
        }
        
        // ============ 阶段8: 重新初始化麦克风 ============
        // 再次检查音乐播放状态（可能在TTS播放期间启动了音乐）
        if (h->music_playing) {
            ESP_LOGI(TAG, "音乐正在播放，退出对话模式");
            printf("\n[语音助手] 音乐播放中，退出对话模式。\n");
            fflush(stdout);
            dialog_active = false;
            continue;
        }
        
        if (h->barge_requested) {
            h->barge_requested = false;
            ESP_LOGI(TAG, "已响应播报打断，继续接收新问题");
        }
        ESP_LOGI(TAG, "对话轮次完成，继续监听");
        }  // 对话模式循环结束
        
        // 回到唤醒词监听，复用已开启的 I2S0。
    }  // 主循环结束
    
    h->active_mic = nullptr;
    mic.end();
    h->state = VOICE_DIALOG_IDLE;
    ESP_LOGI(TAG, "语音对话任务退出");
    vTaskDelete(NULL);
}

// ==================== 公共API ====================

voice_dialog_config_t voice_dialog_get_default_config(void)
{
    voice_dialog_config_t config = {
        .wakeup_word = "小智",
        .mic_data_pin = -1,
        .mic_clk_pin = -1,
        .sample_rate = SAMPLE_RATE,
        .vad_threshold = VAD_DEFAULT_THRESHOLD,
        .wakeup_timeout_ms = WAKEUP_DEFAULT_MS,
        .record_timeout_ms = RECORD_DEFAULT_MS,
        .silence_timeout_ms = SILENCE_DEFAULT_MS,
        .baidu_api_key = NULL,
        .baidu_secret_key = NULL,
        .spark_client = NULL,
        .tts_handle = NULL,
        .audio_handle = NULL,
        .audio_bclk_pin = -1,
        .audio_lrclk_pin = -1,
        .audio_din_pin = -1,
        .audio_sd_mode_pin = -1,
    };
    return config;
}

esp_err_t voice_dialog_init(const voice_dialog_config_t *config,
                            voice_dialog_handle_t *handle)
{
    if (config == NULL || handle == NULL) {
        return ESP_ERR_INVALID_ARG;
    }
    
    if (config->mic_data_pin < 0 || config->mic_clk_pin < 0) {
        ESP_LOGE(TAG, "麦克风引脚未配置");
        return ESP_ERR_INVALID_ARG;
    }
    
    if (config->baidu_api_key == NULL || config->baidu_secret_key == NULL) {
        ESP_LOGE(TAG, "百度API Key未配置");
        return ESP_ERR_INVALID_ARG;
    }
    
    // 分配句柄
    struct voice_dialog_handle *h = (struct voice_dialog_handle *)calloc(1, sizeof(struct voice_dialog_handle));
    if (h == NULL) {
        ESP_LOGE(TAG, "句柄分配失败");
        return ESP_ERR_NO_MEM;
    }
    
    h->config = *config;
    h->state = VOICE_DIALOG_IDLE;
    h->running = false;
    h->triggered = false;
    
    // 创建互斥锁
    h->mutex = xSemaphoreCreateMutex();
    if (h->mutex == NULL) {
        ESP_LOGE(TAG, "互斥锁创建失败");
        free(h);
        return ESP_ERR_NO_MEM;
    }
    
    // 初始化百度ASR
    baidu_asr_config_t asr_cfg = {
        .api_key = config->baidu_api_key,
        .secret_key = config->baidu_secret_key,
        .sample_rate = config->sample_rate,
        .timeout_ms = 30000,
    };
    
    esp_err_t ret = baidu_asr_init(&h->asr, &asr_cfg);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "ASR初始化失败");
        vSemaphoreDelete(h->mutex);
        free(h);
        return ret;
    }
    
    // 分配音频缓冲区（优先使用PSRAM）
    h->audio_buffer_size = MAX_RECORD_SAMPLES;
    h->audio_buffer = (int16_t *)heap_caps_malloc(h->audio_buffer_size * sizeof(int16_t), 
                                                   MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
    if (h->audio_buffer == NULL) {
        ESP_LOGW(TAG, "PSRAM分配失败，尝试内部RAM");
        h->audio_buffer = (int16_t *)malloc(h->audio_buffer_size * sizeof(int16_t));
    }
    if (h->audio_buffer == NULL) {
        ESP_LOGE(TAG, "录音缓冲区分配失败");
        vSemaphoreDelete(h->mutex);
        free(h);
        return ESP_ERR_NO_MEM;
    }
    ESP_LOGI(TAG, "录音缓冲区已分配: %u字节", h->audio_buffer_size * 2);
    
    // 分配唤醒词缓冲区
    h->wakeup_buffer = (int16_t *)heap_caps_malloc(WAKEUP_AUDIO_SAMPLES * sizeof(int16_t), 
                                                    MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
    if (h->wakeup_buffer == NULL) {
        h->wakeup_buffer = (int16_t *)malloc(WAKEUP_AUDIO_SAMPLES * sizeof(int16_t));
    }
    if (h->wakeup_buffer == NULL) {
        ESP_LOGE(TAG, "唤醒词缓冲区分配失败");
        free(h->audio_buffer);
        vSemaphoreDelete(h->mutex);
        free(h);
        return ESP_ERR_NO_MEM;
    }
    
    // 分配临时读取缓冲区
    h->chunk_buffer = (int16_t *)malloc(BUFFER_SIZE);
    if (h->chunk_buffer == NULL) {
        ESP_LOGE(TAG, "临时缓冲区分配失败");
        free(h->wakeup_buffer);
        free(h->audio_buffer);
        vSemaphoreDelete(h->mutex);
        free(h);
        return ESP_ERR_NO_MEM;
    }
    
    // 初始化TTS（如果外部未提供则自己创建）
    h->own_tts = false;
    h->internal_tts = NULL;
    if (config->tts_handle == NULL) {
        ESP_LOGI(TAG, "创建内部TTS句柄...");
        h->internal_tts = (baidu_tts_handle_t *)malloc(sizeof(baidu_tts_handle_t));
        if (h->internal_tts != NULL) {
            baidu_tts_config_t tts_cfg = {
                .api_key = config->baidu_api_key,
                .secret_key = config->baidu_secret_key,
                .voice = BAIDU_TTS_VOICE_XIAOLU,  // 甜美女声
                .speed = 5,
                .pitch = 5,
                .volume = 10,
                .timeout_ms = 30000,
            };
            esp_err_t tts_ret = baidu_tts_init(h->internal_tts, &tts_cfg, NULL);
            if (tts_ret == ESP_OK) {
                h->config.tts_handle = h->internal_tts;
                h->own_tts = true;
                ESP_LOGI(TAG, "内部TTS初始化成功");
            } else {
                ESP_LOGW(TAG, "内部TTS初始化失败: %s", esp_err_to_name(tts_ret));
                free(h->internal_tts);
                h->internal_tts = NULL;
            }
        } else {
            ESP_LOGW(TAG, "TTS内存分配失败");
        }
    }
    
    *handle = h;
    ESP_LOGI(TAG, "语音对话模块初始化完成");
    ESP_LOGI(TAG, "  唤醒词: %s", config->wakeup_word);
    ESP_LOGI(TAG, "  麦克风: CLK=GPIO%d, DATA=GPIO%d", config->mic_clk_pin, config->mic_data_pin);
    ESP_LOGI(TAG, "  VAD阈值: %d, 静音超时: %lums", config->vad_threshold, config->silence_timeout_ms);
    
    return ESP_OK;
}

esp_err_t voice_dialog_set_callback(voice_dialog_handle_t handle,
                                    voice_dialog_event_cb_t callback,
                                    void *user_ctx)
{
    if (handle == NULL) {
        return ESP_ERR_INVALID_ARG;
    }
    
    handle->callback = callback;
    handle->user_ctx = user_ctx;
    return ESP_OK;
}

esp_err_t voice_dialog_start(voice_dialog_handle_t handle)
{
    if (handle == NULL) {
        return ESP_ERR_INVALID_ARG;
    }
    
    if (handle->running) {
        ESP_LOGW(TAG, "语音对话已在运行");
        return ESP_OK;
    }
    
    handle->running = true;
    handle->state = VOICE_DIALOG_LISTENING;
    
    // 创建任务（在核心1上运行，避免与WiFi任务冲突）
    BaseType_t ret = xTaskCreatePinnedToCore(
        voice_dialog_task,
        "voice_dialog",
        16384,          // 栈大小（增大以避免栈溢出）
        handle,
        5,              // 优先级
        &handle->task_handle,
        1               // 核心1
    );
    
    if (ret != pdPASS) {
        ESP_LOGE(TAG, "任务创建失败");
        handle->running = false;
        return ESP_FAIL;
    }
    
    ESP_LOGI(TAG, "语音对话已启动");
    return ESP_OK;
}

esp_err_t voice_dialog_stop(voice_dialog_handle_t handle)
{
    if (handle == NULL) {
        return ESP_ERR_INVALID_ARG;
    }
    
    if (!handle->running) {
        return ESP_OK;
    }
    
    ESP_LOGI(TAG, "停止语音对话...");
    handle->running = false;
    
    // 等待任务退出
    vTaskDelay(pdMS_TO_TICKS(500));
    
    handle->state = VOICE_DIALOG_IDLE;
    ESP_LOGI(TAG, "语音对话已停止");
    return ESP_OK;
}

esp_err_t voice_dialog_trigger(voice_dialog_handle_t handle)
{
    if (handle == NULL) {
        return ESP_ERR_INVALID_ARG;
    }
    
    if (!handle->running) {
        ESP_LOGW(TAG, "语音对话未运行");
        return ESP_ERR_INVALID_STATE;
    }

    if (handle->state == VOICE_DIALOG_SPEAKING && handle->config.tts_handle != NULL) {
        handle->barge_requested = true;
        baidu_tts_request_cancel(handle->config.tts_handle);
        ESP_LOGI(TAG, "屏幕按钮打断当前播报");
        return ESP_OK;
    }
    
    handle->triggered = true;
    ESP_LOGI(TAG, "手动触发唤醒");
    return ESP_OK;
}

voice_dialog_state_t voice_dialog_get_state(voice_dialog_handle_t handle)
{
    if (handle == NULL) {
        return VOICE_DIALOG_IDLE;
    }
    return handle->state;
}

void voice_dialog_deinit(voice_dialog_handle_t handle)
{
    if (handle == NULL) {
        return;
    }
    
    voice_dialog_stop(handle);
    
    // 释放内部创建的TTS
    if (handle->own_tts && handle->internal_tts != NULL) {
        // baidu_tts没有deinit函数，直接释放内存
        free(handle->internal_tts);
        handle->internal_tts = NULL;
        ESP_LOGI(TAG, "内部TTS已释放");
    }
    
    if (handle->chunk_buffer) {
        free(handle->chunk_buffer);
    }
    if (handle->wakeup_buffer) {
        free(handle->wakeup_buffer);
    }
    if (handle->audio_buffer) {
        free(handle->audio_buffer);
    }
    if (handle->mutex) {
        vSemaphoreDelete(handle->mutex);
    }
    
    free(handle);
    ESP_LOGI(TAG, "语音对话模块已释放");
}

void voice_dialog_set_music_playing(voice_dialog_handle_t handle, bool playing)
{
    if (handle == NULL) {
        ESP_LOGW(TAG, "voice_dialog_set_music_playing: handle为NULL!");
        return;
    }
    bool old_value = handle->music_playing;
    handle->music_playing = playing;
    ESP_LOGI(TAG, "音乐播放标志: %s -> %s", 
             old_value ? "true" : "false",
             playing ? "true" : "false");
}

void voice_dialog_set_skip_tts(voice_dialog_handle_t handle, bool skip)
{
    if (handle == NULL) {
        return;
    }
    handle->skip_tts = skip;
    if (skip) {
        ESP_LOGI(TAG, "设置跳过AI回复的TTS播放");
    }
}

} // extern "C"
