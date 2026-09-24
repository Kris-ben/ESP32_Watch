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



int screen_wifi_digital_clock_time_min_value = 25;
int screen_wifi_digital_clock_time_hour_value = 11;
int screen_wifi_digital_clock_time_sec_value = 50;
void setup_scr_screen_wifi(lv_ui *ui)
{
    //Write codes screen_wifi
    ui->screen_wifi = lv_obj_create(NULL);
    lv_obj_set_size(ui->screen_wifi, 240, 284);
    lv_obj_set_scrollbar_mode(ui->screen_wifi, LV_SCROLLBAR_MODE_OFF);

    //Write style for screen_wifi, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_wifi, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_wifi, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_wifi, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_wifi_list_wifi
    ui->screen_wifi_list_wifi = lv_list_create(ui->screen_wifi);
    lv_obj_set_pos(ui->screen_wifi_list_wifi, 2, 41);
    lv_obj_set_size(ui->screen_wifi_list_wifi, 235, 169);
    lv_obj_set_scrollbar_mode(ui->screen_wifi_list_wifi, LV_SCROLLBAR_MODE_OFF);
    ui->screen_wifi_list_wifi_item0 = lv_list_add_button(ui->screen_wifi_list_wifi, LV_SYMBOL_WIFI, "wifi");
    ui->screen_wifi_list_wifi_item1 = lv_list_add_button(ui->screen_wifi_list_wifi, LV_SYMBOL_WIFI, "wifi_1");
    ui->screen_wifi_list_wifi_item2 = lv_list_add_button(ui->screen_wifi_list_wifi, LV_SYMBOL_WIFI, "wifi_2");
    ui->screen_wifi_list_wifi_item3 = lv_list_add_button(ui->screen_wifi_list_wifi, LV_SYMBOL_WIFI, "wifi_3");
    ui->screen_wifi_list_wifi_item4 = lv_list_add_button(ui->screen_wifi_list_wifi, LV_SYMBOL_WIFI, "wifi_4");

    //Write style state: LV_STATE_DEFAULT for &style_screen_wifi_list_wifi_main_main_default
    static lv_style_t style_screen_wifi_list_wifi_main_main_default;
    ui_init_style(&style_screen_wifi_list_wifi_main_main_default);

    lv_style_set_pad_top(&style_screen_wifi_list_wifi_main_main_default, 5);
    lv_style_set_pad_left(&style_screen_wifi_list_wifi_main_main_default, 5);
    lv_style_set_pad_right(&style_screen_wifi_list_wifi_main_main_default, 5);
    lv_style_set_pad_bottom(&style_screen_wifi_list_wifi_main_main_default, 5);
    lv_style_set_bg_opa(&style_screen_wifi_list_wifi_main_main_default, 255);
    lv_style_set_bg_color(&style_screen_wifi_list_wifi_main_main_default, lv_color_hex(0xffffff));
    lv_style_set_bg_grad_dir(&style_screen_wifi_list_wifi_main_main_default, LV_GRAD_DIR_NONE);
    lv_style_set_border_width(&style_screen_wifi_list_wifi_main_main_default, 1);
    lv_style_set_border_opa(&style_screen_wifi_list_wifi_main_main_default, 255);
    lv_style_set_border_color(&style_screen_wifi_list_wifi_main_main_default, lv_color_hex(0xe1e6ee));
    lv_style_set_border_side(&style_screen_wifi_list_wifi_main_main_default, LV_BORDER_SIDE_FULL);
    lv_style_set_radius(&style_screen_wifi_list_wifi_main_main_default, 3);
    lv_style_set_shadow_width(&style_screen_wifi_list_wifi_main_main_default, 0);
    lv_obj_add_style(ui->screen_wifi_list_wifi, &style_screen_wifi_list_wifi_main_main_default, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style state: LV_STATE_DEFAULT for &style_screen_wifi_list_wifi_main_scrollbar_default
    static lv_style_t style_screen_wifi_list_wifi_main_scrollbar_default;
    ui_init_style(&style_screen_wifi_list_wifi_main_scrollbar_default);

    lv_style_set_radius(&style_screen_wifi_list_wifi_main_scrollbar_default, 3);
    lv_style_set_bg_opa(&style_screen_wifi_list_wifi_main_scrollbar_default, 255);
    lv_style_set_bg_color(&style_screen_wifi_list_wifi_main_scrollbar_default, lv_color_hex(0xffffff));
    lv_style_set_bg_grad_dir(&style_screen_wifi_list_wifi_main_scrollbar_default, LV_GRAD_DIR_NONE);
    lv_obj_add_style(ui->screen_wifi_list_wifi, &style_screen_wifi_list_wifi_main_scrollbar_default, LV_PART_SCROLLBAR|LV_STATE_DEFAULT);

    //Write style state: LV_STATE_DEFAULT for &style_screen_wifi_list_wifi_extra_btns_main_default
    static lv_style_t style_screen_wifi_list_wifi_extra_btns_main_default;
    ui_init_style(&style_screen_wifi_list_wifi_extra_btns_main_default);

    lv_style_set_pad_top(&style_screen_wifi_list_wifi_extra_btns_main_default, 5);
    lv_style_set_pad_left(&style_screen_wifi_list_wifi_extra_btns_main_default, 5);
    lv_style_set_pad_right(&style_screen_wifi_list_wifi_extra_btns_main_default, 5);
    lv_style_set_pad_bottom(&style_screen_wifi_list_wifi_extra_btns_main_default, 5);
    lv_style_set_border_width(&style_screen_wifi_list_wifi_extra_btns_main_default, 0);
    lv_style_set_text_color(&style_screen_wifi_list_wifi_extra_btns_main_default, lv_color_hex(0x0D3055));
    lv_style_set_text_font(&style_screen_wifi_list_wifi_extra_btns_main_default, &lv_font_ZiTiQuanWeiJunHeiW22_18);
    lv_style_set_text_opa(&style_screen_wifi_list_wifi_extra_btns_main_default, 255);
    lv_style_set_radius(&style_screen_wifi_list_wifi_extra_btns_main_default, 3);
    lv_style_set_bg_opa(&style_screen_wifi_list_wifi_extra_btns_main_default, 255);
    lv_style_set_bg_color(&style_screen_wifi_list_wifi_extra_btns_main_default, lv_color_hex(0xffffff));
    lv_style_set_bg_grad_dir(&style_screen_wifi_list_wifi_extra_btns_main_default, LV_GRAD_DIR_NONE);
    lv_obj_add_style(ui->screen_wifi_list_wifi_item4, &style_screen_wifi_list_wifi_extra_btns_main_default, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_add_style(ui->screen_wifi_list_wifi_item3, &style_screen_wifi_list_wifi_extra_btns_main_default, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_add_style(ui->screen_wifi_list_wifi_item2, &style_screen_wifi_list_wifi_extra_btns_main_default, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_add_style(ui->screen_wifi_list_wifi_item1, &style_screen_wifi_list_wifi_extra_btns_main_default, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_add_style(ui->screen_wifi_list_wifi_item0, &style_screen_wifi_list_wifi_extra_btns_main_default, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style state: LV_STATE_DEFAULT for &style_screen_wifi_list_wifi_extra_texts_main_default
    static lv_style_t style_screen_wifi_list_wifi_extra_texts_main_default;
    ui_init_style(&style_screen_wifi_list_wifi_extra_texts_main_default);

    lv_style_set_pad_top(&style_screen_wifi_list_wifi_extra_texts_main_default, 5);
    lv_style_set_pad_left(&style_screen_wifi_list_wifi_extra_texts_main_default, 5);
    lv_style_set_pad_right(&style_screen_wifi_list_wifi_extra_texts_main_default, 5);
    lv_style_set_pad_bottom(&style_screen_wifi_list_wifi_extra_texts_main_default, 5);
    lv_style_set_border_width(&style_screen_wifi_list_wifi_extra_texts_main_default, 0);
    lv_style_set_text_color(&style_screen_wifi_list_wifi_extra_texts_main_default, lv_color_hex(0x000000));
    lv_style_set_text_font(&style_screen_wifi_list_wifi_extra_texts_main_default, &lv_font_ZiTiQuanWeiJunHeiW22_24);
    lv_style_set_text_opa(&style_screen_wifi_list_wifi_extra_texts_main_default, 255);
    lv_style_set_radius(&style_screen_wifi_list_wifi_extra_texts_main_default, 3);
    lv_style_set_transform_width(&style_screen_wifi_list_wifi_extra_texts_main_default, 0);
    lv_style_set_bg_opa(&style_screen_wifi_list_wifi_extra_texts_main_default, 255);
    lv_style_set_bg_color(&style_screen_wifi_list_wifi_extra_texts_main_default, lv_color_hex(0xffffff));
    lv_style_set_bg_grad_dir(&style_screen_wifi_list_wifi_extra_texts_main_default, LV_GRAD_DIR_NONE);

    //Write codes screen_wifi_btn_scanf
    ui->screen_wifi_btn_scanf = lv_button_create(ui->screen_wifi);
    lv_obj_set_pos(ui->screen_wifi_btn_scanf, 17, 219);
    lv_obj_set_size(ui->screen_wifi_btn_scanf, 76, 29);
    ui->screen_wifi_btn_scanf_label = lv_label_create(ui->screen_wifi_btn_scanf);
    lv_label_set_text(ui->screen_wifi_btn_scanf_label, "扫描WiFi");
    lv_label_set_long_mode(ui->screen_wifi_btn_scanf_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(ui->screen_wifi_btn_scanf_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(ui->screen_wifi_btn_scanf, 0, LV_STATE_DEFAULT);
    lv_obj_set_width(ui->screen_wifi_btn_scanf_label, LV_PCT(100));

    //Write style for screen_wifi_btn_scanf, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_wifi_btn_scanf, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_wifi_btn_scanf, lv_color_hex(0x2195f6), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_wifi_btn_scanf, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->screen_wifi_btn_scanf, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_wifi_btn_scanf, 5, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_wifi_btn_scanf, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_wifi_btn_scanf, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_wifi_btn_scanf, &lv_font_ZiTiQuanWeiJunHeiW22_18, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_wifi_btn_scanf, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_wifi_btn_scanf, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_wifi_digital_clock_time
    static bool screen_wifi_digital_clock_time_timer_enabled = false;
    ui->screen_wifi_digital_clock_time = lv_label_create(ui->screen_wifi);
    lv_obj_set_pos(ui->screen_wifi_digital_clock_time, 12, 4);
    lv_obj_set_size(ui->screen_wifi_digital_clock_time, 48, 29);
    lv_label_set_text(ui->screen_wifi_digital_clock_time, "11:25");
    if (!screen_wifi_digital_clock_time_timer_enabled) {
        lv_timer_create(screen_wifi_digital_clock_time_timer, 1000, NULL);
        screen_wifi_digital_clock_time_timer_enabled = true;
    }

    //Write style for screen_wifi_digital_clock_time, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_radius(ui->screen_wifi_digital_clock_time, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_wifi_digital_clock_time, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_wifi_digital_clock_time, &lv_font_ZiTiQuanWeiJunHeiW22_18, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_wifi_digital_clock_time, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->screen_wifi_digital_clock_time, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_wifi_digital_clock_time, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_wifi_digital_clock_time, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_wifi_digital_clock_time, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_wifi_digital_clock_time, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_wifi_digital_clock_time, 7, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_wifi_digital_clock_time, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_wifi_digital_clock_time, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_wifi_digital_clock_time, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_wifi_digital_clock_time, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_wifi_img_wi_close
    ui->screen_wifi_img_wi_close = lv_image_create(ui->screen_wifi);
    lv_obj_set_pos(ui->screen_wifi_img_wi_close, 134, 4);
    lv_obj_set_size(ui->screen_wifi_img_wi_close, 23, 26);
    lv_obj_add_flag(ui->screen_wifi_img_wi_close, LV_OBJ_FLAG_CLICKABLE);
    lv_image_set_src(ui->screen_wifi_img_wi_close, &_wifi_close_RGB565A8_23x26);
    lv_image_set_pivot(ui->screen_wifi_img_wi_close, 50,50);
    lv_image_set_rotation(ui->screen_wifi_img_wi_close, 0);

    //Write style for screen_wifi_img_wi_close, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_image_recolor_opa(ui->screen_wifi_img_wi_close, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_image_opa(ui->screen_wifi_img_wi_close, 255, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_wifi_btn_return
    ui->screen_wifi_btn_return = lv_button_create(ui->screen_wifi);
    lv_obj_set_pos(ui->screen_wifi_btn_return, 139, 219);
    lv_obj_set_size(ui->screen_wifi_btn_return, 76, 29);
    ui->screen_wifi_btn_return_label = lv_label_create(ui->screen_wifi_btn_return);
    lv_label_set_text(ui->screen_wifi_btn_return_label, "返回");
    lv_label_set_long_mode(ui->screen_wifi_btn_return_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(ui->screen_wifi_btn_return_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(ui->screen_wifi_btn_return, 0, LV_STATE_DEFAULT);
    lv_obj_set_width(ui->screen_wifi_btn_return_label, LV_PCT(100));

    //Write style for screen_wifi_btn_return, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_wifi_btn_return, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_wifi_btn_return, lv_color_hex(0x2195f6), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_wifi_btn_return, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->screen_wifi_btn_return, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_wifi_btn_return, 5, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_wifi_btn_return, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_wifi_btn_return, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_wifi_btn_return, &lv_font_ZiTiQuanWeiJunHeiW22_18, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_wifi_btn_return, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_wifi_btn_return, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_wifi_list_battery
    ui->screen_wifi_list_battery = lv_list_create(ui->screen_wifi);
    lv_obj_set_pos(ui->screen_wifi_list_battery, 163, 0);
    lv_obj_set_size(ui->screen_wifi_list_battery, 68, 33);
    lv_obj_set_scrollbar_mode(ui->screen_wifi_list_battery, LV_SCROLLBAR_MODE_OFF);
    ui->screen_wifi_list_battery_item0 = lv_list_add_button(ui->screen_wifi_list_battery, NULL, "--");

    //Write style state: LV_STATE_DEFAULT for &style_screen_wifi_list_battery_main_main_default
    static lv_style_t style_screen_wifi_list_battery_main_main_default;
    ui_init_style(&style_screen_wifi_list_battery_main_main_default);

    lv_style_set_pad_top(&style_screen_wifi_list_battery_main_main_default, 5);
    lv_style_set_pad_left(&style_screen_wifi_list_battery_main_main_default, 5);
    lv_style_set_pad_right(&style_screen_wifi_list_battery_main_main_default, 5);
    lv_style_set_pad_bottom(&style_screen_wifi_list_battery_main_main_default, 5);
    lv_style_set_bg_opa(&style_screen_wifi_list_battery_main_main_default, 255);
    lv_style_set_bg_color(&style_screen_wifi_list_battery_main_main_default, lv_color_hex(0x050505));
    lv_style_set_bg_grad_dir(&style_screen_wifi_list_battery_main_main_default, LV_GRAD_DIR_NONE);
    lv_style_set_border_width(&style_screen_wifi_list_battery_main_main_default, 1);
    lv_style_set_border_opa(&style_screen_wifi_list_battery_main_main_default, 255);
    lv_style_set_border_color(&style_screen_wifi_list_battery_main_main_default, lv_color_hex(0x000000));
    lv_style_set_border_side(&style_screen_wifi_list_battery_main_main_default, LV_BORDER_SIDE_FULL);
    lv_style_set_radius(&style_screen_wifi_list_battery_main_main_default, 3);
    lv_style_set_shadow_width(&style_screen_wifi_list_battery_main_main_default, 0);
    lv_obj_add_style(ui->screen_wifi_list_battery, &style_screen_wifi_list_battery_main_main_default, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style state: LV_STATE_DEFAULT for &style_screen_wifi_list_battery_main_scrollbar_default
    static lv_style_t style_screen_wifi_list_battery_main_scrollbar_default;
    ui_init_style(&style_screen_wifi_list_battery_main_scrollbar_default);

    lv_style_set_radius(&style_screen_wifi_list_battery_main_scrollbar_default, 3);
    lv_style_set_bg_opa(&style_screen_wifi_list_battery_main_scrollbar_default, 255);
    lv_style_set_bg_color(&style_screen_wifi_list_battery_main_scrollbar_default, lv_color_hex(0x000000));
    lv_style_set_bg_grad_dir(&style_screen_wifi_list_battery_main_scrollbar_default, LV_GRAD_DIR_NONE);
    lv_obj_add_style(ui->screen_wifi_list_battery, &style_screen_wifi_list_battery_main_scrollbar_default, LV_PART_SCROLLBAR|LV_STATE_DEFAULT);

    //Write style state: LV_STATE_DEFAULT for &style_screen_wifi_list_battery_extra_btns_main_default
    static lv_style_t style_screen_wifi_list_battery_extra_btns_main_default;
    ui_init_style(&style_screen_wifi_list_battery_extra_btns_main_default);

    lv_style_set_pad_top(&style_screen_wifi_list_battery_extra_btns_main_default, 5);
    lv_style_set_pad_left(&style_screen_wifi_list_battery_extra_btns_main_default, 5);
    lv_style_set_pad_right(&style_screen_wifi_list_battery_extra_btns_main_default, 5);
    lv_style_set_pad_bottom(&style_screen_wifi_list_battery_extra_btns_main_default, 5);
    lv_style_set_border_width(&style_screen_wifi_list_battery_extra_btns_main_default, 0);
    lv_style_set_text_color(&style_screen_wifi_list_battery_extra_btns_main_default, lv_color_hex(0xfbfbfb));
    lv_style_set_text_font(&style_screen_wifi_list_battery_extra_btns_main_default, &lv_font_ZiTiQuanWeiJunHeiW22_12);
    lv_style_set_text_opa(&style_screen_wifi_list_battery_extra_btns_main_default, 255);
    lv_style_set_radius(&style_screen_wifi_list_battery_extra_btns_main_default, 3);
    lv_style_set_bg_opa(&style_screen_wifi_list_battery_extra_btns_main_default, 255);
    lv_style_set_bg_color(&style_screen_wifi_list_battery_extra_btns_main_default, lv_color_hex(0x000000));
    lv_style_set_bg_grad_dir(&style_screen_wifi_list_battery_extra_btns_main_default, LV_GRAD_DIR_NONE);
    lv_obj_add_style(ui->screen_wifi_list_battery_item0, &style_screen_wifi_list_battery_extra_btns_main_default, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style state: LV_STATE_DEFAULT for &style_screen_wifi_list_battery_extra_texts_main_default
    static lv_style_t style_screen_wifi_list_battery_extra_texts_main_default;
    ui_init_style(&style_screen_wifi_list_battery_extra_texts_main_default);

    lv_style_set_pad_top(&style_screen_wifi_list_battery_extra_texts_main_default, 5);
    lv_style_set_pad_left(&style_screen_wifi_list_battery_extra_texts_main_default, 5);
    lv_style_set_pad_right(&style_screen_wifi_list_battery_extra_texts_main_default, 5);
    lv_style_set_pad_bottom(&style_screen_wifi_list_battery_extra_texts_main_default, 5);
    lv_style_set_border_width(&style_screen_wifi_list_battery_extra_texts_main_default, 0);
    lv_style_set_text_color(&style_screen_wifi_list_battery_extra_texts_main_default, lv_color_hex(0xf9f9f9));
    lv_style_set_text_font(&style_screen_wifi_list_battery_extra_texts_main_default, &lv_font_ZiTiQuanWeiJunHeiW22_12);
    lv_style_set_text_opa(&style_screen_wifi_list_battery_extra_texts_main_default, 255);
    lv_style_set_radius(&style_screen_wifi_list_battery_extra_texts_main_default, 3);
    lv_style_set_transform_width(&style_screen_wifi_list_battery_extra_texts_main_default, 0);
    lv_style_set_bg_opa(&style_screen_wifi_list_battery_extra_texts_main_default, 255);
    lv_style_set_bg_color(&style_screen_wifi_list_battery_extra_texts_main_default, lv_color_hex(0xffffff));
    lv_style_set_bg_grad_dir(&style_screen_wifi_list_battery_extra_texts_main_default, LV_GRAD_DIR_NONE);

    //The custom code of screen_wifi.
    lv_obj_null_on_delete(&ui->screen_wifi_digital_clock_time);

    //Update current screen layout.
    lv_obj_update_layout(ui->screen_wifi);

    //Init events for screen.
    events_init_screen_wifi(ui);
}
