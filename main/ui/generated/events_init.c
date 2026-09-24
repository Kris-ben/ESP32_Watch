/*
* Copyright 2026 NXP
* NXP Proprietary. This software is owned or controlled by NXP and may only be used strictly in
* accordance with the applicable license terms. By expressly accepting such terms or by downloading, installing,
* activating and/or otherwise using the software, you are agreeing that you have read, and that you agree to
* comply with and are bound by, such license terms.  If you do not agree to be bound by the applicable license
* terms, then you may not retain, install, activate or otherwise use the software.
*/

#include "events_init.h"
#include <stdio.h>
#include "lvgl.h"
#include "wifi_connect.h"
#include "alarm_clock.h"
#include "app_swipe_nav.h"
#include "app_page_layout.h"
#include "alarm_set_page.h"
#include "alarm_list_page.h"
#include "calendar_page.h"
#include "quick_settings_brightness.h"
#include "wifi_connect_page.h"
#include "wifi_scan_page.h"

#if LV_USE_GUIDER_SIMULATOR && LV_USE_FREEMASTER
#include "freemaster_client.h"
#endif


static void screen_home_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_GESTURE:
    {
        lv_dir_t dir = lv_indev_get_gesture_dir(lv_indev_active());
        switch(dir) {
        case LV_DIR_LEFT:
        {
            lv_indev_wait_release(lv_indev_active());
            ui_load_scr_animation(&guider_ui, &guider_ui.screen_1, guider_ui.screen_1_del, &guider_ui.screen_home_del, setup_scr_screen_1, LV_SCR_LOAD_ANIM_MOVE_LEFT, 200, 200, true, true);
            break;
        }
        default:
            break;
        }
        break;
    }
    default:
        break;
    }
}

void events_init_screen_home (lv_ui *ui)
{
    // 首页用左右/上下手势切页，滚动容器会优先消耗这些手势。
    lv_obj_remove_flag(ui->screen_home, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_remove_flag(ui->screen_home_list_bettery, LV_OBJ_FLAG_SCROLLABLE | LV_OBJ_FLAG_CLICKABLE);
    lv_obj_remove_flag(ui->screen_home_list_bettery_item0, LV_OBJ_FLAG_CLICKABLE);
    // 首页圆弧只显示传感器数据，不允许滑动时被当作可调节控件。
    lv_obj_remove_flag(ui->screen_home_arc_temp, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_remove_flag(ui->screen_home_arc_heart_rate, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_remove_flag(ui->screen_home_arc_humidity, LV_OBJ_FLAG_CLICKABLE);
    // 图标仍可点击；在图标上滑动时把手势继续交给首页。
    lv_obj_add_flag(ui->screen_home_img_wifi, LV_OBJ_FLAG_GESTURE_BUBBLE);
    lv_obj_add_flag(ui->screen_home_img_foot, LV_OBJ_FLAG_GESTURE_BUBBLE);
    lv_obj_add_flag(ui->screen_home_img_pa, LV_OBJ_FLAG_GESTURE_BUBBLE);
    lv_obj_add_event_cb(ui->screen_home, screen_home_event_handler, LV_EVENT_ALL, ui);
}

static void screen_selete_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_GESTURE:
    {
        lv_dir_t dir = lv_indev_get_gesture_dir(lv_indev_active());
        switch(dir) {
        case LV_DIR_RIGHT:
        {
            lv_indev_wait_release(lv_indev_active());
            ui_load_scr_animation(&guider_ui, &guider_ui.screen_home, guider_ui.screen_home_del, &guider_ui.screen_selete_del, setup_scr_screen_home, LV_SCR_LOAD_ANIM_MOVE_RIGHT, 200, 200, true, true);
            break;
        }
        default:
            break;
        }
        break;
    }
    default:
        break;
    }
}

static void screen_selete_imgbtn_wifi_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_CLICKED:
    {
        ui_load_scr_animation(&guider_ui, &guider_ui.screen_wifi, guider_ui.screen_wifi_del, &guider_ui.screen_selete_del, setup_scr_screen_wifi, LV_SCR_LOAD_ANIM_NONE, 200, 200, true, true);
        break;
    }
    default:
        break;
    }
}

