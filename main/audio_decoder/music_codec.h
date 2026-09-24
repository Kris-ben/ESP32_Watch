#ifndef MUSIC_CODEC_H
#define MUSIC_CODEC_H

#include "esp_err.h"
#include <stdint.h>
#include <stddef.h>
#include "max98357a.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    MUSIC_CODEC_UNKNOWN = 0,
    MUSIC_CODEC_MFLAC,
    MUSIC_CODEC_MGG,
} music_codec_type_t;

typedef esp_err_t (*music_pcm_cb_t)(const int16_t *pcm_interleaved,
                                    size_t frames,
                                    int channels,
                                    int sample_rate,
                                    void *user_data);

music_codec_type_t music_codec_detect(const char *path);

esp_err_t music_codec_decode_mflac(const char *path, music_pcm_cb_t callback, void *user_data);
esp_err_t music_codec_decode_mgg(const char *path, music_pcm_cb_t callback, void *user_data);
esp_err_t music_codec_decode_auto(const char *path, music_pcm_cb_t callback, void *user_data);
esp_err_t music_codec_get_duration_ms(const char *path, uint32_t *out_duration_ms);

esp_err_t music_codec_play_file(const char *path, max98357a_handle_t *speaker);

#ifdef __cplusplus
}
#endif

#endif
