/**
 * @file voice_input.cpp
 * @brief 语音输入模块实现（按需录音，麦克风使用I2S0）
 */

#include "voice_input.h"
#include "baidu_asr.h"
#include "MSM261D4030H1CPM_Driver_V5.hpp"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "esp_log.h"
#include "esp_timer.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static const char *TAG = "VoiceInput";

// 音频缓冲区大小 (3秒 @ 16kHz = 48000 样本 = 96000 字节)
// 减少到3秒以节省内存（原来是5秒/160KB）
#define AUDIO_BUFFER_SAMPLES    (16000 * 3)
#define AUDIO_CHUNK_SIZE        1024

/**
 * @brief 语音输入内部结构
 */
struct voice_input_handle {
    voice_input_config_t config;
    baidu_asr_handle_t asr;
    max98357a_handle_t *audio_handle;
    
    voice_input_state_t state;
    volatile bool stop_flag;        // 外部停止录音标志
    
    int16_t *audio_buffer;
    size_t audio_buffer_size;
};

// C++ 实现，但导出为 C 接口
extern "C" {

/**
 * @brief 计算音频能量（用于检测静音）
 */
static uint32_t calculate_audio_energy(const int16_t *samples, size_t count)
{
    if (count == 0) return 0;
    
    uint64_t sum = 0;
    for (size_t i = 0; i < count; i++) {
        int32_t sample = samples[i];
        sum += (uint64_t)(sample * sample);
    }
    return (uint32_t)(sum / count);
}

voice_input_config_t voice_input_get_default_config(void)
{
    voice_input_config_t config = {
        .mic_data_pin = -1,
        .mic_clk_pin = -1,
        .sample_rate = 16000,
        .max_record_ms = 10000,
        .silence_timeout_ms = 1500,
        .silence_threshold = 2000,  // 提高静音阈值，更好地区分语音和噪音
        .api_key = NULL,
        .secret_key = NULL,
    };
    return config;
}

esp_err_t voice_input_init(const voice_input_config_t *config,
                           max98357a_handle_t *audio_handle,
                           voice_input_handle_t *handle)
{
    if (config == NULL || handle == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    if (config->mic_data_pin < 0 || config->mic_clk_pin < 0) {
        ESP_LOGE(TAG, "麦克风引脚配置无效");
        return ESP_ERR_INVALID_ARG;
    }

    if (config->api_key == NULL || config->secret_key == NULL) {
        ESP_LOGE(TAG, "百度API配置无效");
        return ESP_ERR_INVALID_ARG;
    }

    // 分配句柄
    struct voice_input_handle *h = (struct voice_input_handle *)calloc(1, sizeof(struct voice_input_handle));
    if (h == NULL) {
        ESP_LOGE(TAG, "句柄分配失败");
        return ESP_ERR_NO_MEM;
    }

    h->config = *config;
    h->audio_handle = audio_handle;
    h->state = VOICE_INPUT_IDLE;

    // 初始化百度ASR
    baidu_asr_config_t asr_config = {
        .api_key = config->api_key,
        .secret_key = config->secret_key,
        .sample_rate = config->sample_rate,
        .timeout_ms = 30000,
    };
    
    esp_err_t err = baidu_asr_init(&h->asr, &asr_config);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "ASR初始化失败");
        free(h);
        return err;
    }

    // 分配音频缓冲区
    h->audio_buffer_size = AUDIO_BUFFER_SAMPLES;
    h->audio_buffer = (int16_t *)heap_caps_malloc(h->audio_buffer_size * sizeof(int16_t), 
                                                   MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
    if (h->audio_buffer == NULL) {
        h->audio_buffer = (int16_t *)malloc(h->audio_buffer_size * sizeof(int16_t));
    }
    if (h->audio_buffer == NULL) {
        ESP_LOGE(TAG, "音频缓冲区分配失败");
        free(h);
        return ESP_ERR_NO_MEM;
    }

    *handle = h;
    ESP_LOGI(TAG, "语音输入模块初始化完成");
    return ESP_OK;
}

esp_err_t voice_input_record_and_recognize(voice_input_handle_t handle,
                                            char *result,
                                            size_t result_size)
{
    if (handle == NULL || result == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    struct voice_input_handle *h = handle;
    result[0] = '\0';
    h->stop_flag = false;  // 清除停止标志
    
    ESP_LOGI(TAG, "开始语音录制...");
    
    h->state = VOICE_INPUT_RECORDING;

    // 麦克风使用I2S0，喇叭使用独立的I2S1。
    MSM261D4030H1CPM::MicDriver mic;
    MSM261D4030H1CPM::mic_config_t mic_cfg = {
        .pin_clk = (gpio_num_t)h->config.mic_clk_pin,
        .pin_data = (gpio_num_t)h->config.mic_data_pin,
        .i2s_port = I2S_NUM_0,  // ESP32-S3的PDM接收只能使用I2S0
        .sample_rate = h->config.sample_rate,
        .channel = MSM261D4030H1CPM::MIC_CHANNEL_LEFT,
        .mode = MSM261D4030H1CPM::MIC_MODE_STANDARD_PERFORMANCE,
        .dma_buf_count = 6,  // 增加DMA缓冲区数量，提高稳定性
        .dma_buf_len = 512,  // 减小单个缓冲区大小，降低延迟
        .use_apll = true,
        .mclk_multiple = 256
    };
    mic.config(mic_cfg);

    esp_err_t mic_ret = mic.begin();
    if (mic_ret != ESP_OK) {
        ESP_LOGE(TAG, "麦克风初始化失败: %s", esp_err_to_name(mic_ret));
        h->state = VOICE_INPUT_ERROR;
        return ESP_FAIL;
    }

    // 3. 录制音频
    int16_t *chunk_buffer = (int16_t *)malloc(AUDIO_CHUNK_SIZE * sizeof(int16_t));
    if (chunk_buffer == NULL) {
        ESP_LOGE(TAG, "chunk缓冲区分配失败");
        mic.end();
        h->state = VOICE_INPUT_ERROR;
        return ESP_ERR_NO_MEM;
    }

    size_t audio_pos = 0;
    int64_t record_start = esp_timer_get_time();
    int64_t last_speech_time = record_start;
    bool has_speech = false;
    
    // 先丢弃一些初始数据（麦克风稳定）- 增加丢弃次数确保麦克风完全稳定
    for (int i = 0; i < 15; i++) {
        size_t bytes_read = 0;
        mic.read(chunk_buffer, AUDIO_CHUNK_SIZE * sizeof(int16_t), &bytes_read, 100);
        vTaskDelay(pdMS_TO_TICKS(10));
    }
    
    ESP_LOGI(TAG, "开始录音，最长 %lu ms", h->config.max_record_ms);
    
    int loop_count = 0;
    while (true) {
        loop_count++;
        
        // 每10次循环输出一次状态
        if (loop_count % 10 == 0) {
            int64_t now_ms = (esp_timer_get_time() - record_start) / 1000;
            ESP_LOGI(TAG, "录音循环 #%d, 时间=%lld ms, stop_flag=%d, 已录=%zu样本", 
                     loop_count, now_ms, h->stop_flag, audio_pos);
        }
        
        size_t bytes_read = 0;
        esp_err_t err = mic.read(chunk_buffer, AUDIO_CHUNK_SIZE * sizeof(int16_t), &bytes_read, 500);  // 增加超时到500ms
        
        if (err != ESP_OK || bytes_read == 0) {
            vTaskDelay(pdMS_TO_TICKS(10));
            continue;
        }

        size_t samples_read = bytes_read / sizeof(int16_t);
        int64_t now = esp_timer_get_time();
        
        // 计算音量
        uint32_t energy = calculate_audio_energy(chunk_buffer, samples_read);
        
        // 复制到缓冲区
        size_t space_left = h->audio_buffer_size - audio_pos;
        size_t copy_samples = (samples_read < space_left) ? samples_read : space_left;
        
        if (copy_samples > 0) {
            memcpy(h->audio_buffer + audio_pos, chunk_buffer, copy_samples * sizeof(int16_t));
            audio_pos += copy_samples;
        }
        
        // 检测是否有声音
        if (energy > h->config.silence_threshold) {
            last_speech_time = now;
            if (!has_speech) {
                has_speech = true;
                ESP_LOGI(TAG, "检测到语音，能量: %lu", energy);
            }
        }
        
        // 检查结束条件
        int64_t elapsed_ms = (now - record_start) / 1000;
        
        // 外部停止标志（用户松开按钮）
        if (h->stop_flag) {
            ESP_LOGI(TAG, "用户松开按钮，停止录音");
            break;
        }
        
        // 超时
        if (elapsed_ms > (int64_t)h->config.max_record_ms) {
            ESP_LOGI(TAG, "录音超时");
            break;
        }
        
        // 静音超时（需要先检测到语音）- 仅当没有外部停止标志时使用
        // 注释掉静音检测，改为完全由按钮控制
        // if (has_speech && silence_ms > (int64_t)h->config.silence_timeout_ms) {
        //     ESP_LOGI(TAG, "检测到静音，停止录音");
        //     break;
        // }
        
        // 缓冲区满
        if (audio_pos >= h->audio_buffer_size) {
            ESP_LOGI(TAG, "缓冲区已满");
            break;
        }
    }
    
    // 4. 停止麦克风
    mic.end();
    free(chunk_buffer);
    
    ESP_LOGI(TAG, "录音完成，共 %u 样本 (%.1f 秒)", 
             audio_pos, (float)audio_pos / h->config.sample_rate);
    
    // 5. 语音识别
    if (audio_pos > h->config.sample_rate / 4) {  // 降低最小录音时长要求到0.25秒
        h->state = VOICE_INPUT_RECOGNIZING;
        
        // 对音频进行简单的音量归一化处理
        int16_t max_amplitude = 0;
        for (size_t i = 0; i < audio_pos; i++) {
            int16_t abs_val = (h->audio_buffer[i] < 0) ? -h->audio_buffer[i] : h->audio_buffer[i];
            if (abs_val > max_amplitude) {
                max_amplitude = abs_val;
            }
        }
        
        // 如果音量太小，进行放大（但不超过16位范围）
        if (max_amplitude > 0 && max_amplitude < 8000) {
            float gain = 8000.0f / max_amplitude;
            if (gain > 4.0f) gain = 4.0f;  // 限制最大增益为4倍
            ESP_LOGI(TAG, "音频音量较小，应用软件增益: %.2f倍", gain);
            for (size_t i = 0; i < audio_pos; i++) {
                int32_t amplified = (int32_t)(h->audio_buffer[i] * gain);
                if (amplified > 32767) amplified = 32767;
                if (amplified < -32768) amplified = -32768;
                h->audio_buffer[i] = (int16_t)amplified;
            }
        }
        
        esp_err_t asr_err = baidu_asr_recognize(&h->asr, h->audio_buffer,
                                                 audio_pos * sizeof(int16_t),
                                                 result, result_size);
        
        if (asr_err == ESP_OK && strlen(result) > 0) {
            ESP_LOGI(TAG, "语音识别成功: %s", result);
        } else {
            ESP_LOGW(TAG, "语音识别失败或结果为空");
        }
    } else {
        ESP_LOGW(TAG, "录音太短，跳过识别");
    }
    
    h->state = VOICE_INPUT_IDLE;
    
    // 注意：音频输出需要由调用者重新初始化
    // 这是因为音频句柄在主程序中管理
    
    return (strlen(result) > 0) ? ESP_OK : ESP_FAIL;
}

voice_input_state_t voice_input_get_state(voice_input_handle_t handle)
{
    if (handle == NULL) {
        return VOICE_INPUT_IDLE;
    }
    return handle->state;
}

void voice_input_set_stop_flag(voice_input_handle_t handle, bool stop)
{
    if (handle != NULL) {
        ESP_LOGI(TAG, "设置 stop_flag = %d (当前值: %d)", stop, handle->stop_flag);
        handle->stop_flag = stop;
    } else {
        ESP_LOGE(TAG, "handle 为 NULL，无法设置 stop_flag");
    }
}

void voice_input_deinit(voice_input_handle_t handle)
{
    if (handle == NULL) {
        return;
    }

    if (handle->audio_buffer) {
        free(handle->audio_buffer);
    }

    free(handle);
    ESP_LOGI(TAG, "语音输入模块已释放");
}

} // extern "C"