static void screen_selete_imgbtn_weather_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_CLICKED:
    {
        ui_load_scr_animation(&guider_ui, &guider_ui.screen_weather, guider_ui.screen_weather_del, &guider_ui.screen_selete_del, setup_scr_screen_weather, LV_SCR_LOAD_ANIM_FADE_ON, 200, 200, true, true);
        break;
    }
    default:
        break;
    }
}

static void screen_selete_imgbtn_rli_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_CLICKED:
    {
        ui_load_scr_animation(&guider_ui, &guider_ui.screen_Rli, guider_ui.screen_Rli_del, &guider_ui.screen_selete_del, setup_scr_screen_Rli, LV_SCR_LOAD_ANIM_FADE_ON, 200, 200, true, true);
        break;
    }
    default:
        break;
    }
}

static void screen_selete_imgbtn_AI_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_CLICKED:
    {
        ui_load_scr_animation(&guider_ui, &guider_ui.screen_AI, guider_ui.screen_AI_del, &guider_ui.screen_selete_del, setup_scr_screen_AI, LV_SCR_LOAD_ANIM_FADE_ON, 200, 200, true, true);
        break;
    }
    default:
        break;
    }
}

static void screen_selete_imgbtn_clock_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_CLICKED:
    {
        ui_load_scr_animation(&guider_ui, &guider_ui.screen_clock, guider_ui.screen_clock_del, &guider_ui.screen_selete_del, setup_scr_screen_clock, LV_SCR_LOAD_ANIM_FADE_ON, 200, 200, true, true);
        break;
    }
    default:
        break;
    }
}

void events_init_screen_selete (lv_ui *ui)
{
    lv_obj_add_event_cb(ui->screen_selete, screen_selete_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->screen_selete_imgbtn_wifi, screen_selete_imgbtn_wifi_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->screen_selete_imgbtn_weather, screen_selete_imgbtn_weather_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->screen_selete_imgbtn_rli, screen_selete_imgbtn_rli_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->screen_selete_imgbtn_AI, screen_selete_imgbtn_AI_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->screen_selete_imgbtn_clock, screen_selete_imgbtn_clock_event_handler, LV_EVENT_ALL, ui);
}

static void screen_Rli_btn_return_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_CLICKED:
    {
        ui_load_scr_animation(&guider_ui, &guider_ui.screen_1, guider_ui.screen_1_del, &guider_ui.screen_Rli_del, setup_scr_screen_1, LV_SCR_LOAD_ANIM_FADE_ON, 200, 200, true, true);
        break;
    }
    default:
        break;
    }
}

void events_init_screen_Rli (lv_ui *ui)
{
    lv_obj_add_event_cb(ui->screen_Rli_btn_return, screen_Rli_btn_return_event_handler, LV_EVENT_ALL, ui);
    calendar_page_init(ui);
    app_swipe_nav_bind(ui->screen_Rli, ui->screen_Rli_btn_return, APP_SWIPE_TO_LAUNCHER);
}

static void screen_AI_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_GESTURE:
    {
        lv_dir_t dir = lv_indev_get_gesture_dir(lv_indev_active());
        switch(dir) {
        default:
            break;
        }
        break;
    }
    default:
        break;
    }
}

static void screen_AI_img_ai_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_LONG_PRESSED_REPEAT:
    {
        break;
    }
    default:
        break;
    }
}

static void screen_AI_imgbtn_return_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_CLICKED:
    {
        ui_load_scr_animation(&guider_ui, &guider_ui.screen_1, guider_ui.screen_1_del, &guider_ui.screen_AI_del, setup_scr_screen_1, LV_SCR_LOAD_ANIM_FADE_ON, 200, 200, false, true);
        break;
    }
    default:
        break;
    }
}

void events_init_screen_AI (lv_ui *ui)
{
    lv_obj_add_event_cb(ui->screen_AI, screen_AI_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->screen_AI_img_ai, screen_AI_img_ai_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->screen_AI_imgbtn_return, screen_AI_imgbtn_return_event_handler, LV_EVENT_ALL, ui);
    app_page_layout_apply(ui, APP_LAYOUT_AI);
    app_swipe_nav_bind(ui->screen_AI, ui->screen_AI_imgbtn_return, APP_SWIPE_TO_LAUNCHER);
    wifi_connect_update_icon_nolock(ui, wifi_connect_is_connected());
}

