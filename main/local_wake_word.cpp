#include "local_wake_word.h"

#include <stdlib.h>
#include <string.h>

#include "esp_afe_config.h"
#include "esp_afe_sr_iface.h"
#include "esp_afe_sr_models.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "model_path.h"

static const char *TAG = "LocalWake";

struct local_wake_word {
    srmodel_list_t *models;
    const esp_afe_sr_iface_t *iface;
    esp_afe_sr_data_t *afe;
    int16_t *feed_buffer;
    size_t feed_samples;
    size_t buffered_samples;
};

extern "C" void local_wake_word_destroy(local_wake_word_t *wake)
{
    if (wake == nullptr) return;
    if (wake->afe != nullptr && wake->iface != nullptr) wake->iface->destroy(wake->afe);
    if (wake->models != nullptr) esp_srmodel_deinit(wake->models);
    free(wake->feed_buffer);
    free(wake);
}

extern "C" esp_err_t local_wake_word_create(local_wake_word_t **out)
{
    if (out == nullptr) return ESP_ERR_INVALID_ARG;
    *out = nullptr;

    local_wake_word_t *wake = static_cast<local_wake_word_t *>(calloc(1, sizeof(*wake)));
    if (wake == nullptr) return ESP_ERR_NO_MEM;

    wake->models = esp_srmodel_init("model");
    if (wake->models == nullptr || wake->models->num <= 0) {
        ESP_LOGE(TAG, "未找到本地唤醒模型，请完整烧录 model 分区");
        local_wake_word_destroy(wake);
        return ESP_ERR_NOT_FOUND;
    }

    char *model = esp_srmodel_filter(wake->models, "wn9", "nihaoxiaozhi");
    if (model == nullptr) {
        ESP_LOGE(TAG, "model 分区中没有“你好小智”模型");
        local_wake_word_destroy(wake);
        return ESP_ERR_NOT_FOUND;
    }

    afe_config_t *config = afe_config_init("M", wake->models, AFE_TYPE_SR, AFE_MODE_LOW_COST);
    if (config == nullptr) {
        local_wake_word_destroy(wake);
        return ESP_ERR_NO_MEM;
    }
    // 手表只有一颗麦克风，没有喇叭回采通道；其余音频处理交给 AFE。
    config->aec_init = false;
    config->se_init = false;
    config->wakenet_init = true;
    config->wakenet_model_name = model;
    config->memory_alloc_mode = AFE_MEMORY_ALLOC_MORE_PSRAM;
    config->afe_perferred_core = 1;
    config->afe_perferred_priority = 4;

    wake->iface = esp_afe_handle_from_config(config);
    if (wake->iface != nullptr) wake->afe = wake->iface->create_from_config(config);
    afe_config_free(config);
    if (wake->afe == nullptr) {
        ESP_LOGE(TAG, "唤醒引擎初始化失败");
        local_wake_word_destroy(wake);
        return ESP_FAIL;
    }

    wake->feed_samples = static_cast<size_t>(wake->iface->get_feed_chunksize(wake->afe));
    if (wake->feed_samples == 0 || wake->feed_samples > 2048) {
        local_wake_word_destroy(wake);
        return ESP_ERR_INVALID_SIZE;
    }
    wake->feed_buffer = static_cast<int16_t *>(malloc(wake->feed_samples * sizeof(int16_t)));
    if (wake->feed_buffer == nullptr) {
        local_wake_word_destroy(wake);
        return ESP_ERR_NO_MEM;
    }
    ESP_LOGI(TAG, "本地唤醒就绪: %s, 每帧 %u 样本", model, (unsigned)wake->feed_samples);
    *out = wake;
    return ESP_OK;
}

extern "C" bool local_wake_word_process(local_wake_word_t *wake, const int16_t *pcm, size_t samples)
{
    if (wake == nullptr || pcm == nullptr) return false;
    while (samples > 0) {
        size_t count = wake->feed_samples - wake->buffered_samples;
        if (count > samples) count = samples;
        memcpy(wake->feed_buffer + wake->buffered_samples, pcm, count * sizeof(int16_t));
        wake->buffered_samples += count;
        pcm += count;
        samples -= count;
        if (wake->buffered_samples != wake->feed_samples) continue;

        wake->iface->feed(wake->afe, wake->feed_buffer);
        wake->buffered_samples = 0;
        // 每送入一帧只等一次结果；读到队列为空会让 AFE 持续打印警告。
        afe_fetch_result_t *result = wake->iface->fetch_with_delay(wake->afe, pdMS_TO_TICKS(50));
        if (result != nullptr && result->ret_value == ESP_OK &&
            result->wakeup_state == WAKENET_DETECTED) return true;
    }
    return false;
}

extern "C" void local_wake_word_reset(local_wake_word_t *wake)
{
    if (wake == nullptr) return;
    wake->buffered_samples = 0;
    wake->iface->reset_buffer(wake->afe);
}
