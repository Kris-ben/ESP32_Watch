/*
* Copyright 2026 NXP
* NXP Proprietary. This software is owned or controlled by NXP and may only be used strictly in
* accordance with the applicable license terms. By expressly accepting such terms or by downloading, installing,
* activating and/or otherwise using the software, you are agreeing that you have read, and that you agree to
* comply with and are bound by, such license terms.  If you do not agree to be bound by the applicable license
* terms, then you may not retain, install, activate or otherwise use the software.
*/

#include "app_theme.h"
#include "lvgl.h"
#include <stdio.h>
#include "gui_guider.h"
#include "events_init.h"
#include "widgets_init.h"
#include "custom.h"



void setup_scr_screen_wifi_connect(lv_ui *ui)
{
    //Write codes screen_wifi_connect
    ui->screen_wifi_connect = lv_obj_create(NULL);
    lv_obj_set_size(ui->screen_wifi_connect, 240, 284);
    lv_obj_set_scrollbar_mode(ui->screen_wifi_connect, LV_SCROLLBAR_MODE_OFF);

    //Write style for screen_wifi_connect, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_wifi_connect, 248, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_wifi_connect, lv_color_hex(APP_THEME_BG), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_wifi_connect, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_wifi_connect_ta_input
    ui->screen_wifi_connect_ta_input = lv_textarea_create(ui->screen_wifi_connect);
    lv_obj_set_pos(ui->screen_wifi_connect_ta_input, 1, 96);
    lv_obj_set_size(ui->screen_wifi_connect_ta_input, 236, 83);
    lv_textarea_set_text(ui->screen_wifi_connect_ta_input, "请输入密码");
    lv_textarea_set_placeholder_text(ui->screen_wifi_connect_ta_input, "");
    lv_textarea_set_password_bullet(ui->screen_wifi_connect_ta_input, "*");
    lv_textarea_set_password_mode(ui->screen_wifi_connect_ta_input, false);
    lv_textarea_set_one_line(ui->screen_wifi_connect_ta_input, false);
    lv_textarea_set_accepted_chars(ui->screen_wifi_connect_ta_input, "");
    lv_textarea_set_max_length(ui->screen_wifi_connect_ta_input, 32);
#if LV_USE_KEYBOARD
    lv_obj_add_event_cb(ui->screen_wifi_connect_ta_input, ta_event_cb, LV_EVENT_ALL, ui->g_kb_top_layer);
#endif

    //Write style for screen_wifi_connect_ta_input, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_text_color(ui->screen_wifi_connect_ta_input, lv_color_hex(APP_THEME_BG), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_wifi_connect_ta_input, &lv_font_ZiTiQuanWeiJunHeiW22_24, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_wifi_connect_ta_input, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->screen_wifi_connect_ta_input, 2, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_wifi_connect_ta_input, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_wifi_connect_ta_input, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_wifi_connect_ta_input, lv_color_hex(APP_THEME_CARD), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_wifi_connect_ta_input, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->screen_wifi_connect_ta_input, 2, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(ui->screen_wifi_connect_ta_input, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(ui->screen_wifi_connect_ta_input, lv_color_hex(APP_THEME_TEXT), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_side(ui->screen_wifi_connect_ta_input, LV_BORDER_SIDE_FULL, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_wifi_connect_ta_input, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_wifi_connect_ta_input, 4, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_wifi_connect_ta_input, 4, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_wifi_connect_ta_input, 4, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_wifi_connect_ta_input, 4, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style for screen_wifi_connect_ta_input, Part: LV_PART_SCROLLBAR, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_wifi_connect_ta_input, 255, LV_PART_SCROLLBAR|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_wifi_connect_ta_input, lv_color_hex(APP_THEME_CARD), LV_PART_SCROLLBAR|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_wifi_connect_ta_input, LV_GRAD_DIR_NONE, LV_PART_SCROLLBAR|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_wifi_connect_ta_input, 0, LV_PART_SCROLLBAR|LV_STATE_DEFAULT);

    //Write codes screen_wifi_connect_label_connect
    ui->screen_wifi_connect_label_connect = lv_label_create(ui->screen_wifi_connect);
    lv_obj_set_pos(ui->screen_wifi_connect_label_connect, 64, 37);
    lv_obj_set_size(ui->screen_wifi_connect_label_connect, 104, 19);
    lv_label_set_text(ui->screen_wifi_connect_label_connect, "连接到");
    lv_label_set_long_mode(ui->screen_wifi_connect_label_connect, LV_LABEL_LONG_WRAP);

    //Write style for screen_wifi_connect_label_connect, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_wifi_connect_label_connect, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_wifi_connect_label_connect, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_wifi_connect_label_connect, lv_color_hex(APP_THEME_TEXT), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_wifi_connect_label_connect, &lv_font_ZiTiQuanWeiJunHeiW22_18, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_wifi_connect_label_connect, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->screen_wifi_connect_label_connect, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->screen_wifi_connect_label_connect, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_wifi_connect_label_connect, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_wifi_connect_label_connect, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_wifi_connect_label_connect, lv_color_hex(APP_THEME_BG), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_wifi_connect_label_connect, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_wifi_connect_label_connect, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_wifi_connect_label_connect, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_wifi_connect_label_connect, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_wifi_connect_label_connect, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_wifi_connect_label_connect, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_wifi_connect_btn_set
    ui->screen_wifi_connect_btn_set = lv_button_create(ui->screen_wifi_connect);
    lv_obj_set_pos(ui->screen_wifi_connect_btn_set, 77, 204);
    lv_obj_set_size(ui->screen_wifi_connect_btn_set, 77, 27);
    ui->screen_wifi_connect_btn_set_label = lv_label_create(ui->screen_wifi_connect_btn_set);
    lv_label_set_text(ui->screen_wifi_connect_btn_set_label, "确定");
    lv_label_set_long_mode(ui->screen_wifi_connect_btn_set_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(ui->screen_wifi_connect_btn_set_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(ui->screen_wifi_connect_btn_set, 0, LV_STATE_DEFAULT);
    lv_obj_set_width(ui->screen_wifi_connect_btn_set_label, LV_PCT(100));

    //Write style for screen_wifi_connect_btn_set, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_wifi_connect_btn_set, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_wifi_connect_btn_set, lv_color_hex(APP_THEME_ACCENT), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_wifi_connect_btn_set, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->screen_wifi_connect_btn_set, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_wifi_connect_btn_set, 5, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_wifi_connect_btn_set, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_wifi_connect_btn_set, lv_color_hex(APP_THEME_TEXT), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_wifi_connect_btn_set, &lv_font_ZiTiQuanWeiJunHeiW22_18, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_wifi_connect_btn_set, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_wifi_connect_btn_set, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_wifi_connect_label_wifi
    ui->screen_wifi_connect_label_wifi = lv_label_create(ui->screen_wifi_connect);
    lv_obj_set_pos(ui->screen_wifi_connect_label_wifi, 56, 65);
    lv_obj_set_size(ui->screen_wifi_connect_label_wifi, 55, 20);
    lv_label_set_text(ui->screen_wifi_connect_label_wifi, "WiFi:");
    lv_label_set_long_mode(ui->screen_wifi_connect_label_wifi, LV_LABEL_LONG_WRAP);

    //Write style for screen_wifi_connect_label_wifi, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_wifi_connect_label_wifi, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_wifi_connect_label_wifi, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_wifi_connect_label_wifi, lv_color_hex(APP_THEME_TEXT), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_wifi_connect_label_wifi, &lv_font_ZiTiQuanWeiJunHeiW22_18, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_wifi_connect_label_wifi, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->screen_wifi_connect_label_wifi, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->screen_wifi_connect_label_wifi, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_wifi_connect_label_wifi, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_wifi_connect_label_wifi, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_wifi_connect_label_wifi, lv_color_hex(APP_THEME_BG), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_wifi_connect_label_wifi, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_wifi_connect_label_wifi, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_wifi_connect_label_wifi, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_wifi_connect_label_wifi, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_wifi_connect_label_wifi, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_wifi_connect_label_wifi, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_wifi_connect_label_wifiname
    ui->screen_wifi_connect_label_wifiname = lv_label_create(ui->screen_wifi_connect);
    lv_obj_set_pos(ui->screen_wifi_connect_label_wifiname, 116, 65);
    lv_obj_set_size(ui->screen_wifi_connect_label_wifiname, 55, 20);
    lv_label_set_text(ui->screen_wifi_connect_label_wifiname, "name");
    lv_label_set_long_mode(ui->screen_wifi_connect_label_wifiname, LV_LABEL_LONG_WRAP);

    //Write style for screen_wifi_connect_label_wifiname, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_wifi_connect_label_wifiname, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_wifi_connect_label_wifiname, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_wifi_connect_label_wifiname, lv_color_hex(APP_THEME_TEXT), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_wifi_connect_label_wifiname, &lv_font_ZiTiQuanWeiJunHeiW22_18, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_wifi_connect_label_wifiname, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->screen_wifi_connect_label_wifiname, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->screen_wifi_connect_label_wifiname, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_wifi_connect_label_wifiname, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_wifi_connect_label_wifiname, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_wifi_connect_label_wifiname, lv_color_hex(APP_THEME_BG), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_wifi_connect_label_wifiname, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_wifi_connect_label_wifiname, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_wifi_connect_label_wifiname, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_wifi_connect_label_wifiname, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_wifi_connect_label_wifiname, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_wifi_connect_label_wifiname, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //The custom code of screen_wifi_connect.


    //Update current screen layout.
    lv_obj_update_layout(ui->screen_wifi_connect);

    //Init events for screen.
    events_init_screen_wifi_connect(ui);
}