static void screen_wifi_list_wifi_item0_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_CLICKED:
    {
        if (!wifi_connect_set_selected_index(0)) break;
        ui_load_scr_animation(&guider_ui, &guider_ui.screen_wifi_connect, guider_ui.screen_wifi_connect_del, &guider_ui.screen_wifi_del, setup_scr_screen_wifi_connect, LV_SCR_LOAD_ANIM_FADE_ON, 200, 200, true, true);
        break;
    }
    default:
        break;
    }
}

static void screen_wifi_list_wifi_item1_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_CLICKED:
    {
        if (!wifi_connect_set_selected_index(1)) break;
        ui_load_scr_animation(&guider_ui, &guider_ui.screen_wifi_connect, guider_ui.screen_wifi_connect_del, &guider_ui.screen_wifi_del, setup_scr_screen_wifi_connect, LV_SCR_LOAD_ANIM_FADE_ON, 200, 200, true, true);
        break;
    }
    default:
        break;
    }
}

static void screen_wifi_list_wifi_item2_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_CLICKED:
    {
        if (!wifi_connect_set_selected_index(2)) break;
        ui_load_scr_animation(&guider_ui, &guider_ui.screen_wifi_connect, guider_ui.screen_wifi_connect_del, &guider_ui.screen_wifi_del, setup_scr_screen_wifi_connect, LV_SCR_LOAD_ANIM_FADE_ON, 200, 200, true, true);
        break;
    }
    default:
        break;
    }
}

static void screen_wifi_list_wifi_item3_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_CLICKED:
    {
        if (!wifi_connect_set_selected_index(3)) break;
        ui_load_scr_animation(&guider_ui, &guider_ui.screen_wifi_connect, guider_ui.screen_wifi_connect_del, &guider_ui.screen_wifi_del, setup_scr_screen_wifi_connect, LV_SCR_LOAD_ANIM_FADE_ON, 200, 200, true, true);
        break;
    }
    default:
        break;
    }
}

static void screen_wifi_list_wifi_item4_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_CLICKED:
    {
        if (!wifi_connect_set_selected_index(4)) break;
        ui_load_scr_animation(&guider_ui, &guider_ui.screen_wifi_connect, guider_ui.screen_wifi_connect_del, &guider_ui.screen_wifi_del, setup_scr_screen_wifi_connect, LV_SCR_LOAD_ANIM_FADE_ON, 200, 200, true, true);
        break;
    }
    default:
        break;
    }
}

static void screen_wifi_btn_return_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_CLICKED:
    {
        ui_load_scr_animation(&guider_ui, &guider_ui.screen_1, guider_ui.screen_1_del, &guider_ui.screen_wifi_del, setup_scr_screen_1, LV_SCR_LOAD_ANIM_FADE_ON, 200, 200, true, true);
        break;
    }
    default:
        break;
    }
}

void events_init_screen_wifi (lv_ui *ui)
{
    // 注册扫描按钮并更新WiFi状态图标；列表项由本文件先校验SSID再切换页面。
    wifi_connect_register_wifi_events(ui);
    lv_obj_add_event_cb(ui->screen_wifi_list_wifi_item0, screen_wifi_list_wifi_item0_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->screen_wifi_list_wifi_item1, screen_wifi_list_wifi_item1_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->screen_wifi_list_wifi_item2, screen_wifi_list_wifi_item2_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->screen_wifi_list_wifi_item3, screen_wifi_list_wifi_item3_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->screen_wifi_list_wifi_item4, screen_wifi_list_wifi_item4_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->screen_wifi_btn_return, screen_wifi_btn_return_event_handler, LV_EVENT_ALL, ui);
    app_page_layout_apply(ui, APP_LAYOUT_WIFI);
    app_swipe_nav_bind(ui->screen_wifi, ui->screen_wifi_btn_return, APP_SWIPE_TO_LAUNCHER);
    wifi_scan_page_init(ui);
}

