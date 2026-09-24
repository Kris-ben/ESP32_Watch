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
#include "weather.h"
#include "guider_customer_fonts.h"



void setup_scr_screen_weather(lv_ui *ui)
{
    //Write codes screen_weather
    ui->screen_weather = lv_obj_create(NULL);
    lv_obj_set_size(ui->screen_weather, 240, 284);
    lv_obj_set_scrollbar_mode(ui->screen_weather, LV_SCROLLBAR_MODE_OFF);

    //Write style for screen_weather, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_weather, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_weather, lv_color_hex(APP_THEME_BG), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_weather, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_weather_label_later
    ui->screen_weather_label_later = lv_label_create(ui->screen_weather);
    lv_obj_set_pos(ui->screen_weather_label_later, 166, 71);
    lv_obj_set_size(ui->screen_weather_label_later, 71, 22);
    lv_label_set_text(ui->screen_weather_label_later, "后日");
    lv_label_set_long_mode(ui->screen_weather_label_later, LV_LABEL_LONG_WRAP);

    //Write style for screen_weather_label_later, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_weather_label_later, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_weather_label_later, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_weather_label_later, lv_color_hex(APP_THEME_TEXT), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_weather_label_later, &lv_font_ZiTiQuanWeiJunHeiW22_18, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_weather_label_later, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->screen_weather_label_later, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->screen_weather_label_later, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_weather_label_later, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_weather_label_later, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_weather_label_later, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_weather_label_later, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_weather_label_later, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_weather_label_later, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_weather_label_later, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_weather_label_tomorrow
    ui->screen_weather_label_tomorrow = lv_label_create(ui->screen_weather);
    lv_obj_set_pos(ui->screen_weather_label_tomorrow, 89, 71);
    lv_obj_set_size(ui->screen_weather_label_tomorrow, 71, 22);
    lv_label_set_text(ui->screen_weather_label_tomorrow, "明日");
    lv_label_set_long_mode(ui->screen_weather_label_tomorrow, LV_LABEL_LONG_WRAP);

    //Write style for screen_weather_label_tomorrow, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_weather_label_tomorrow, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_weather_label_tomorrow, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_weather_label_tomorrow, lv_color_hex(APP_THEME_TEXT), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_weather_label_tomorrow, &lv_font_ZiTiQuanWeiJunHeiW22_18, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_weather_label_tomorrow, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->screen_weather_label_tomorrow, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->screen_weather_label_tomorrow, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_weather_label_tomorrow, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_weather_label_tomorrow, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_weather_label_tomorrow, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_weather_label_tomorrow, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_weather_label_tomorrow, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_weather_label_tomorrow, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_weather_label_tomorrow, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_weather_label_today
    ui->screen_weather_label_today = lv_label_create(ui->screen_weather);
    lv_obj_set_pos(ui->screen_weather_label_today, 8, 71);
    lv_obj_set_size(ui->screen_weather_label_today, 71, 22);
    lv_label_set_text(ui->screen_weather_label_today, "今日");
    lv_label_set_long_mode(ui->screen_weather_label_today, LV_LABEL_LONG_WRAP);

    //Write style for screen_weather_label_today, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_weather_label_today, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_weather_label_today, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_weather_label_today, lv_color_hex(APP_THEME_TEXT), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_weather_label_today, &lv_font_ZiTiQuanWeiJunHeiW22_18, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_weather_label_today, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->screen_weather_label_today, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->screen_weather_label_today, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_weather_label_today, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_weather_label_today, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_weather_label_today, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_weather_label_today, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_weather_label_today, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_weather_label_today, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_weather_label_today, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_weather_label_later_temp
    ui->screen_weather_label_later_temp = lv_label_create(ui->screen_weather);
    lv_obj_set_pos(ui->screen_weather_label_later_temp, 169, 201);
    lv_obj_set_size(ui->screen_weather_label_later_temp, 71, 22);
    lv_label_set_text(ui->screen_weather_label_later_temp, "--℃");
    lv_label_set_long_mode(ui->screen_weather_label_later_temp, LV_LABEL_LONG_WRAP);

    //Write style for screen_weather_label_later_temp, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_weather_label_later_temp, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_weather_label_later_temp, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_weather_label_later_temp, lv_color_hex(APP_THEME_TEXT), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_weather_label_later_temp, &lv_font_ZiTiQuanWeiJunHeiW22_18, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_weather_label_later_temp, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->screen_weather_label_later_temp, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->screen_weather_label_later_temp, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_weather_label_later_temp, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_weather_label_later_temp, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_weather_label_later_temp, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_weather_label_later_temp, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_weather_label_later_temp, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_weather_label_later_temp, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_weather_label_later_temp, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_weather_label_tomorrow_temp
    ui->screen_weather_label_tomorrow_temp = lv_label_create(ui->screen_weather);
    lv_obj_set_pos(ui->screen_weather_label_tomorrow_temp, 83, 201);
    lv_obj_set_size(ui->screen_weather_label_tomorrow_temp, 71, 22);
    lv_label_set_text(ui->screen_weather_label_tomorrow_temp, "--℃");
    lv_label_set_long_mode(ui->screen_weather_label_tomorrow_temp, LV_LABEL_LONG_WRAP);

    //Write style for screen_weather_label_tomorrow_temp, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_weather_label_tomorrow_temp, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_weather_label_tomorrow_temp, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_weather_label_tomorrow_temp, lv_color_hex(APP_THEME_TEXT), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_weather_label_tomorrow_temp, &lv_font_ZiTiQuanWeiJunHeiW22_18, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_weather_label_tomorrow_temp, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->screen_weather_label_tomorrow_temp, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->screen_weather_label_tomorrow_temp, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_weather_label_tomorrow_temp, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_weather_label_tomorrow_temp, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_weather_label_tomorrow_temp, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_weather_label_tomorrow_temp, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_weather_label_tomorrow_temp, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_weather_label_tomorrow_temp, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_weather_label_tomorrow_temp, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_weather_label_today_temp
    ui->screen_weather_label_today_temp = lv_label_create(ui->screen_weather);
    lv_obj_set_pos(ui->screen_weather_label_today_temp, 8, 201);
    lv_obj_set_size(ui->screen_weather_label_today_temp, 71, 22);
    lv_label_set_text(ui->screen_weather_label_today_temp, "--℃");
    lv_label_set_long_mode(ui->screen_weather_label_today_temp, LV_LABEL_LONG_WRAP);

    //Write style for screen_weather_label_today_temp, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_weather_label_today_temp, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_weather_label_today_temp, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_weather_label_today_temp, lv_color_hex(APP_THEME_TEXT), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_weather_label_today_temp, &lv_font_ZiTiQuanWeiJunHeiW22_18, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_weather_label_today_temp, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->screen_weather_label_today_temp, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->screen_weather_label_today_temp, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_weather_label_today_temp, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_weather_label_today_temp, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_weather_label_today_temp, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_weather_label_today_temp, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_weather_label_today_temp, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_weather_label_today_temp, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_weather_label_today_temp, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_weather_img_tomorrow
    ui->screen_weather_img_tomorrow = lv_image_create(ui->screen_weather);
    lv_obj_set_pos(ui->screen_weather_img_tomorrow, 94, 104);
    lv_obj_set_size(ui->screen_weather_img_tomorrow, 60, 59);
    lv_obj_add_flag(ui->screen_weather_img_tomorrow, LV_OBJ_FLAG_CLICKABLE);
    lv_image_set_src(ui->screen_weather_img_tomorrow, &_yu_RGB565A8_60x59);
    lv_image_set_pivot(ui->screen_weather_img_tomorrow, 50,50);
    lv_image_set_rotation(ui->screen_weather_img_tomorrow, 0);

    //Write style for screen_weather_img_tomorrow, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_image_recolor_opa(ui->screen_weather_img_tomorrow, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_image_opa(ui->screen_weather_img_tomorrow, 255, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_weather_img_today
    ui->screen_weather_img_today = lv_image_create(ui->screen_weather);
    lv_obj_set_pos(ui->screen_weather_img_today, 15, 104);
    lv_obj_set_size(ui->screen_weather_img_today, 58, 57);
    lv_obj_add_flag(ui->screen_weather_img_today, LV_OBJ_FLAG_CLICKABLE);
    lv_image_set_src(ui->screen_weather_img_today, &_qing_RGB565A8_58x57);
    lv_image_set_pivot(ui->screen_weather_img_today, 50,50);
    lv_image_set_rotation(ui->screen_weather_img_today, 0);

    //Write style for screen_weather_img_today, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_image_recolor_opa(ui->screen_weather_img_today, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_image_opa(ui->screen_weather_img_today, 255, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_weather_img_later
    ui->screen_weather_img_later = lv_image_create(ui->screen_weather);
    lv_obj_set_pos(ui->screen_weather_img_later, 169, 104);
    lv_obj_set_size(ui->screen_weather_img_later, 57, 50);
    lv_obj_add_flag(ui->screen_weather_img_later, LV_OBJ_FLAG_CLICKABLE);
    lv_image_set_src(ui->screen_weather_img_later, &_yun_RGB565A8_57x50);
    lv_image_set_pivot(ui->screen_weather_img_later, 50,50);
    lv_image_set_rotation(ui->screen_weather_img_later, 0);

    //Write style for screen_weather_img_later, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_image_recolor_opa(ui->screen_weather_img_later, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_image_opa(ui->screen_weather_img_later, 255, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_weather_label_today1
    ui->screen_weather_label_today1 = lv_label_create(ui->screen_weather);
    lv_obj_set_pos(ui->screen_weather_label_today1, 15, 175);
    lv_obj_set_size(ui->screen_weather_label_today1, 71, 17);
    lv_label_set_text(ui->screen_weather_label_today1, "--");
    lv_label_set_long_mode(ui->screen_weather_label_today1, LV_LABEL_LONG_WRAP);

    //Write style for screen_weather_label_today1, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_weather_label_today1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_weather_label_today1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_weather_label_today1, lv_color_hex(APP_THEME_TEXT), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_weather_label_today1, &lv_customer_font_ZiTiQuanWeiJunHeiW22_12, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_weather_label_today1, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->screen_weather_label_today1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->screen_weather_label_today1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_weather_label_today1, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_weather_label_today1, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_weather_label_today1, lv_color_hex(APP_THEME_BG), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_weather_label_today1, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_weather_label_today1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_weather_label_today1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_weather_label_today1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_weather_label_today1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_weather_label_today1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_weather_label_tomorrow2
    ui->screen_weather_label_tomorrow2 = lv_label_create(ui->screen_weather);
    lv_obj_set_pos(ui->screen_weather_label_tomorrow2, 89, 175);
    lv_obj_set_size(ui->screen_weather_label_tomorrow2, 71, 17);
    lv_label_set_text(ui->screen_weather_label_tomorrow2, "--");
    lv_label_set_long_mode(ui->screen_weather_label_tomorrow2, LV_LABEL_LONG_WRAP);

    //Write style for screen_weather_label_tomorrow2, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_weather_label_tomorrow2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_weather_label_tomorrow2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_weather_label_tomorrow2, lv_color_hex(APP_THEME_TEXT), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_weather_label_tomorrow2, &lv_customer_font_ZiTiQuanWeiJunHeiW22_12, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_weather_label_tomorrow2, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->screen_weather_label_tomorrow2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->screen_weather_label_tomorrow2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_weather_label_tomorrow2, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_weather_label_tomorrow2, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_weather_label_tomorrow2, lv_color_hex(APP_THEME_BG), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_weather_label_tomorrow2, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_weather_label_tomorrow2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_weather_label_tomorrow2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_weather_label_tomorrow2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_weather_label_tomorrow2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_weather_label_tomorrow2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_weather_label_later2
    ui->screen_weather_label_later2 = lv_label_create(ui->screen_weather);
    lv_obj_set_pos(ui->screen_weather_label_later2, 166, 175);
    lv_obj_set_size(ui->screen_weather_label_later2, 71, 17);
    lv_label_set_text(ui->screen_weather_label_later2, "--");
    lv_label_set_long_mode(ui->screen_weather_label_later2, LV_LABEL_LONG_WRAP);

    //Write style for screen_weather_label_later2, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_weather_label_later2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_weather_label_later2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_weather_label_later2, lv_color_hex(APP_THEME_TEXT), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_weather_label_later2, &lv_customer_font_ZiTiQuanWeiJunHeiW22_12, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_weather_label_later2, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->screen_weather_label_later2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->screen_weather_label_later2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_weather_label_later2, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_weather_label_later2, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_weather_label_later2, lv_color_hex(APP_THEME_BG), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_weather_label_later2, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_weather_label_later2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_weather_label_later2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_weather_label_later2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_weather_label_later2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_weather_label_later2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_weather_btn_return
    ui->screen_weather_btn_return = lv_button_create(ui->screen_weather);
    lv_obj_set_pos(ui->screen_weather_btn_return, 83, 228);
    lv_obj_set_size(ui->screen_weather_btn_return, 76, 37);
    ui->screen_weather_btn_return_label = lv_label_create(ui->screen_weather_btn_return);
    lv_label_set_text(ui->screen_weather_btn_return_label, "返回");
    lv_label_set_long_mode(ui->screen_weather_btn_return_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(ui->screen_weather_btn_return_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(ui->screen_weather_btn_return, 0, LV_STATE_DEFAULT);
    lv_obj_set_width(ui->screen_weather_btn_return_label, LV_PCT(100));

    //Write style for screen_weather_btn_return, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_weather_btn_return, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_weather_btn_return, lv_color_hex(APP_THEME_ACCENT), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_weather_btn_return, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->screen_weather_btn_return, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_weather_btn_return, 5, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_weather_btn_return, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_weather_btn_return, lv_color_hex(APP_THEME_TEXT), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_weather_btn_return, &lv_font_ZiTiQuanWeiJunHeiW22_18, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_weather_btn_return, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_weather_btn_return, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_weather_label_city
    ui->screen_weather_label_city = lv_label_create(ui->screen_weather);
    lv_obj_set_pos(ui->screen_weather_label_city, 0, 8);
    lv_obj_set_size(ui->screen_weather_label_city, 120, 22);
    lv_label_set_text(ui->screen_weather_label_city, "定位中");
    lv_label_set_long_mode(ui->screen_weather_label_city, LV_LABEL_LONG_WRAP);

    //Write style for screen_weather_label_city, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_weather_label_city, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_weather_label_city, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_weather_label_city, lv_color_hex(APP_THEME_TEXT), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_weather_label_city, &lv_customer_font_ZiTiQuanWeiJunHeiW22_18, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_weather_label_city, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->screen_weather_label_city, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->screen_weather_label_city, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_weather_label_city, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_weather_label_city, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_weather_label_city, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_weather_label_city, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_weather_label_city, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_weather_label_city, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_weather_label_city, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_weather_img_1
    ui->screen_weather_img_1 = lv_image_create(ui->screen_weather);
    lv_obj_set_pos(ui->screen_weather_img_1, 0, 80);
    lv_obj_set_size(ui->screen_weather_img_1, 218, 37);
    lv_obj_add_flag(ui->screen_weather_img_1, LV_OBJ_FLAG_CLICKABLE);
    lv_image_set_src(ui->screen_weather_img_1, &_line_RGB565A8_218x37);
    lv_image_set_pivot(ui->screen_weather_img_1, 50,50);
    lv_image_set_rotation(ui->screen_weather_img_1, 0);

    //Write style for screen_weather_img_1, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_image_recolor_opa(ui->screen_weather_img_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_image_opa(ui->screen_weather_img_1, 255, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_weather_list_bettery
    ui->screen_weather_list_bettery = lv_list_create(ui->screen_weather);
    lv_obj_set_pos(ui->screen_weather_list_bettery, 170, -2);
    lv_obj_set_size(ui->screen_weather_list_bettery, 64, 26);
    lv_obj_set_scrollbar_mode(ui->screen_weather_list_bettery, LV_SCROLLBAR_MODE_OFF);
    ui->screen_weather_list_bettery_item0 = lv_list_add_button(ui->screen_weather_list_bettery, NULL, "--");

    //Write style state: LV_STATE_DEFAULT for &style_screen_weather_list_bettery_main_main_default
    static lv_style_t style_screen_weather_list_bettery_main_main_default;
    ui_init_style(&style_screen_weather_list_bettery_main_main_default);

    lv_style_set_pad_top(&style_screen_weather_list_bettery_main_main_default, 5);
    lv_style_set_pad_left(&style_screen_weather_list_bettery_main_main_default, 5);
    lv_style_set_pad_right(&style_screen_weather_list_bettery_main_main_default, 5);
    lv_style_set_pad_bottom(&style_screen_weather_list_bettery_main_main_default, 5);
    lv_style_set_bg_opa(&style_screen_weather_list_bettery_main_main_default, 255);
    lv_style_set_bg_color(&style_screen_weather_list_bettery_main_main_default, lv_color_hex(APP_THEME_BG));
    lv_style_set_bg_grad_dir(&style_screen_weather_list_bettery_main_main_default, LV_GRAD_DIR_NONE);
    lv_style_set_border_width(&style_screen_weather_list_bettery_main_main_default, 1);
    lv_style_set_border_opa(&style_screen_weather_list_bettery_main_main_default, 255);
    lv_style_set_border_color(&style_screen_weather_list_bettery_main_main_default, lv_color_hex(APP_THEME_BG));
    lv_style_set_border_side(&style_screen_weather_list_bettery_main_main_default, LV_BORDER_SIDE_FULL);
    lv_style_set_radius(&style_screen_weather_list_bettery_main_main_default, 3);
    lv_style_set_shadow_width(&style_screen_weather_list_bettery_main_main_default, 0);
    lv_obj_add_style(ui->screen_weather_list_bettery, &style_screen_weather_list_bettery_main_main_default, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style state: LV_STATE_DEFAULT for &style_screen_weather_list_bettery_main_scrollbar_default
    static lv_style_t style_screen_weather_list_bettery_main_scrollbar_default;
    ui_init_style(&style_screen_weather_list_bettery_main_scrollbar_default);

    lv_style_set_radius(&style_screen_weather_list_bettery_main_scrollbar_default, 3);
    lv_style_set_bg_opa(&style_screen_weather_list_bettery_main_scrollbar_default, 255);
    lv_style_set_bg_color(&style_screen_weather_list_bettery_main_scrollbar_default, lv_color_hex(APP_THEME_BG));
    lv_style_set_bg_grad_dir(&style_screen_weather_list_bettery_main_scrollbar_default, LV_GRAD_DIR_NONE);
    lv_obj_add_style(ui->screen_weather_list_bettery, &style_screen_weather_list_bettery_main_scrollbar_default, LV_PART_SCROLLBAR|LV_STATE_DEFAULT);

    //Write style state: LV_STATE_DEFAULT for &style_screen_weather_list_bettery_extra_btns_main_default
    static lv_style_t style_screen_weather_list_bettery_extra_btns_main_default;
    ui_init_style(&style_screen_weather_list_bettery_extra_btns_main_default);

    lv_style_set_pad_top(&style_screen_weather_list_bettery_extra_btns_main_default, 5);
    lv_style_set_pad_left(&style_screen_weather_list_bettery_extra_btns_main_default, 5);
    lv_style_set_pad_right(&style_screen_weather_list_bettery_extra_btns_main_default, 5);
    lv_style_set_pad_bottom(&style_screen_weather_list_bettery_extra_btns_main_default, 5);
    lv_style_set_border_width(&style_screen_weather_list_bettery_extra_btns_main_default, 0);
    lv_style_set_text_color(&style_screen_weather_list_bettery_extra_btns_main_default, lv_color_hex(APP_THEME_TEXT));
    lv_style_set_text_font(&style_screen_weather_list_bettery_extra_btns_main_default, &lv_font_ZiTiQuanWeiJunHeiW22_12);
    lv_style_set_text_opa(&style_screen_weather_list_bettery_extra_btns_main_default, 255);
    lv_style_set_radius(&style_screen_weather_list_bettery_extra_btns_main_default, 3);
    lv_style_set_bg_opa(&style_screen_weather_list_bettery_extra_btns_main_default, 255);
    lv_style_set_bg_color(&style_screen_weather_list_bettery_extra_btns_main_default, lv_color_hex(APP_THEME_BG));
    lv_style_set_bg_grad_dir(&style_screen_weather_list_bettery_extra_btns_main_default, LV_GRAD_DIR_NONE);
    lv_obj_add_style(ui->screen_weather_list_bettery_item0, &style_screen_weather_list_bettery_extra_btns_main_default, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style state: LV_STATE_DEFAULT for &style_screen_weather_list_bettery_extra_texts_main_default
    static lv_style_t style_screen_weather_list_bettery_extra_texts_main_default;
    ui_init_style(&style_screen_weather_list_bettery_extra_texts_main_default);

    lv_style_set_pad_top(&style_screen_weather_list_bettery_extra_texts_main_default, 5);
    lv_style_set_pad_left(&style_screen_weather_list_bettery_extra_texts_main_default, 5);
    lv_style_set_pad_right(&style_screen_weather_list_bettery_extra_texts_main_default, 5);
    lv_style_set_pad_bottom(&style_screen_weather_list_bettery_extra_texts_main_default, 5);
    lv_style_set_border_width(&style_screen_weather_list_bettery_extra_texts_main_default, 0);
    lv_style_set_text_color(&style_screen_weather_list_bettery_extra_texts_main_default, lv_color_hex(APP_THEME_TEXT));
    lv_style_set_text_font(&style_screen_weather_list_bettery_extra_texts_main_default, &lv_font_ZiTiQuanWeiJunHeiW22_12);
    lv_style_set_text_opa(&style_screen_weather_list_bettery_extra_texts_main_default, 255);
    lv_style_set_radius(&style_screen_weather_list_bettery_extra_texts_main_default, 3);
    lv_style_set_transform_width(&style_screen_weather_list_bettery_extra_texts_main_default, 0);
    lv_style_set_bg_opa(&style_screen_weather_list_bettery_extra_texts_main_default, 255);
    lv_style_set_bg_color(&style_screen_weather_list_bettery_extra_texts_main_default, lv_color_hex(APP_THEME_CARD));
    lv_style_set_bg_grad_dir(&style_screen_weather_list_bettery_extra_texts_main_default, LV_GRAD_DIR_NONE);

    //Write codes screen_weather_img_wificlose
    ui->screen_weather_img_wificlose = lv_image_create(ui->screen_weather);
    lv_obj_set_pos(ui->screen_weather_img_wificlose, 150, 4);
    lv_obj_set_size(ui->screen_weather_img_wificlose, 20, 24);
    lv_obj_add_flag(ui->screen_weather_img_wificlose, LV_OBJ_FLAG_CLICKABLE);
    lv_image_set_src(ui->screen_weather_img_wificlose, &_wifi_close_RGB565A8_20x24);
    lv_image_set_pivot(ui->screen_weather_img_wificlose, 50,50);
    lv_image_set_rotation(ui->screen_weather_img_wificlose, 0);

    //Write style for screen_weather_img_wificlose, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_image_recolor_opa(ui->screen_weather_img_wificlose, 26, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_image_recolor(ui->screen_weather_img_wificlose, lv_color_hex(0x090000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_image_opa(ui->screen_weather_img_wificlose, 255, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_weather_datetext_1
    ui->screen_weather_datetext_1 = lv_label_create(ui->screen_weather);
    lv_obj_set_pos(ui->screen_weather_datetext_1, 56, 36);
    lv_obj_set_size(ui->screen_weather_datetext_1, 130, 23);
    lv_label_set_text(ui->screen_weather_datetext_1, "--/--/--");
    lv_obj_set_style_text_align(ui->screen_weather_datetext_1, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_add_flag(ui->screen_weather_datetext_1, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_event_cb(ui->screen_weather_datetext_1, screen_weather_datetext_1_event_handler, LV_EVENT_ALL, NULL);

    //Write style for screen_weather_datetext_1, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_text_color(ui->screen_weather_datetext_1, lv_color_hex(APP_THEME_TEXT), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_weather_datetext_1, &lv_font_ZiTiQuanWeiJunHeiW22_18, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_weather_datetext_1, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->screen_weather_datetext_1, 2, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_weather_datetext_1, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_weather_datetext_1, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_weather_datetext_1, lv_color_hex(APP_THEME_BG), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_weather_datetext_1, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->screen_weather_datetext_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_weather_datetext_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_weather_datetext_1, 7, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_weather_datetext_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_weather_datetext_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_weather_datetext_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //The custom code of screen_weather.

    weather_refresh_screen(ui);


    //Update current screen layout.
    lv_obj_update_layout(ui->screen_weather);

    //Init events for screen.
    events_init_screen_weather(ui);
}
