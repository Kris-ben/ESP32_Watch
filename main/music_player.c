#include "music_player.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "esp_log.h"
#include "sd_card_fs.h"
#include "audio_decoder/music_codec.h"
#include "max98357a.h"

static const char *TAG = "MUSIC_PLAYER";

extern max98357a_handle_t **get_audio_handle(void);

typedef struct {
    char selected_file[SD_MUSIC_NAME_MAX + sizeof(SD_MOUNT_POINT)];
    uint8_t volume;
    uint8_t progress_percent;
    bool playing;
    bool paused;
    bool stop_requested;
    uint32_t duration_ms;
    uint32_t played_ms;
    uint8_t seek_request_percent;
    bool seek_pending;
    TaskHandle_t task;
    SemaphoreHandle_t lock;
} music_player_state_t;

static music_player_state_t g_state = {
    .selected_file = {0},
    .volume = 80,
    .progress_percent = 0,
    .playing = false,
    .paused = false,
    .stop_requested = false,
    .duration_ms = 0,
    .played_ms = 0,
    .seek_request_percent = 0,
    .seek_pending = false,
    .task = NULL,
    .lock = NULL,
};

typedef struct {
    max98357a_handle_t *speaker;
    uint32_t sample_rate;
    int16_t *mix_buffer;
    size_t mix_buffer_samples;
    uint32_t skip_ms;
} playback_ctx_t;

static void ensure_lock(void)
{
    if (g_state.lock == NULL) {
        g_state.lock = xSemaphoreCreateMutex();
    }
}

static void set_playing_state(bool playing, bool paused)
{
    ensure_lock();
    if (xSemaphoreTake(g_state.lock, pdMS_TO_TICKS(50)) == pdTRUE) {
        g_state.playing = playing;
        g_state.paused = paused;
        xSemaphoreGive(g_state.lock);
    }
}

static esp_err_t playback_pcm_cb(const int16_t *pcm_interleaved,
                                 size_t frames,
                                 int channels,
                                 int sample_rate,
                                 void *user_data)
{
    playback_ctx_t *ctx = (playback_ctx_t *)user_data;
    if (!ctx || !ctx->speaker || !pcm_interleaved || channels <= 0) return ESP_ERR_INVALID_ARG;

    ensure_lock();

    while (1) {
        bool paused = false;
        bool stop_requested = false;
        if (xSemaphoreTake(g_state.lock, pdMS_TO_TICKS(20)) == pdTRUE) {
            paused = g_state.paused;
            stop_requested = g_state.stop_requested;
            xSemaphoreGive(g_state.lock);
        }

        if (stop_requested) {
            return ESP_ERR_INVALID_STATE;
        }
        if (!paused) break;
        vTaskDelay(pdMS_TO_TICKS(20));
    }

    if (ctx->sample_rate != (uint32_t)sample_rate) {
        esp_err_t rate_ret = max98357a_set_sample_rate(ctx->speaker, (uint32_t)sample_rate);
        if (rate_ret != ESP_OK) {
            return rate_ret;
        }
        ctx->sample_rate = (uint32_t)sample_rate;
    }

    uint32_t chunk_ms = (uint32_t)((frames * 1000ULL) / (uint32_t)sample_rate);
    if (ctx->skip_ms > 0) {
        if (chunk_ms >= ctx->skip_ms) {
            ctx->skip_ms = 0;
        } else {
            ctx->skip_ms -= chunk_ms;
            if (xSemaphoreTake(g_state.lock, pdMS_TO_TICKS(20)) == pdTRUE) {
                g_state.played_ms += chunk_ms;
                if (g_state.duration_ms > 0) {
                    uint32_t p = (uint32_t)((g_state.played_ms * 100ULL) / g_state.duration_ms);
                    g_state.progress_percent = (uint8_t)(p > 100 ? 100 : p);
                }
                xSemaphoreGive(g_state.lock);
            }
            return ESP_OK;
        }
    }

    size_t samples = frames * (size_t)channels;
    if (ctx->mix_buffer_samples < samples) {
        int16_t *new_buf = (int16_t *)realloc(ctx->mix_buffer, samples * sizeof(int16_t));
        if (!new_buf) return ESP_ERR_NO_MEM;
        ctx->mix_buffer = new_buf;
        ctx->mix_buffer_samples = samples;
    }

    uint8_t volume = 80;
    if (xSemaphoreTake(g_state.lock, pdMS_TO_TICKS(20)) == pdTRUE) {
        volume = g_state.volume;
        xSemaphoreGive(g_state.lock);
    }

    for (size_t i = 0; i < samples; i++) {
        int32_t v = (int32_t)pcm_interleaved[i] * (int32_t)volume / 100;
        if (v > 32767) v = 32767;
        if (v < -32768) v = -32768;
        ctx->mix_buffer[i] = (int16_t)v;
    }

    size_t written = 0;
    esp_err_t wr = max98357a_write(ctx->speaker, ctx->mix_buffer, samples * sizeof(int16_t), &written, 1000);
    if (wr != ESP_OK) {
        return wr;
    }

    if (xSemaphoreTake(g_state.lock, pdMS_TO_TICKS(20)) == pdTRUE) {
        g_state.played_ms += chunk_ms;
        if (g_state.duration_ms > 0) {
            uint32_t p = (uint32_t)((g_state.played_ms * 100ULL) / g_state.duration_ms);
            g_state.progress_percent = (uint8_t)(p > 100 ? 100 : p);
        }
        xSemaphoreGive(g_state.lock);
    }

    return ESP_OK;
}