void events_init_screen_wifi_connect (lv_ui *ui)
{
    wifi_connect_register_connect_events(ui);
    app_page_layout_apply(ui, APP_LAYOUT_WIFI_CONNECT);
    wifi_connect_page_init(ui);
    app_swipe_nav_bind(ui->screen_wifi_connect, NULL, APP_SWIPE_TO_WIFI);
}

static void screen_weather_btn_return_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_CLICKED:
    {
        ui_load_scr_animation(&guider_ui, &guider_ui.screen_1, guider_ui.screen_1_del, &guider_ui.screen_weather_del, setup_scr_screen_1, LV_SCR_LOAD_ANIM_FADE_ON, 200, 200, true, true);
        break;
    }
    default:
        break;
    }
}

void events_init_screen_weather (lv_ui *ui)
{
    lv_obj_add_event_cb(ui->screen_weather_btn_return, screen_weather_btn_return_event_handler, LV_EVENT_ALL, ui);
    app_page_layout_apply(ui, APP_LAYOUT_WEATHER);
    app_swipe_nav_bind(ui->screen_weather, ui->screen_weather_btn_return, APP_SWIPE_TO_LAUNCHER);
}

static void screen_down_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_GESTURE:
    {
        lv_dir_t dir = lv_indev_get_gesture_dir(lv_indev_active());
        switch(dir) {
        case LV_DIR_TOP:
        {
            lv_indev_wait_release(lv_indev_active());
            ui_load_scr_animation(&guider_ui, &guider_ui.screen_home, guider_ui.screen_home_del, &guider_ui.screen_down_del, setup_scr_screen_home, LV_SCR_LOAD_ANIM_MOVE_TOP, 200, 200, true, true);
            break;
        }
        default:
            break;
        }
        break;
    }
    default:
        break;
    }
}

static void screen_down_img_wifi_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_LONG_PRESSED_REPEAT:
    {
        ui_load_scr_animation(&guider_ui, &guider_ui.screen_wifi, guider_ui.screen_wifi_del, &guider_ui.screen_down_del, setup_scr_screen_wifi, LV_SCR_LOAD_ANIM_FADE_ON, 200, 200, true, true);
        break;
    }
    default:
        break;
    }
}

static void screen_down_img_musci_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_LONG_PRESSED_REPEAT:
    {
        ui_load_scr_animation(&guider_ui, &guider_ui.screen_musiclist, guider_ui.screen_musiclist_del, &guider_ui.screen_down_del, setup_scr_screen_musiclist, LV_SCR_LOAD_ANIM_FADE_ON, 200, 200, true, true);
        break;
    }
    default:
        break;
    }
}

void events_init_screen_down (lv_ui *ui)
{
    lv_obj_add_event_cb(ui->screen_down, screen_down_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->screen_down_img_wifi, screen_down_img_wifi_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->screen_down_img_musci, screen_down_img_musci_event_handler, LV_EVENT_ALL, ui);
    quick_settings_brightness_init(ui);
}

static void screen_set_clock_btn_return_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_CLICKED:
    {
        ui_load_scr_animation(&guider_ui, &guider_ui.screen_clock, guider_ui.screen_clock_del, &guider_ui.screen_set_clock_del, setup_scr_screen_clock, LV_SCR_LOAD_ANIM_FADE_ON, 200, 200, true, true);
        break;
    }
    default:
        break;
    }
}

void events_init_screen_set_clock (lv_ui *ui)
{
    lv_obj_add_event_cb(ui->screen_set_clock_btn_return, screen_set_clock_btn_return_event_handler, LV_EVENT_ALL, ui);
    app_page_layout_apply(ui, APP_LAYOUT_SET_CLOCK);
    alarm_set_page_init(ui);
    app_swipe_nav_bind(ui->screen_set_clock, ui->screen_set_clock_btn_return, APP_SWIPE_TO_CLOCK);
}

static void screen_clock_btn_return_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_CLICKED:
    {
        ui_load_scr_animation(&guider_ui, &guider_ui.screen_1, guider_ui.screen_1_del, &guider_ui.screen_clock_del, setup_scr_screen_1, LV_SCR_LOAD_ANIM_FADE_ON, 200, 200, true, true);
        break;
    }
    default:
        break;
    }
}

