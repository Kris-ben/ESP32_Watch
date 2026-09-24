#include "voice_barge_in.h"

#include <math.h>
#include <stdlib.h>
#include <string.h>
#include "esp_heap_caps.h"
#include "esp_log.h"
#include "esp_timer.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "freertos/task.h"

namespace {
constexpr size_t kChunkSamples = 1024;
constexpr size_t kWakeSamples = 16000 * 3;
constexpr int64_t kSilenceUs = 500000;
constexpr uint32_t kSpeechMinMs = 400;
const char *kTag = "VoiceBargeIn";
}

struct voice_barge_in {
    MSM261D4030H1CPM::MicDriver *mic;
    baidu_asr_handle_t asr;
    baidu_tts_handle_t *tts;
    const char *wakeup_word;
    uint16_t vad_threshold;
    int16_t *chunk;
    int16_t *audio;
    SemaphoreHandle_t done;
    bool stop;
    bool detected;
};

static void monitor_task(void *arg)
{
    voice_barge_in *monitor = static_cast<voice_barge_in *>(arg);
    size_t audio_pos = 0;
    uint32_t speech_ms = 0;
    int64_t last_voice_us = 0;

    while (!__atomic_load_n(&monitor->stop, __ATOMIC_ACQUIRE)) {
        size_t bytes_read = 0;
        if (monitor->mic->read(monitor->chunk, kChunkSamples * sizeof(int16_t),
                               &bytes_read, 100) != ESP_OK || bytes_read == 0) {
            vTaskDelay(pdMS_TO_TICKS(10));
            continue;
        }

        if (!baidu_tts_is_playing_audio(monitor->tts)) {
            audio_pos = 0;
            speech_ms = 0;
            last_voice_us = 0;
            continue;
        }

        size_t samples = bytes_read / sizeof(int16_t);
        if (samples == 0) continue;
        int32_t dc_sum = 0;
        for (size_t i = 0; i < samples; ++i) {
            dc_sum += monitor->chunk[i];
        }
        int32_t dc = dc_sum / static_cast<int32_t>(samples);
        uint64_t energy = 0;
        for (size_t i = 0; i < samples; ++i) {
            int32_t value = (static_cast<int32_t>(monitor->chunk[i]) - dc) * 5;
            if (value > 32767) value = 32767;
            if (value < -32768) value = -32768;
            monitor->chunk[i] = static_cast<int16_t>(value);
            energy += static_cast<int64_t>(value) * value;
        }
        float rms = sqrtf(static_cast<float>(energy / samples));
        bool speech = rms > monitor->vad_threshold;
        int64_t now_us = esp_timer_get_time();
        if (speech) {
            last_voice_us = now_us;
            speech_ms += static_cast<uint32_t>(samples * 1000 / 16000);
        }
        if (speech || audio_pos > 0) {
            size_t to_copy = samples < kWakeSamples - audio_pos ? samples : kWakeSamples - audio_pos;
            memcpy(monitor->audio + audio_pos, monitor->chunk, to_copy * sizeof(int16_t));
            audio_pos += to_copy;
        }

        if (audio_pos >= 16000 / 2 && speech_ms >= kSpeechMinMs &&
            !speech && last_voice_us > 0 && now_us - last_voice_us >= kSilenceUs) {
            char result[256] = {0};
            ESP_LOGI(kTag, "播报中检测到语音，核对唤醒词");
            esp_err_t err = baidu_asr_recognize(&monitor->asr, monitor->audio,
                                                 audio_pos * sizeof(int16_t),
                                                 result, sizeof(result));
            // 只接受简短唤醒语，减少把喇叭播报的整句话误认成用户打断。
            if (err == ESP_OK && strlen(result) <= 24 &&
                strstr(result, monitor->wakeup_word) != nullptr) {
                ESP_LOGI(kTag, "听到唤醒词，停止播报: %s", result);
                __atomic_store_n(&monitor->detected, true, __ATOMIC_RELEASE);
                baidu_tts_request_cancel(monitor->tts);
                break;
            }
            audio_pos = 0;
            speech_ms = 0;
            last_voice_us = 0;
        } else if (audio_pos >= kWakeSamples ||
                   (audio_pos > 0 && last_voice_us > 0 && now_us - last_voice_us > 3000000)) {
            audio_pos = 0;
            speech_ms = 0;
            last_voice_us = 0;
        }
    }

    xSemaphoreGive(monitor->done);
    vTaskDelete(nullptr);
}

voice_barge_in *voice_barge_in_start(MSM261D4030H1CPM::MicDriver *mic,
                                    const baidu_asr_handle_t *asr,
                                    baidu_tts_handle_t *tts,
                                    const char *wakeup_word,
                                    uint16_t vad_threshold)
{
    if (mic == nullptr || asr == nullptr || tts == nullptr || wakeup_word == nullptr) {
        return nullptr;
    }
    voice_barge_in *monitor = static_cast<voice_barge_in *>(calloc(1, sizeof(voice_barge_in)));
    if (monitor == nullptr) return nullptr;
    monitor->mic = mic;
    monitor->asr = *asr;
    monitor->asr.config.timeout_ms = 8000;
    monitor->asr.config.max_retries = 1;
    monitor->tts = tts;
    monitor->wakeup_word = wakeup_word;
    // 播报时喇叭声也会进入麦克风，打断阈值不随普通录音灵敏度一同降低。
    monitor->vad_threshold = vad_threshold < 2000 ? 2000 : vad_threshold;
    monitor->done = xSemaphoreCreateBinary();
    monitor->chunk = static_cast<int16_t *>(malloc(kChunkSamples * sizeof(int16_t)));
    monitor->audio = static_cast<int16_t *>(heap_caps_malloc(kWakeSamples * sizeof(int16_t),
                                                             MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT));
    if (monitor->audio == nullptr) {
        monitor->audio = static_cast<int16_t *>(malloc(kWakeSamples * sizeof(int16_t)));
    }
    if (monitor->done == nullptr || monitor->chunk == nullptr || monitor->audio == nullptr ||
        xTaskCreatePinnedToCore(monitor_task, "voice_barge", 8192, monitor, 5, nullptr, 1) != pdPASS) {
        if (monitor->done != nullptr) vSemaphoreDelete(monitor->done);
        free(monitor->chunk);
        free(monitor->audio);
        free(monitor);
        ESP_LOGW(kTag, "播报时监听任务启动失败");
        return nullptr;
    }
    return monitor;
}

bool voice_barge_in_stop(voice_barge_in *monitor)
{
    if (monitor == nullptr) return false;
    __atomic_store_n(&monitor->stop, true, __ATOMIC_RELEASE);
    xSemaphoreTake(monitor->done, portMAX_DELAY);
    bool detected = __atomic_load_n(&monitor->detected, __ATOMIC_ACQUIRE);
    vSemaphoreDelete(monitor->done);
    free(monitor->chunk);
    free(monitor->audio);
    free(monitor);
    return detected;
}
