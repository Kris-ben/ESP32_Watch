/*
* Copyright 2026 NXP
* NXP Proprietary. This software is owned or controlled by NXP and may only be used strictly in
* accordance with the applicable license terms. By expressly accepting such terms or by downloading, installing,
* activating and/or otherwise using the software, you are agreeing that you have read, and that you agree to
* comply with and are bound by, such license terms.  If you do not agree to be bound by the applicable license
* terms, then you may not retain, install, activate or otherwise use the software.
*/

#include "lvgl.h"
#include <stdio.h>
#include "gui_guider.h"
#include "widgets_init.h"
#include "quick_settings_page.h"

void ui_init_style(lv_style_t * style)
{
    if (style->prop_cnt > 1)
        lv_style_reset(style);
    else
        lv_style_init(style);
}

void ui_load_scr_animation(lv_ui *ui, lv_obj_t ** new_scr, bool new_scr_del, bool * old_scr_del, ui_setup_scr_t setup_scr,
                           lv_screen_load_anim_t anim_type, uint32_t time, uint32_t delay, bool is_clean, bool auto_del)
{
    (void)delay;
    (void)is_clean;

#if LV_USE_GUIDER_SIMULATOR && LV_USE_FREEMASTER
#include "gg_external_data.h"
    if(auto_del) {
        gg_edata_task_clear(lv_screen_active());
    }
#endif
    // 首页保留在内存中，隐藏期间时钟、WiFi 和电池状态仍会更新；返回时不再显示新建页面的占位值。
    bool keep_home = lv_screen_active() == ui->screen_home && new_scr != &ui->screen_home;
    if (new_scr_del) {
        setup_scr(ui);
    }
    if (new_scr == &ui->screen_home) {
        ui->screen_home_del = false;
    }
    bool delete_old = auto_del && !keep_home;
    // 页面不移动；旧页先隐藏到纯色底层，新页再淡入，避免拉伸拖影和两页叠影。
    uint32_t transition_ms = anim_type == LV_SCR_LOAD_ANIM_NONE ? 0 : (time > 120 ? 120 : time);
    lv_obj_t *old_scr = lv_screen_active();
    // 延后 1 ms 开始，等当前触摸事件回调结束后再切换或删除旧页面。
    lv_screen_load_anim(*new_scr,
                        transition_ms ? LV_SCR_LOAD_ANIM_FADE_ON : LV_SCR_LOAD_ANIM_NONE,
                        transition_ms, 1, delete_old);
    if (transition_ms > 0 && old_scr && old_scr != *new_scr && lv_obj_is_valid(old_scr)) {
        lv_obj_set_style_opa(old_scr, LV_OPA_TRANSP, 0);
    }
    // 兼容调用方传 NULL 的情况：往空指针写会让芯片直接异常重启（黑屏）
    if (old_scr_del) {
        *old_scr_del = delete_old;
    }
}

void ui_animation(void * var, uint32_t duration, int32_t delay, int32_t start_value, int32_t end_value, lv_anim_path_cb_t path_cb,
                  uint32_t repeat_cnt, uint32_t repeat_delay, uint32_t playback_time, uint32_t playback_delay,
                  lv_anim_exec_xcb_t exec_cb, lv_anim_start_cb_t start_cb, lv_anim_completed_cb_t ready_cb, lv_anim_deleted_cb_t deleted_cb)
{
    lv_anim_t anim;
    lv_anim_init(&anim);
    lv_anim_set_var(&anim, var);
    lv_anim_set_exec_cb(&anim, exec_cb);
    lv_anim_set_values(&anim, start_value, end_value);
    lv_anim_set_time(&anim, duration);
    lv_anim_set_delay(&anim, delay);
    lv_anim_set_path_cb(&anim, path_cb);
    lv_anim_set_repeat_count(&anim, repeat_cnt);
    lv_anim_set_repeat_delay(&anim, repeat_delay);
    lv_anim_set_playback_time(&anim, playback_time);
    lv_anim_set_playback_delay(&anim, playback_delay);
    if (start_cb) {
        lv_anim_set_start_cb(&anim, start_cb);
    }
    if (ready_cb) {
        lv_anim_set_completed_cb(&anim, ready_cb);
    }
    if (deleted_cb) {
        lv_anim_set_deleted_cb(&anim, deleted_cb);
    }
    lv_anim_start(&anim);
}

void init_scr_del_flag(lv_ui *ui)
{

    ui->screen_home_del = true;
    ui->screen_selete_del = true;
    ui->screen_Rli_del = true;
    ui->screen_AI_del = true;
    ui->screen_wifi_del = true;
    ui->screen_wifi_connect_del = true;
    ui->screen_weather_del = true;
    ui->screen_down_del = true;
    ui->screen_set_clock_del = true;
    ui->screen_clock_del = true;
    ui->screen_1_del = true;
    ui->screen_musiclist_del = true;
    ui->screen_play_del = true;
}

void setup_bottom_layer(void)
{
    lv_theme_apply(lv_layer_bottom());
    lv_obj_set_style_bg_color(lv_layer_bottom(), lv_color_hex(0x010101), 0);
    lv_obj_set_style_bg_opa(lv_layer_bottom(), LV_OPA_COVER, 0);
}

void setup_ui(lv_ui *ui)
{
    setup_bottom_layer();
    init_scr_del_flag(ui);
    init_keyboard(ui);
    setup_scr_screen_home(ui);
    ui->screen_home_del = false;
    lv_screen_load(ui->screen_home);
    quick_settings_page_bind(ui);
}

void video_play(lv_ui *ui)
{

}

void init_keyboard(lv_ui *ui)
{
    ui->g_kb_top_layer = lv_keyboard_create(lv_layer_top());
    lv_obj_add_event_cb(ui->g_kb_top_layer, kb_event_cb, LV_EVENT_ALL, NULL);
    lv_obj_add_flag(ui->g_kb_top_layer, LV_OBJ_FLAG_HIDDEN);
    lv_obj_set_style_text_font(ui->g_kb_top_layer, &lv_font_SourceHanSerifSC_Regular_18, LV_PART_MAIN|LV_STATE_DEFAULT);
}