void events_init_screen_clock (lv_ui *ui)
{
    lv_obj_add_event_cb(ui->screen_clock_btn_return, screen_clock_btn_return_event_handler, LV_EVENT_ALL, ui);
    app_page_layout_apply(ui, APP_LAYOUT_CLOCK);
    alarm_list_page_init(ui);
    app_swipe_nav_bind(ui->screen_clock, ui->screen_clock_btn_return, APP_SWIPE_TO_LAUNCHER);
}

static void screen_1_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_GESTURE:
    {
        lv_dir_t dir = lv_indev_get_gesture_dir(lv_indev_active());
        switch(dir) {
        case LV_DIR_RIGHT:
        {
            lv_indev_wait_release(lv_indev_active());
            ui_load_scr_animation(&guider_ui, &guider_ui.screen_home, guider_ui.screen_home_del, &guider_ui.screen_1_del, setup_scr_screen_home, LV_SCR_LOAD_ANIM_OVER_RIGHT, 200, 200, true, true);
            break;
        }
        default:
            break;
        }
        break;
    }
    default:
        break;
    }
}

void events_init_screen_1 (lv_ui *ui)
{
    lv_obj_add_event_cb(ui->screen_1, screen_1_event_handler, LV_EVENT_ALL, ui);
}

static void screen_musiclist_list_music_item0_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_CLICKED:
    {
        ui_load_scr_animation(&guider_ui, &guider_ui.screen_play, guider_ui.screen_play_del, &guider_ui.screen_musiclist_del, setup_scr_screen_play, LV_SCR_LOAD_ANIM_FADE_ON, 200, 200, false, true);
        break;
    }
    default:
        break;
    }
}

static void screen_musiclist_list_music_item1_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_CLICKED:
    {
        ui_load_scr_animation(&guider_ui, &guider_ui.screen_play, guider_ui.screen_play_del, &guider_ui.screen_musiclist_del, setup_scr_screen_play, LV_SCR_LOAD_ANIM_FADE_ON, 200, 200, false, true);
        break;
    }
    default:
        break;
    }
}

static void screen_musiclist_list_music_item2_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_CLICKED:
    {
        ui_load_scr_animation(&guider_ui, &guider_ui.screen_play, guider_ui.screen_play_del, &guider_ui.screen_musiclist_del, setup_scr_screen_play, LV_SCR_LOAD_ANIM_FADE_ON, 200, 200, false, true);
        break;
    }
    default:
        break;
    }
}

static void screen_musiclist_list_music_item3_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_CLICKED:
    {
        ui_load_scr_animation(&guider_ui, &guider_ui.screen_play, guider_ui.screen_play_del, &guider_ui.screen_musiclist_del, setup_scr_screen_play, LV_SCR_LOAD_ANIM_FADE_ON, 200, 200, false, true);
        break;
    }
    default:
        break;
    }
}

static void screen_musiclist_list_music_item4_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_CLICKED:
    {
        ui_load_scr_animation(&guider_ui, &guider_ui.screen_play, guider_ui.screen_play_del, &guider_ui.screen_musiclist_del, setup_scr_screen_play, LV_SCR_LOAD_ANIM_FADE_ON, 200, 200, false, true);
        break;
    }
    default:
        break;
    }
}

static void screen_musiclist_list_music_item5_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_CLICKED:
    {
        ui_load_scr_animation(&guider_ui, &guider_ui.screen_play, guider_ui.screen_play_del, &guider_ui.screen_musiclist_del, setup_scr_screen_play, LV_SCR_LOAD_ANIM_FADE_ON, 200, 200, false, true);
        break;
    }
    default:
        break;
    }
}

static void screen_musiclist_list_music_item6_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_CLICKED:
    {
        ui_load_scr_animation(&guider_ui, &guider_ui.screen_play, guider_ui.screen_play_del, &guider_ui.screen_musiclist_del, setup_scr_screen_play, LV_SCR_LOAD_ANIM_FADE_ON, 200, 200, false, true);
        break;
    }
    default:
        break;
    }
}

