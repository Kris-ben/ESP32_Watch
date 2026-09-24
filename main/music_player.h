#ifndef MUSIC_PLAYER_H
#define MUSIC_PLAYER_H

#include <stdbool.h>
#include <stdint.h>
#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

esp_err_t music_player_set_selected_file(const char *filename);
const char *music_player_get_selected_file(void);

esp_err_t music_player_play_selected(void);
esp_err_t music_player_toggle_pause(void);
void music_player_stop(void);

void music_player_set_volume(uint8_t volume_percent);
uint8_t music_player_get_volume(void);

uint8_t music_player_get_progress_percent(void);
uint32_t music_player_get_duration_ms(void);
uint32_t music_player_get_played_ms(void);
esp_err_t music_player_seek_percent(uint8_t percent);

bool music_player_is_playing(void);
bool music_player_is_paused(void);

#ifdef __cplusplus
}
#endif

#endif
