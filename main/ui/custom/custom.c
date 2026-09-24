/*
* Copyright 2024 NXP
* NXP Proprietary. This software is owned or controlled by NXP and may only be used strictly in
* accordance with the applicable license terms. By expressly accepting such terms or by downloading, installing,
* activating and/or otherwise using the software, you are agreeing that you have read, and that you agree to
* comply with and are bound by, such license terms.  If you do not agree to be bound by the applicable license
* terms, then you may not retain, install, activate or otherwise use the software.
*/


/*********************
 *      INCLUDES
 *********************/
#include <stdio.h>
#include <string.h>
#include "lvgl.h"
#include "custom.h"
#include "music_player.h"
#include "system_settings.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

/**********************
 *  STATIC VARIABLES
 **********************/

/**
 * Create a demo application
 */

void custom_init(lv_ui *ui)
{
    /* Add your codes here */
}

static lv_timer_t *g_play_ui_timer = NULL;

static void play_screen_deleted_cb(lv_event_t *e)
{
    (void)e;
    if (g_play_ui_timer) {
        lv_timer_del(g_play_ui_timer);
        g_play_ui_timer = NULL;
    }
}

static const char *get_list_button_text(lv_obj_t *btn)
{
    if (!btn) return NULL;
    lv_obj_t *txt = lv_obj_get_child(btn, 1);
    if (!txt) return NULL;
    return lv_label_get_text(txt);
}

static void musiclist_item_click_cb(lv_event_t *e)
{
    if (lv_event_get_code(e) != LV_EVENT_CLICKED) return;
    lv_obj_t *btn = lv_event_get_target(e);
    const char *name = get_list_button_text(btn);
    if (!name || name[0] == '\0' || strcmp(name, "-") == 0) {
        return;
    }
    if (strstr(name, ".mfl") == NULL && strstr(name, ".mgg") == NULL &&
        strstr(name, ".MFL") == NULL && strstr(name, ".MGG") == NULL &&
        strstr(name, ".mflac") == NULL && strstr(name, ".MFLAC") == NULL &&
        strstr(name, ".flac") == NULL && strstr(name, ".FLAC") == NULL &&
        strstr(name, ".ogg") == NULL && strstr(name, ".OGG") == NULL) {
        return;
    }
    music_player_set_selected_file(name);
}

void music_ui_bind_musiclist(lv_ui *ui)
{
    if (!ui) return;
    lv_obj_t *items[] = {
        ui->screen_musiclist_list_music_item0,
        ui->screen_musiclist_list_music_item1,
        ui->screen_musiclist_list_music_item2,
        ui->screen_musiclist_list_music_item3,
        ui->screen_musiclist_list_music_item4,
        ui->screen_musiclist_list_music_item5,
        ui->screen_musiclist_list_music_item6,
        ui->screen_musiclist_list_music_item7,
        ui->screen_musiclist_list_music_item8,
        ui->screen_musiclist_list_music_item9,
        ui->screen_musiclist_list_music_item10,
    };

    for (size_t i = 0; i < sizeof(items) / sizeof(items[0]); i++) {
        if (items[i]) {
            lv_obj_add_event_cb(items[i], musiclist_item_click_cb, LV_EVENT_CLICKED, ui);
        }
    }
}

static void update_play_icon(lv_obj_t *btn, bool paused)
{
    if (!btn) return;
    lv_obj_t *icon = lv_obj_get_child(btn, 0);
    if (icon) {
        /* child[0] of lv_list_add_button is an lv_image, NOT lv_label.
         * Use lv_image_set_src to update the symbol correctly. */
        lv_image_set_src(icon, paused ? LV_SYMBOL_PLAY : LV_SYMBOL_PAUSE);
    }
}

static void play_btn_click_cb(lv_event_t *e)
{
    if (lv_event_get_code(e) != LV_EVENT_CLICKED) return;
    lv_ui *ui = (lv_ui *)lv_event_get_user_data(e);
    if (!ui) return;

    music_player_toggle_pause();
    update_play_icon(ui->screen_play_list_play_item0, music_player_is_paused());
}

static void volume_slider_cb(lv_event_t *e)
{
    if (lv_event_get_code(e) != LV_EVENT_VALUE_CHANGED) return;
    lv_obj_t *slider = lv_event_get_target(e);
    int v = lv_slider_get_value(slider);
    if (v < 0) v = 0;
    if (v > 100) v = 100;
    (void)system_preview_volume((uint8_t)v);
}

static void volume_save_cb(lv_event_t *e)
{
    if (lv_event_get_code(e) != LV_EVENT_RELEASED) return;
    lv_obj_t *slider = lv_event_get_target(e);
    (void)system_set_volume((uint8_t)lv_slider_get_value(slider));
}

static void time_slider_cb(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    if (code != LV_EVENT_RELEASED && code != LV_EVENT_VALUE_CHANGED) return;
    lv_obj_t *slider = lv_event_get_target(e);
    int p = lv_slider_get_value(slider);
    if (p < 0) p = 0;
    if (p > 100) p = 100;

    if (code == LV_EVENT_RELEASED) {
        music_player_seek_percent((uint8_t)p);
    }
}

static void play_ui_timer_cb(lv_timer_t *timer)
{
    lv_ui *ui = (lv_ui *)lv_timer_get_user_data(timer);
    if (!ui || !ui->screen_play) return;
    if (!lv_obj_is_valid(ui->screen_play)) return;
    if (!ui->screen_play_slider_musictime || !lv_obj_is_valid(ui->screen_play_slider_musictime)) return;
    if (!ui->screen_play_list_play_item0 || !lv_obj_is_valid(ui->screen_play_list_play_item0)) return;

    uint8_t progress = music_player_get_progress_percent();
    lv_slider_set_value(ui->screen_play_slider_musictime, progress, LV_ANIM_OFF);
    update_play_icon(ui->screen_play_list_play_item0, music_player_is_paused());
}

void music_ui_bind_play(lv_ui *ui)
{
    if (!ui) return;

    lv_obj_add_event_cb(ui->screen_play, play_screen_deleted_cb, LV_EVENT_DELETE, NULL);

    lv_slider_set_value(ui->screen_play_slider_aduio, music_player_get_volume(), LV_ANIM_OFF);
    lv_slider_set_value(ui->screen_play_slider_musictime, music_player_get_progress_percent(), LV_ANIM_OFF);

    lv_obj_add_event_cb(ui->screen_play_list_play_item0, play_btn_click_cb, LV_EVENT_CLICKED, ui);
    lv_obj_add_event_cb(ui->screen_play_slider_aduio, volume_slider_cb, LV_EVENT_VALUE_CHANGED, ui);
    lv_obj_add_event_cb(ui->screen_play_slider_aduio, volume_save_cb, LV_EVENT_RELEASED, ui);
    lv_obj_add_event_cb(ui->screen_play_slider_musictime, time_slider_cb, LV_EVENT_VALUE_CHANGED, ui);
    lv_obj_add_event_cb(ui->screen_play_slider_musictime, time_slider_cb, LV_EVENT_RELEASED, ui);

    if (g_play_ui_timer) {
        lv_timer_del(g_play_ui_timer);
        g_play_ui_timer = NULL;
    }
    g_play_ui_timer = lv_timer_create(play_ui_timer_cb, 300, ui);

    music_player_play_selected();
    update_play_icon(ui->screen_play_list_play_item0, music_player_is_paused());
}