static void screen_musiclist_list_music_item7_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_CLICKED:
    {
        ui_load_scr_animation(&guider_ui, &guider_ui.screen_play, guider_ui.screen_play_del, &guider_ui.screen_musiclist_del, setup_scr_screen_play, LV_SCR_LOAD_ANIM_FADE_ON, 200, 200, false, true);
        break;
    }
    default:
        break;
    }
}

static void screen_musiclist_list_music_item8_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_CLICKED:
    {
        ui_load_scr_animation(&guider_ui, &guider_ui.screen_play, guider_ui.screen_play_del, &guider_ui.screen_musiclist_del, setup_scr_screen_play, LV_SCR_LOAD_ANIM_FADE_ON, 200, 200, false, true);
        break;
    }
    default:
        break;
    }
}

static void screen_musiclist_list_music_item9_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_CLICKED:
    {
        ui_load_scr_animation(&guider_ui, &guider_ui.screen_play, guider_ui.screen_play_del, &guider_ui.screen_musiclist_del, setup_scr_screen_play, LV_SCR_LOAD_ANIM_FADE_ON, 200, 200, false, true);
        break;
    }
    default:
        break;
    }
}

static void screen_musiclist_list_music_item10_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_CLICKED:
    {
        ui_load_scr_animation(&guider_ui, &guider_ui.screen_play, guider_ui.screen_play_del, &guider_ui.screen_musiclist_del, setup_scr_screen_play, LV_SCR_LOAD_ANIM_FADE_ON, 200, 200, false, true);
        break;
    }
    default:
        break;
    }
}

static void screen_musiclist_btn_return_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_CLICKED:
    {
        ui_load_scr_animation(&guider_ui, &guider_ui.screen_1, guider_ui.screen_1_del, &guider_ui.screen_musiclist_del, setup_scr_screen_1, LV_SCR_LOAD_ANIM_FADE_ON, 200, 200, true, true);
        break;
    }
    default:
        break;
    }
}

void events_init_screen_musiclist (lv_ui *ui)
{
    lv_obj_add_event_cb(ui->screen_musiclist_list_music_item0, screen_musiclist_list_music_item0_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->screen_musiclist_list_music_item1, screen_musiclist_list_music_item1_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->screen_musiclist_list_music_item2, screen_musiclist_list_music_item2_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->screen_musiclist_list_music_item3, screen_musiclist_list_music_item3_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->screen_musiclist_list_music_item4, screen_musiclist_list_music_item4_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->screen_musiclist_list_music_item5, screen_musiclist_list_music_item5_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->screen_musiclist_list_music_item6, screen_musiclist_list_music_item6_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->screen_musiclist_list_music_item7, screen_musiclist_list_music_item7_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->screen_musiclist_list_music_item8, screen_musiclist_list_music_item8_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->screen_musiclist_list_music_item9, screen_musiclist_list_music_item9_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->screen_musiclist_list_music_item10, screen_musiclist_list_music_item10_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->screen_musiclist_btn_return, screen_musiclist_btn_return_event_handler, LV_EVENT_ALL, ui);
    app_page_layout_apply(ui, APP_LAYOUT_MUSICLIST);
    app_swipe_nav_bind(ui->screen_musiclist, ui->screen_musiclist_btn_return, APP_SWIPE_TO_LAUNCHER);
}

static void screen_play_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_GESTURE:
    {
        lv_dir_t dir = lv_indev_get_gesture_dir(lv_indev_active());
        switch(dir) {
        case LV_DIR_RIGHT:
        {
            lv_indev_wait_release(lv_indev_active());
            ui_load_scr_animation(&guider_ui, &guider_ui.screen_musiclist, guider_ui.screen_musiclist_del, &guider_ui.screen_play_del, setup_scr_screen_musiclist, LV_SCR_LOAD_ANIM_OVER_RIGHT, 200, 200, true, true);
            break;
        }
        default:
            break;
        }
        break;
    }
    default:
        break;
    }
}

void events_init_screen_play (lv_ui *ui)
{
    lv_obj_add_event_cb(ui->screen_play, screen_play_event_handler, LV_EVENT_ALL, ui);
}


void events_init(lv_ui *ui)
{

}