static void player_task(void *arg)
{
    (void)arg;

    char file_path[SD_MUSIC_NAME_MAX + sizeof(SD_MOUNT_POINT)] = {0};
    uint32_t duration_ms = 0;
    uint8_t seek_percent = 0;

    ensure_lock();
    if (xSemaphoreTake(g_state.lock, pdMS_TO_TICKS(100)) == pdTRUE) {
        strncpy(file_path, g_state.selected_file, sizeof(file_path) - 1);
        g_state.stop_requested = false;
        g_state.playing = true;
        g_state.paused = false;
        g_state.progress_percent = 0;
        g_state.played_ms = 0;
        g_state.duration_ms = 0;
        seek_percent = g_state.seek_request_percent;
        g_state.seek_pending = false;
        xSemaphoreGive(g_state.lock);
    }

    max98357a_handle_t **pp_handle = get_audio_handle();
    if (!pp_handle || !*pp_handle || file_path[0] == '\0') {
        ESP_LOGE(TAG, "音频句柄或文件无效");
        set_playing_state(false, false);
        g_state.task = NULL;
        vTaskDelete(NULL);
        return;
    }

    max98357a_enable(*pp_handle);

    if (music_codec_get_duration_ms(file_path, &duration_ms) == ESP_OK) {
        if (xSemaphoreTake(g_state.lock, pdMS_TO_TICKS(50)) == pdTRUE) {
            g_state.duration_ms = duration_ms;
            xSemaphoreGive(g_state.lock);
        }
    }

    playback_ctx_t ctx = {
        .speaker = *pp_handle,
        .sample_rate = 0,
        .mix_buffer = NULL,
        .mix_buffer_samples = 0,
        .skip_ms = (duration_ms > 0) ? (uint32_t)((duration_ms * seek_percent) / 100U) : 0,
    };

    esp_err_t ret = music_codec_decode_auto(file_path, playback_pcm_cb, &ctx);
    if (ret != ESP_OK && ret != ESP_ERR_INVALID_STATE) {
        ESP_LOGW(TAG, "播放结束/失败: %s", esp_err_to_name(ret));
    }

    free(ctx.mix_buffer);

    if (xSemaphoreTake(g_state.lock, pdMS_TO_TICKS(50)) == pdTRUE) {
        g_state.playing = false;
        g_state.paused = false;
        g_state.stop_requested = false;
        if (ret == ESP_OK) {
            g_state.progress_percent = 100;
        }
        g_state.task = NULL;
        xSemaphoreGive(g_state.lock);
    }

    vTaskDelete(NULL);
}

esp_err_t music_player_set_selected_file(const char *filename)
{
    if (!filename || filename[0] == '\0') return ESP_ERR_INVALID_ARG;
    ensure_lock();

    if (xSemaphoreTake(g_state.lock, pdMS_TO_TICKS(100)) != pdTRUE) {
        return ESP_ERR_TIMEOUT;
    }

    if (strncmp(filename, SD_MOUNT_POINT "/", strlen(SD_MOUNT_POINT "/")) == 0) {
        strncpy(g_state.selected_file, filename, sizeof(g_state.selected_file) - 1);
    } else {
        snprintf(g_state.selected_file, sizeof(g_state.selected_file), SD_MOUNT_POINT "/%s", filename);
    }
    g_state.selected_file[sizeof(g_state.selected_file) - 1] = '\0';
    xSemaphoreGive(g_state.lock);

    return ESP_OK;
}

const char *music_player_get_selected_file(void)
{
    return g_state.selected_file;
}

