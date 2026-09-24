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
#include "events_init.h"
#include "widgets_init.h"
#include "custom.h"
#include "guider_customer_fonts.h"



void setup_scr_screen_home(lv_ui *ui)
{
    //Write codes screen_home
    ui->screen_home = lv_obj_create(NULL);
    lv_obj_set_size(ui->screen_home, 240, 284);
    lv_obj_set_scrollbar_mode(ui->screen_home, LV_SCROLLBAR_MODE_OFF);

    //Write style for screen_home, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_home, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_home, lv_color_hex(0x010101), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_home, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_home_label_city
    ui->screen_home_label_city = lv_label_create(ui->screen_home);
    lv_obj_set_pos(ui->screen_home_label_city, 179, 35);
    lv_obj_set_size(ui->screen_home_label_city, 71, 22);
    lv_label_set_text(ui->screen_home_label_city, "南宁");
    lv_label_set_long_mode(ui->screen_home_label_city, LV_LABEL_LONG_WRAP);

    //Write style for screen_home_label_city, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_home_label_city, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_home_label_city, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_home_label_city, lv_color_hex(0xf5f5f5), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_home_label_city, &lv_font_ZiTiQuanWeiJunHeiW22_18, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_home_label_city, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->screen_home_label_city, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->screen_home_label_city, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_home_label_city, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_home_label_city, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_home_label_city, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_home_label_city, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_home_label_city, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_home_label_city, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_home_label_city, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_home_digital_clock
    static bool screen_home_digital_clock_timer_enabled = false;
    ui->screen_home_digital_clock = lv_label_create(ui->screen_home);
    lv_obj_set_pos(ui->screen_home_digital_clock, 11, 44);
    lv_obj_set_size(ui->screen_home_digital_clock, 220, 59);
    lv_label_set_text(ui->screen_home_digital_clock, "--:--:--");
    if (!screen_home_digital_clock_timer_enabled) {
        lv_timer_create(screen_home_digital_clock_timer, 1000, NULL);
        screen_home_digital_clock_timer_enabled = true;
    }

    //Write style for screen_home_digital_clock, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_radius(ui->screen_home_digital_clock, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_home_digital_clock, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_home_digital_clock, &lv_font_ZiTiQuanWeiJunHeiW22_48, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_home_digital_clock, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->screen_home_digital_clock, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_home_digital_clock, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_home_digital_clock, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_home_digital_clock, 7, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_home_digital_clock, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_home_digital_clock, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_home_digital_clock, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_home_digital_clock, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_home_label_data
    ui->screen_home_label_data = lv_label_create(ui->screen_home);
    lv_obj_set_pos(ui->screen_home_label_data, -8, 35);
    lv_obj_set_size(ui->screen_home_label_data, 124, 22);
    lv_label_set_text(ui->screen_home_label_data, "----/--/--");
    lv_label_set_long_mode(ui->screen_home_label_data, LV_LABEL_LONG_WRAP);

    //Write style for screen_home_label_data, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_home_label_data, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_home_label_data, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_home_label_data, lv_color_hex(0xf5f5f5), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_home_label_data, &lv_font_ZiTiQuanWeiJunHeiW22_18, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_home_label_data, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->screen_home_label_data, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->screen_home_label_data, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_home_label_data, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_home_label_data, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_home_label_data, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_home_label_data, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_home_label_data, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_home_label_data, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_home_label_data, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_home_label_week
    ui->screen_home_label_week = lv_label_create(ui->screen_home);
    lv_obj_set_pos(ui->screen_home_label_week, 116, 35);
    lv_obj_set_size(ui->screen_home_label_week, 60, 22);
    lv_label_set_text(ui->screen_home_label_week, "星期一");
    lv_label_set_long_mode(ui->screen_home_label_week, LV_LABEL_LONG_WRAP);

    //Write style for screen_home_label_week, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_home_label_week, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_home_label_week, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_home_label_week, lv_color_hex(0xf5f5f5), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_home_label_week, &lv_customer_font_ZiTiQuanWeiJunHeiW22_18, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_home_label_week, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->screen_home_label_week, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->screen_home_label_week, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_home_label_week, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_home_label_week, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_home_label_week, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_home_label_week, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_home_label_week, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_home_label_week, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_home_label_week, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_home_arc_temp
    ui->screen_home_arc_temp = lv_arc_create(ui->screen_home);
    lv_obj_set_pos(ui->screen_home_arc_temp, 11, 150);
    lv_obj_set_size(ui->screen_home_arc_temp, 69, 73);
    lv_arc_set_mode(ui->screen_home_arc_temp, LV_ARC_MODE_NORMAL);
    lv_arc_set_range(ui->screen_home_arc_temp, 0, 100);
    lv_arc_set_bg_angles(ui->screen_home_arc_temp, 135, 45);
    lv_arc_set_value(ui->screen_home_arc_temp, 70);
    lv_arc_set_rotation(ui->screen_home_arc_temp, 0);

    //Write style for screen_home_arc_temp, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_home_arc_temp, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_home_arc_temp, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_home_arc_temp, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->screen_home_arc_temp, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_arc_width(ui->screen_home_arc_temp, 12, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_arc_opa(ui->screen_home_arc_temp, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_arc_color(ui->screen_home_arc_temp, lv_color_hex(0xe6e6e6), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_arc_rounded(ui->screen_home_arc_temp, true, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_home_arc_temp, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_home_arc_temp, 5, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_home_arc_temp, 5, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_home_arc_temp, 5, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_home_arc_temp, 5, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_home_arc_temp, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style for screen_home_arc_temp, Part: LV_PART_INDICATOR, State: LV_STATE_DEFAULT.
    lv_obj_set_style_arc_width(ui->screen_home_arc_temp, 12, LV_PART_INDICATOR|LV_STATE_DEFAULT);
    lv_obj_set_style_arc_opa(ui->screen_home_arc_temp, 255, LV_PART_INDICATOR|LV_STATE_DEFAULT);
    lv_obj_set_style_arc_color(ui->screen_home_arc_temp, lv_color_hex(0xe29612), LV_PART_INDICATOR|LV_STATE_DEFAULT);
    lv_obj_set_style_arc_rounded(ui->screen_home_arc_temp, true, LV_PART_INDICATOR|LV_STATE_DEFAULT);

    //Write style for screen_home_arc_temp, Part: LV_PART_KNOB, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_home_arc_temp, 255, LV_PART_KNOB|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_home_arc_temp, lv_color_hex(0xf2a906), LV_PART_KNOB|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_home_arc_temp, LV_GRAD_DIR_NONE, LV_PART_KNOB|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_all(ui->screen_home_arc_temp, 0, LV_PART_KNOB|LV_STATE_DEFAULT);

    //Write codes screen_home_arc_heart_rate
    ui->screen_home_arc_heart_rate = lv_arc_create(ui->screen_home);
    lv_obj_set_pos(ui->screen_home_arc_heart_rate, 87, 150);
    lv_obj_set_size(ui->screen_home_arc_heart_rate, 69, 73);
    lv_arc_set_mode(ui->screen_home_arc_heart_rate, LV_ARC_MODE_NORMAL);
    lv_arc_set_range(ui->screen_home_arc_heart_rate, 0, 100);
    lv_arc_set_bg_angles(ui->screen_home_arc_heart_rate, 135, 45);
    lv_arc_set_value(ui->screen_home_arc_heart_rate, 70);
    lv_arc_set_rotation(ui->screen_home_arc_heart_rate, 0);

    //Write style for screen_home_arc_heart_rate, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_home_arc_heart_rate, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_home_arc_heart_rate, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_home_arc_heart_rate, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->screen_home_arc_heart_rate, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_arc_width(ui->screen_home_arc_heart_rate, 12, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_arc_opa(ui->screen_home_arc_heart_rate, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_arc_color(ui->screen_home_arc_heart_rate, lv_color_hex(0xf2f2f2), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_arc_rounded(ui->screen_home_arc_heart_rate, true, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_home_arc_heart_rate, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_home_arc_heart_rate, 5, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_home_arc_heart_rate, 5, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_home_arc_heart_rate, 5, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_home_arc_heart_rate, 5, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_home_arc_heart_rate, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style for screen_home_arc_heart_rate, Part: LV_PART_INDICATOR, State: LV_STATE_DEFAULT.
    lv_obj_set_style_arc_width(ui->screen_home_arc_heart_rate, 12, LV_PART_INDICATOR|LV_STATE_DEFAULT);
    lv_obj_set_style_arc_opa(ui->screen_home_arc_heart_rate, 255, LV_PART_INDICATOR|LV_STATE_DEFAULT);
    lv_obj_set_style_arc_color(ui->screen_home_arc_heart_rate, lv_color_hex(0x95192b), LV_PART_INDICATOR|LV_STATE_DEFAULT);
    lv_obj_set_style_arc_rounded(ui->screen_home_arc_heart_rate, true, LV_PART_INDICATOR|LV_STATE_DEFAULT);

    //Write style for screen_home_arc_heart_rate, Part: LV_PART_KNOB, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_home_arc_heart_rate, 255, LV_PART_KNOB|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_home_arc_heart_rate, lv_color_hex(0x900d2c), LV_PART_KNOB|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_home_arc_heart_rate, LV_GRAD_DIR_NONE, LV_PART_KNOB|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_all(ui->screen_home_arc_heart_rate, 0, LV_PART_KNOB|LV_STATE_DEFAULT);

    //Write codes screen_home_arc_humidity
    ui->screen_home_arc_humidity = lv_arc_create(ui->screen_home);
    lv_obj_set_pos(ui->screen_home_arc_humidity, 165, 150);
    lv_obj_set_size(ui->screen_home_arc_humidity, 69, 73);
    lv_arc_set_mode(ui->screen_home_arc_humidity, LV_ARC_MODE_NORMAL);
    lv_arc_set_range(ui->screen_home_arc_humidity, 0, 100);
    lv_arc_set_bg_angles(ui->screen_home_arc_humidity, 135, 45);
    lv_arc_set_value(ui->screen_home_arc_humidity, 70);
    lv_arc_set_rotation(ui->screen_home_arc_humidity, 0);

    //Write style for screen_home_arc_humidity, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_home_arc_humidity, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_home_arc_humidity, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_home_arc_humidity, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->screen_home_arc_humidity, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_arc_width(ui->screen_home_arc_humidity, 12, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_arc_opa(ui->screen_home_arc_humidity, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_arc_color(ui->screen_home_arc_humidity, lv_color_hex(0xe6e6e6), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_arc_rounded(ui->screen_home_arc_humidity, true, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_home_arc_humidity, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_home_arc_humidity, 5, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_home_arc_humidity, 5, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_home_arc_humidity, 5, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_home_arc_humidity, 5, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_home_arc_humidity, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style for screen_home_arc_humidity, Part: LV_PART_INDICATOR, State: LV_STATE_DEFAULT.
    lv_obj_set_style_arc_width(ui->screen_home_arc_humidity, 12, LV_PART_INDICATOR|LV_STATE_DEFAULT);
    lv_obj_set_style_arc_opa(ui->screen_home_arc_humidity, 255, LV_PART_INDICATOR|LV_STATE_DEFAULT);
    lv_obj_set_style_arc_color(ui->screen_home_arc_humidity, lv_color_hex(0x2195f6), LV_PART_INDICATOR|LV_STATE_DEFAULT);
    lv_obj_set_style_arc_rounded(ui->screen_home_arc_humidity, true, LV_PART_INDICATOR|LV_STATE_DEFAULT);

    //Write style for screen_home_arc_humidity, Part: LV_PART_KNOB, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_home_arc_humidity, 255, LV_PART_KNOB|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_home_arc_humidity, lv_color_hex(0x2195f6), LV_PART_KNOB|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_home_arc_humidity, LV_GRAD_DIR_NONE, LV_PART_KNOB|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_all(ui->screen_home_arc_humidity, 0, LV_PART_KNOB|LV_STATE_DEFAULT);

    //Write codes screen_home_label_temp
    ui->screen_home_label_temp = lv_label_create(ui->screen_home);
    lv_obj_set_pos(ui->screen_home_label_temp, 11, 121);
    lv_obj_set_size(ui->screen_home_label_temp, 52, 18);
    lv_label_set_text(ui->screen_home_label_temp, "温度");
    lv_label_set_long_mode(ui->screen_home_label_temp, LV_LABEL_LONG_WRAP);

    //Write style for screen_home_label_temp, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_home_label_temp, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_home_label_temp, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_home_label_temp, lv_color_hex(0xefeeee), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_home_label_temp, &lv_font_ZiTiQuanWeiJunHeiW22_18, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_home_label_temp, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->screen_home_label_temp, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->screen_home_label_temp, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_home_label_temp, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_home_label_temp, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_home_label_temp, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_home_label_temp, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_home_label_temp, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_home_label_temp, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_home_label_temp, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_home_label_temp, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_home_label_temp, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_home_label_heart_rate
    ui->screen_home_label_heart_rate = lv_label_create(ui->screen_home);
    lv_obj_set_pos(ui->screen_home_label_heart_rate, 87, 121);
    lv_obj_set_size(ui->screen_home_label_heart_rate, 52, 18);
    lv_label_set_text(ui->screen_home_label_heart_rate, "心率");
    lv_label_set_long_mode(ui->screen_home_label_heart_rate, LV_LABEL_LONG_WRAP);

    //Write style for screen_home_label_heart_rate, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_home_label_heart_rate, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_home_label_heart_rate, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_home_label_heart_rate, lv_color_hex(0xefeeee), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_home_label_heart_rate, &lv_font_ZiTiQuanWeiJunHeiW22_18, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_home_label_heart_rate, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->screen_home_label_heart_rate, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->screen_home_label_heart_rate, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_home_label_heart_rate, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_home_label_heart_rate, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_home_label_heart_rate, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_home_label_heart_rate, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_home_label_heart_rate, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_home_label_heart_rate, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_home_label_heart_rate, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_home_label_heart_rate, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_home_label_heart_rate, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_home_label_humidity
    ui->screen_home_label_humidity = lv_label_create(ui->screen_home);
    lv_obj_set_pos(ui->screen_home_label_humidity, 170, 121);
    lv_obj_set_size(ui->screen_home_label_humidity, 52, 18);
    lv_label_set_text(ui->screen_home_label_humidity, "湿度");
    lv_label_set_long_mode(ui->screen_home_label_humidity, LV_LABEL_LONG_WRAP);

    //Write style for screen_home_label_humidity, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_home_label_humidity, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_home_label_humidity, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_home_label_humidity, lv_color_hex(0xefeeee), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_home_label_humidity, &lv_font_ZiTiQuanWeiJunHeiW22_18, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_home_label_humidity, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->screen_home_label_humidity, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->screen_home_label_humidity, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_home_label_humidity, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_home_label_humidity, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_home_label_humidity, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_home_label_humidity, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_home_label_humidity, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_home_label_humidity, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_home_label_humidity, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_home_label_humidity, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_home_label_humidity, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_home_label_te
    ui->screen_home_label_te = lv_label_create(ui->screen_home);
    lv_obj_set_pos(ui->screen_home_label_te, 16, 202);
    lv_obj_set_size(ui->screen_home_label_te, 52, 18);
    lv_label_set_text(ui->screen_home_label_te, "27℃");
    lv_label_set_long_mode(ui->screen_home_label_te, LV_LABEL_LONG_WRAP);

    //Write style for screen_home_label_te, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_home_label_te, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_home_label_te, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_home_label_te, lv_color_hex(0xefeeee), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_home_label_te, &lv_font_ZiTiQuanWeiJunHeiW22_18, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_home_label_te, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->screen_home_label_te, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->screen_home_label_te, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_home_label_te, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_home_label_te, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_home_label_te, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_home_label_te, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_home_label_te, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_home_label_te, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_home_label_te, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_home_label_te, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_home_label_te, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_home_label_hr
    ui->screen_home_label_hr = lv_label_create(ui->screen_home);
    lv_obj_set_pos(ui->screen_home_label_hr, 87, 202);
    lv_obj_set_size(ui->screen_home_label_hr, 66, 18);
    lv_label_set_text(ui->screen_home_label_hr, "67次/分");
    lv_label_set_long_mode(ui->screen_home_label_hr, LV_LABEL_LONG_WRAP);

    //Write style for screen_home_label_hr, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_home_label_hr, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_home_label_hr, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_home_label_hr, lv_color_hex(0xefeeee), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_home_label_hr, &lv_font_ZiTiQuanWeiJunHeiW22_18, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_home_label_hr, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->screen_home_label_hr, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->screen_home_label_hr, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_home_label_hr, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_home_label_hr, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_home_label_hr, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_home_label_hr, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_home_label_hr, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_home_label_hr, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_home_label_hr, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_home_label_hr, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_home_label_hr, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_home_label_him
    ui->screen_home_label_him = lv_label_create(ui->screen_home);
    lv_obj_set_pos(ui->screen_home_label_him, 170, 202);
    lv_obj_set_size(ui->screen_home_label_him, 52, 18);
    lv_label_set_text(ui->screen_home_label_him, "81%");
    lv_label_set_long_mode(ui->screen_home_label_him, LV_LABEL_LONG_WRAP);

    //Write style for screen_home_label_him, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_home_label_him, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_home_label_him, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_home_label_him, lv_color_hex(0xefeeee), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_home_label_him, &lv_font_ZiTiQuanWeiJunHeiW22_18, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_home_label_him, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->screen_home_label_him, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->screen_home_label_him, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_home_label_him, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_home_label_him, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_home_label_him, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_home_label_him, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_home_label_him, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_home_label_him, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_home_label_him, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_home_label_him, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_home_label_him, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_home_img_wifi
    ui->screen_home_img_wifi = lv_image_create(ui->screen_home);
    lv_obj_set_pos(ui->screen_home_img_wifi, 148, 7);
    lv_obj_set_size(ui->screen_home_img_wifi, 20, 24);
    lv_obj_add_flag(ui->screen_home_img_wifi, LV_OBJ_FLAG_CLICKABLE);
    lv_image_set_src(ui->screen_home_img_wifi, &_wifi_close_RGB565A8_20x24);
    lv_image_set_pivot(ui->screen_home_img_wifi, 50,50);
    lv_image_set_rotation(ui->screen_home_img_wifi, 0);

    //Write style for screen_home_img_wifi, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_image_recolor_opa(ui->screen_home_img_wifi, 26, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_image_recolor(ui->screen_home_img_wifi, lv_color_hex(0x090000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_image_opa(ui->screen_home_img_wifi, 255, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_home_img_foot
    ui->screen_home_img_foot = lv_image_create(ui->screen_home);
    lv_obj_set_pos(ui->screen_home_img_foot, 3, 233);
    lv_obj_set_size(ui->screen_home_img_foot, 51, 34);
    lv_obj_add_flag(ui->screen_home_img_foot, LV_OBJ_FLAG_CLICKABLE);
    lv_image_set_src(ui->screen_home_img_foot, &_foot_RGB565A8_51x34);
    lv_image_set_pivot(ui->screen_home_img_foot, 50,50);
    lv_image_set_rotation(ui->screen_home_img_foot, 0);

    //Write style for screen_home_img_foot, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_image_recolor_opa(ui->screen_home_img_foot, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_image_opa(ui->screen_home_img_foot, 255, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_home_label_7
    ui->screen_home_label_7 = lv_label_create(ui->screen_home);
    lv_obj_set_pos(ui->screen_home_label_7, 46, 233);
    lv_obj_set_size(ui->screen_home_label_7, 74, 18);
    lv_label_set_text(ui->screen_home_label_7, "今日步数");
    lv_label_set_long_mode(ui->screen_home_label_7, LV_LABEL_LONG_WRAP);

    //Write style for screen_home_label_7, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_home_label_7, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_home_label_7, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_home_label_7, lv_color_hex(0xf3f1f1), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_home_label_7, &lv_font_ZiTiQuanWeiJunHeiW22_12, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_home_label_7, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->screen_home_label_7, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->screen_home_label_7, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_home_label_7, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_home_label_7, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_home_label_7, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_home_label_7, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_home_label_7, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_home_label_7, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_home_label_7, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_home_label_foot
    ui->screen_home_label_foot = lv_label_create(ui->screen_home);
    lv_obj_set_pos(ui->screen_home_label_foot, 29, 245);
    lv_obj_set_size(ui->screen_home_label_foot, 100, 32);
    lv_label_set_text(ui->screen_home_label_foot, "1234");
    lv_label_set_long_mode(ui->screen_home_label_foot, LV_LABEL_LONG_WRAP);

    //Write style for screen_home_label_foot, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_home_label_foot, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_home_label_foot, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_home_label_foot, lv_color_hex(0xe8dede), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_home_label_foot, &lv_font_ZiTiQuanWeiJunHeiW22_18, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_home_label_foot, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->screen_home_label_foot, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->screen_home_label_foot, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_home_label_foot, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_home_label_foot, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_home_label_foot, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_home_label_foot, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_home_label_foot, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_home_label_foot, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_home_label_foot, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_home_img_pa
    ui->screen_home_img_pa = lv_image_create(ui->screen_home);
    lv_obj_set_pos(ui->screen_home_img_pa, 120, 230);
    lv_obj_set_size(ui->screen_home_img_pa, 45, 39);
    lv_obj_add_flag(ui->screen_home_img_pa, LV_OBJ_FLAG_CLICKABLE);
    lv_image_set_src(ui->screen_home_img_pa, &_Air_pressure_RGB565A8_45x39);
    lv_image_set_pivot(ui->screen_home_img_pa, 50,50);
    lv_image_set_rotation(ui->screen_home_img_pa, 0);

    //Write style for screen_home_img_pa, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_image_recolor_opa(ui->screen_home_img_pa, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_image_opa(ui->screen_home_img_pa, 255, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_home_label_9
    ui->screen_home_label_9 = lv_label_create(ui->screen_home);
    lv_obj_set_pos(ui->screen_home_label_9, 156, 233);
    lv_obj_set_size(ui->screen_home_label_9, 74, 18);
    lv_label_set_text(ui->screen_home_label_9, "当前气压\n");
    lv_label_set_long_mode(ui->screen_home_label_9, LV_LABEL_LONG_WRAP);

    //Write style for screen_home_label_9, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_home_label_9, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_home_label_9, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_home_label_9, lv_color_hex(0xf3f1f1), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_home_label_9, &lv_font_ZiTiQuanWeiJunHeiW22_12, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_home_label_9, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->screen_home_label_9, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->screen_home_label_9, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_home_label_9, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_home_label_9, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_home_label_9, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_home_label_9, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_home_label_9, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_home_label_9, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_home_label_9, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_home_label_pa
    ui->screen_home_label_pa = lv_label_create(ui->screen_home);
    lv_obj_set_pos(ui->screen_home_label_pa, 148, 245);
    lv_obj_set_size(ui->screen_home_label_pa, 100, 32);
    lv_label_set_text(ui->screen_home_label_pa, "1234 Pa");
    lv_label_set_long_mode(ui->screen_home_label_pa, LV_LABEL_LONG_WRAP);

    //Write style for screen_home_label_pa, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_home_label_pa, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_home_label_pa, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_home_label_pa, lv_color_hex(0xe8dede), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_home_label_pa, &lv_font_ZiTiQuanWeiJunHeiW22_18, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_home_label_pa, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->screen_home_label_pa, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->screen_home_label_pa, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_home_label_pa, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_home_label_pa, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_home_label_pa, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_home_label_pa, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_home_label_pa, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_home_label_pa, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_home_label_pa, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_home_list_bettery
    ui->screen_home_list_bettery = lv_list_create(ui->screen_home);
    lv_obj_set_pos(ui->screen_home_list_bettery, 170, 3);
    lv_obj_set_size(ui->screen_home_list_bettery, 64, 26);
    lv_obj_set_scrollbar_mode(ui->screen_home_list_bettery, LV_SCROLLBAR_MODE_OFF);
    ui->screen_home_list_bettery_item0 = lv_list_add_button(ui->screen_home_list_bettery, NULL, "--");

    //Write style state: LV_STATE_DEFAULT for &style_screen_home_list_bettery_main_main_default
    static lv_style_t style_screen_home_list_bettery_main_main_default;
    ui_init_style(&style_screen_home_list_bettery_main_main_default);

    lv_style_set_pad_top(&style_screen_home_list_bettery_main_main_default, 5);
    lv_style_set_pad_left(&style_screen_home_list_bettery_main_main_default, 5);
    lv_style_set_pad_right(&style_screen_home_list_bettery_main_main_default, 5);
    lv_style_set_pad_bottom(&style_screen_home_list_bettery_main_main_default, 5);
    lv_style_set_bg_opa(&style_screen_home_list_bettery_main_main_default, 255);
    lv_style_set_bg_color(&style_screen_home_list_bettery_main_main_default, lv_color_hex(0x050505));
    lv_style_set_bg_grad_dir(&style_screen_home_list_bettery_main_main_default, LV_GRAD_DIR_NONE);
    lv_style_set_border_width(&style_screen_home_list_bettery_main_main_default, 1);
    lv_style_set_border_opa(&style_screen_home_list_bettery_main_main_default, 255);
    lv_style_set_border_color(&style_screen_home_list_bettery_main_main_default, lv_color_hex(0x000000));
    lv_style_set_border_side(&style_screen_home_list_bettery_main_main_default, LV_BORDER_SIDE_FULL);
    lv_style_set_radius(&style_screen_home_list_bettery_main_main_default, 3);
    lv_style_set_shadow_width(&style_screen_home_list_bettery_main_main_default, 0);
    lv_obj_add_style(ui->screen_home_list_bettery, &style_screen_home_list_bettery_main_main_default, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style state: LV_STATE_DEFAULT for &style_screen_home_list_bettery_main_scrollbar_default
    static lv_style_t style_screen_home_list_bettery_main_scrollbar_default;
    ui_init_style(&style_screen_home_list_bettery_main_scrollbar_default);

    lv_style_set_radius(&style_screen_home_list_bettery_main_scrollbar_default, 3);
    lv_style_set_bg_opa(&style_screen_home_list_bettery_main_scrollbar_default, 255);
    lv_style_set_bg_color(&style_screen_home_list_bettery_main_scrollbar_default, lv_color_hex(0x000000));
    lv_style_set_bg_grad_dir(&style_screen_home_list_bettery_main_scrollbar_default, LV_GRAD_DIR_NONE);
    lv_obj_add_style(ui->screen_home_list_bettery, &style_screen_home_list_bettery_main_scrollbar_default, LV_PART_SCROLLBAR|LV_STATE_DEFAULT);

    //Write style state: LV_STATE_DEFAULT for &style_screen_home_list_bettery_extra_btns_main_default
    static lv_style_t style_screen_home_list_bettery_extra_btns_main_default;
    ui_init_style(&style_screen_home_list_bettery_extra_btns_main_default);

    lv_style_set_pad_top(&style_screen_home_list_bettery_extra_btns_main_default, 5);
    lv_style_set_pad_left(&style_screen_home_list_bettery_extra_btns_main_default, 5);
    lv_style_set_pad_right(&style_screen_home_list_bettery_extra_btns_main_default, 5);
    lv_style_set_pad_bottom(&style_screen_home_list_bettery_extra_btns_main_default, 5);
    lv_style_set_border_width(&style_screen_home_list_bettery_extra_btns_main_default, 0);
    lv_style_set_text_color(&style_screen_home_list_bettery_extra_btns_main_default, lv_color_hex(0xfbfbfb));
    lv_style_set_text_font(&style_screen_home_list_bettery_extra_btns_main_default, &lv_font_ZiTiQuanWeiJunHeiW22_12);
    lv_style_set_text_opa(&style_screen_home_list_bettery_extra_btns_main_default, 255);
    lv_style_set_radius(&style_screen_home_list_bettery_extra_btns_main_default, 3);
    lv_style_set_bg_opa(&style_screen_home_list_bettery_extra_btns_main_default, 255);
    lv_style_set_bg_color(&style_screen_home_list_bettery_extra_btns_main_default, lv_color_hex(0x000000));
    lv_style_set_bg_grad_dir(&style_screen_home_list_bettery_extra_btns_main_default, LV_GRAD_DIR_NONE);
    lv_obj_add_style(ui->screen_home_list_bettery_item0, &style_screen_home_list_bettery_extra_btns_main_default, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style state: LV_STATE_DEFAULT for &style_screen_home_list_bettery_extra_texts_main_default
    static lv_style_t style_screen_home_list_bettery_extra_texts_main_default;
    ui_init_style(&style_screen_home_list_bettery_extra_texts_main_default);

    lv_style_set_pad_top(&style_screen_home_list_bettery_extra_texts_main_default, 5);
    lv_style_set_pad_left(&style_screen_home_list_bettery_extra_texts_main_default, 5);
    lv_style_set_pad_right(&style_screen_home_list_bettery_extra_texts_main_default, 5);
    lv_style_set_pad_bottom(&style_screen_home_list_bettery_extra_texts_main_default, 5);
    lv_style_set_border_width(&style_screen_home_list_bettery_extra_texts_main_default, 0);
    lv_style_set_text_color(&style_screen_home_list_bettery_extra_texts_main_default, lv_color_hex(0xf9f9f9));
    lv_style_set_text_font(&style_screen_home_list_bettery_extra_texts_main_default, &lv_font_ZiTiQuanWeiJunHeiW22_12);
    lv_style_set_text_opa(&style_screen_home_list_bettery_extra_texts_main_default, 255);
    lv_style_set_radius(&style_screen_home_list_bettery_extra_texts_main_default, 3);
    lv_style_set_transform_width(&style_screen_home_list_bettery_extra_texts_main_default, 0);
    lv_style_set_bg_opa(&style_screen_home_list_bettery_extra_texts_main_default, 255);
    lv_style_set_bg_color(&style_screen_home_list_bettery_extra_texts_main_default, lv_color_hex(0xffffff));
    lv_style_set_bg_grad_dir(&style_screen_home_list_bettery_extra_texts_main_default, LV_GRAD_DIR_NONE);

    //The custom code of screen_home.
    // 首页会被切换时删除，定时器仍运行；清空标签指针避免地址复用后误写其他页面。
    lv_obj_null_on_delete(&ui->screen_home_digital_clock);
    lv_obj_null_on_delete(&ui->screen_home_label_data);
    lv_obj_null_on_delete(&ui->screen_home_label_week);

    //Update current screen layout.
    lv_obj_update_layout(ui->screen_home);

    //Init events for screen.
    events_init_screen_home(ui);
}
