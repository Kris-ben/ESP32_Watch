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



void setup_scr_screen_set_clock(lv_ui *ui)
{
    //Write codes screen_set_clock
    ui->screen_set_clock = lv_obj_create(NULL);
    lv_obj_set_size(ui->screen_set_clock, 240, 284);
    lv_obj_set_scrollbar_mode(ui->screen_set_clock, LV_SCROLLBAR_MODE_OFF);

    //Write style for screen_set_clock, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_set_clock, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_set_clock, lv_color_hex(APP_THEME_BG), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_set_clock, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_set_clock_roller_hour
    ui->screen_set_clock_roller_hour = lv_roller_create(ui->screen_set_clock);
    lv_obj_set_pos(ui->screen_set_clock_roller_hour, 8, 39);
    lv_obj_set_width(ui->screen_set_clock_roller_hour, 100);
    lv_roller_set_options(ui->screen_set_clock_roller_hour, "1时\n2时\n3时\n4时\n5时\n6时\n7时\n8时\n9时\n10时\n11时\n12时\n13时\n14时\n15时\n16时\n17时\n18时\n19时\n20时\n21时\n22时\n23时\n0时", LV_ROLLER_MODE_INFINITE);

    //Write style for screen_set_clock_roller_hour, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_radius(ui->screen_set_clock_roller_hour, 5, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_set_clock_roller_hour, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_set_clock_roller_hour, lv_color_hex(APP_THEME_CARD), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_set_clock_roller_hour, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_set_clock_roller_hour, lv_color_hex(APP_THEME_CARD), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_set_clock_roller_hour, &lv_font_ZiTiQuanWeiJunHeiW22_18, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_set_clock_roller_hour, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_set_clock_roller_hour, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->screen_set_clock_roller_hour, 2, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(ui->screen_set_clock_roller_hour, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(ui->screen_set_clock_roller_hour, lv_color_hex(APP_THEME_TEXT), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_side(ui->screen_set_clock_roller_hour, LV_BORDER_SIDE_FULL, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_set_clock_roller_hour, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_set_clock_roller_hour, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_set_clock_roller_hour, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style for screen_set_clock_roller_hour, Part: LV_PART_SELECTED, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_set_clock_roller_hour, 255, LV_PART_SELECTED|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_set_clock_roller_hour, lv_color_hex(APP_THEME_ACCENT), LV_PART_SELECTED|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_set_clock_roller_hour, LV_GRAD_DIR_NONE, LV_PART_SELECTED|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_set_clock_roller_hour, lv_color_hex(APP_THEME_ON_ACCENT), LV_PART_SELECTED|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_set_clock_roller_hour, &lv_font_ZiTiQuanWeiJunHeiW22_24, LV_PART_SELECTED|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_set_clock_roller_hour, 255, LV_PART_SELECTED|LV_STATE_DEFAULT);

    lv_roller_set_visible_row_count(ui->screen_set_clock_roller_hour, 4);
    //Write codes screen_set_clock_roller_minute
    ui->screen_set_clock_roller_minute = lv_roller_create(ui->screen_set_clock);
    lv_obj_set_pos(ui->screen_set_clock_roller_minute, 125, 39);
    lv_obj_set_width(ui->screen_set_clock_roller_minute, 100);
    lv_roller_set_options(ui->screen_set_clock_roller_minute, "00分\n10分\n20分\n30分\n40分\n50分", LV_ROLLER_MODE_INFINITE);

    //Write style for screen_set_clock_roller_minute, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_radius(ui->screen_set_clock_roller_minute, 5, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_set_clock_roller_minute, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_set_clock_roller_minute, lv_color_hex(APP_THEME_CARD), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_set_clock_roller_minute, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_set_clock_roller_minute, lv_color_hex(APP_THEME_CARD), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_set_clock_roller_minute, &lv_font_ZiTiQuanWeiJunHeiW22_18, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_set_clock_roller_minute, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_set_clock_roller_minute, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->screen_set_clock_roller_minute, 2, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(ui->screen_set_clock_roller_minute, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(ui->screen_set_clock_roller_minute, lv_color_hex(APP_THEME_TEXT), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_side(ui->screen_set_clock_roller_minute, LV_BORDER_SIDE_FULL, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_set_clock_roller_minute, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_set_clock_roller_minute, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_set_clock_roller_minute, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style for screen_set_clock_roller_minute, Part: LV_PART_SELECTED, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_set_clock_roller_minute, 255, LV_PART_SELECTED|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_set_clock_roller_minute, lv_color_hex(APP_THEME_ACCENT), LV_PART_SELECTED|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_set_clock_roller_minute, LV_GRAD_DIR_NONE, LV_PART_SELECTED|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_set_clock_roller_minute, lv_color_hex(APP_THEME_ON_ACCENT), LV_PART_SELECTED|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_set_clock_roller_minute, &lv_font_ZiTiQuanWeiJunHeiW22_24, LV_PART_SELECTED|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_set_clock_roller_minute, 255, LV_PART_SELECTED|LV_STATE_DEFAULT);

    lv_roller_set_visible_row_count(ui->screen_set_clock_roller_minute, 4);
    //Write codes screen_set_clock_btn_set
    ui->screen_set_clock_btn_set = lv_button_create(ui->screen_set_clock);
    lv_obj_set_pos(ui->screen_set_clock_btn_set, 17, 221);
    lv_obj_set_size(ui->screen_set_clock_btn_set, 76, 29);
    ui->screen_set_clock_btn_set_label = lv_label_create(ui->screen_set_clock_btn_set);
    lv_label_set_text(ui->screen_set_clock_btn_set_label, "确定");
    lv_label_set_long_mode(ui->screen_set_clock_btn_set_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(ui->screen_set_clock_btn_set_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(ui->screen_set_clock_btn_set, 0, LV_STATE_DEFAULT);
    lv_obj_set_width(ui->screen_set_clock_btn_set_label, LV_PCT(100));

    //Write style for screen_set_clock_btn_set, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_set_clock_btn_set, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_set_clock_btn_set, lv_color_hex(APP_THEME_ACCENT), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_set_clock_btn_set, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->screen_set_clock_btn_set, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_set_clock_btn_set, 5, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_set_clock_btn_set, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_set_clock_btn_set, lv_color_hex(APP_THEME_ON_ACCENT), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_set_clock_btn_set, &lv_font_ZiTiQuanWeiJunHeiW22_18, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_set_clock_btn_set, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_set_clock_btn_set, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_set_clock_btn_return
    ui->screen_set_clock_btn_return = lv_button_create(ui->screen_set_clock);
    lv_obj_set_pos(ui->screen_set_clock_btn_return, 133, 221);
    lv_obj_set_size(ui->screen_set_clock_btn_return, 76, 29);
    ui->screen_set_clock_btn_return_label = lv_label_create(ui->screen_set_clock_btn_return);
    lv_label_set_text(ui->screen_set_clock_btn_return_label, "返回");
    lv_label_set_long_mode(ui->screen_set_clock_btn_return_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(ui->screen_set_clock_btn_return_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(ui->screen_set_clock_btn_return, 0, LV_STATE_DEFAULT);
    lv_obj_set_width(ui->screen_set_clock_btn_return_label, LV_PCT(100));

    //Write style for screen_set_clock_btn_return, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_set_clock_btn_return, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_set_clock_btn_return, lv_color_hex(APP_THEME_ACCENT), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_set_clock_btn_return, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->screen_set_clock_btn_return, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_set_clock_btn_return, 5, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_set_clock_btn_return, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_set_clock_btn_return, lv_color_hex(APP_THEME_ON_ACCENT), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_set_clock_btn_return, &lv_font_ZiTiQuanWeiJunHeiW22_18, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_set_clock_btn_return, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_set_clock_btn_return, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);

    //The custom code of screen_set_clock.


    //Update current screen layout.
    lv_obj_update_layout(ui->screen_set_clock);

    //Init events for screen.
    events_init_screen_set_clock(ui);
}