void music_player_stop(void)
{
    ensure_lock();
    TaskHandle_t task = NULL;

    if (xSemaphoreTake(g_state.lock, pdMS_TO_TICKS(100)) == pdTRUE) {
        g_state.stop_requested = true;
        task = g_state.task;
        xSemaphoreGive(g_state.lock);
    }

    if (task) {
        for (int i = 0; i < 100; i++) {
            bool still_running = false;
            if (xSemaphoreTake(g_state.lock, pdMS_TO_TICKS(20)) == pdTRUE) {
                still_running = (g_state.task != NULL);
                xSemaphoreGive(g_state.lock);
            }
            if (!still_running) break;
            vTaskDelay(pdMS_TO_TICKS(10));
        }
    }
}

esp_err_t music_player_play_selected(void)
{
    ensure_lock();

    if (g_state.selected_file[0] == '\0') {
        ESP_LOGW(TAG, "未选择歌曲");
        return ESP_ERR_INVALID_STATE;
    }

    music_player_stop();

    if (xSemaphoreTake(g_state.lock, pdMS_TO_TICKS(100)) != pdTRUE) {
        return ESP_ERR_TIMEOUT;
    }
    g_state.stop_requested = false;
    g_state.progress_percent = 0;
    g_state.played_ms = 0;
    g_state.duration_ms = 0;
    xSemaphoreGive(g_state.lock);

    BaseType_t ok = xTaskCreate(player_task, "music_player", 8192, NULL, 5, &g_state.task);
    return ok == pdPASS ? ESP_OK : ESP_FAIL;
}

esp_err_t music_player_toggle_pause(void)
{
    ensure_lock();
    if (xSemaphoreTake(g_state.lock, pdMS_TO_TICKS(100)) != pdTRUE) {
        return ESP_ERR_TIMEOUT;
    }

    if (!g_state.playing) {
        xSemaphoreGive(g_state.lock);
        return music_player_play_selected();
    }

    g_state.paused = !g_state.paused;
    xSemaphoreGive(g_state.lock);
    return ESP_OK;
}

void music_player_set_volume(uint8_t volume_percent)
{
    if (volume_percent > 100) volume_percent = 100;
    ensure_lock();
    if (xSemaphoreTake(g_state.lock, pdMS_TO_TICKS(50)) == pdTRUE) {
        g_state.volume = volume_percent;
        xSemaphoreGive(g_state.lock);
    }
}

uint8_t music_player_get_volume(void)
{
    ensure_lock();
    if (xSemaphoreTake(g_state.lock, pdMS_TO_TICKS(50)) != pdTRUE) {
        return 80;
    }
    uint8_t v = g_state.volume;
    xSemaphoreGive(g_state.lock);
    return v;
}

uint8_t music_player_get_progress_percent(void)
{
    ensure_lock();
    if (xSemaphoreTake(g_state.lock, pdMS_TO_TICKS(50)) != pdTRUE) {
        return 0;
    }
    uint8_t p = g_state.progress_percent;
    xSemaphoreGive(g_state.lock);
    return p;
}

uint32_t music_player_get_duration_ms(void)
{
    ensure_lock();
    if (xSemaphoreTake(g_state.lock, pdMS_TO_TICKS(50)) != pdTRUE) return 0;
    uint32_t value = g_state.duration_ms;
    xSemaphoreGive(g_state.lock);
    return value;
}

uint32_t music_player_get_played_ms(void)
{
    ensure_lock();
    if (xSemaphoreTake(g_state.lock, pdMS_TO_TICKS(50)) != pdTRUE) return 0;
    uint32_t value = g_state.played_ms;
    xSemaphoreGive(g_state.lock);
    return value;
}

esp_err_t music_player_seek_percent(uint8_t percent)
{
    if (percent > 100) percent = 100;
    ensure_lock();

    if (xSemaphoreTake(g_state.lock, pdMS_TO_TICKS(100)) != pdTRUE) {
        return ESP_ERR_TIMEOUT;
    }

    g_state.seek_request_percent = percent;
    g_state.seek_pending = true;
    xSemaphoreGive(g_state.lock);

    return music_player_play_selected();
}

bool music_player_is_playing(void)
{
    ensure_lock();
    if (xSemaphoreTake(g_state.lock, pdMS_TO_TICKS(50)) != pdTRUE) {
        return false;
    }
    bool r = g_state.playing;
    xSemaphoreGive(g_state.lock);
    return r;
}

bool music_player_is_paused(void)
{
    ensure_lock();
    if (xSemaphoreTake(g_state.lock, pdMS_TO_TICKS(50)) != pdTRUE) {
        return false;
    }
    bool r = g_state.paused;
    xSemaphoreGive(g_state.lock);
    return r;
}
