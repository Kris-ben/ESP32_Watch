#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct local_wake_word local_wake_word_t;

/* 使用 flash 中的“你好小智”模型创建离线唤醒器。输入为 16 kHz 单声道 PCM。 */
esp_err_t local_wake_word_create(local_wake_word_t **out);
bool local_wake_word_process(local_wake_word_t *wake, const int16_t *pcm, size_t samples);
void local_wake_word_reset(local_wake_word_t *wake);
void local_wake_word_destroy(local_wake_word_t *wake);

#ifdef __cplusplus
}
#endif
