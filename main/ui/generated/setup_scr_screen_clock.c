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



void setup_scr_screen_clock(lv_ui *ui)
{
    //Write codes screen_clock
    ui->screen_clock = lv_obj_create(NULL);
    lv_obj_set_size(ui->screen_clock, 240, 284);
    lv_obj_set_scrollbar_mode(ui->screen_clock, LV_SCROLLBAR_MODE_OFF);

    //Write style for screen_clock, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_clock, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_clock, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_clock, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_clock_list_clock
    ui->screen_clock_list_clock = lv_list_create(ui->screen_clock);
    lv_obj_set_pos(ui->screen_clock_list_clock, 29, 54);
    lv_obj_set_size(ui->screen_clock_list_clock, 106, 157);
    lv_obj_set_scrollbar_mode(ui->screen_clock_list_clock, LV_SCROLLBAR_MODE_OFF);
    ui->screen_clock_list_clock_item0 = lv_list_add_button(ui->screen_clock_list_clock, LV_SYMBOL_BELL, "8:00");
    ui->screen_clock_list_clock_item1 = lv_list_add_button(ui->screen_clock_list_clock, LV_SYMBOL_BELL, "12:00");
    ui->screen_clock_list_clock_item2 = lv_list_add_button(ui->screen_clock_list_clock, LV_SYMBOL_BELL, "14:00");
    ui->screen_clock_list_clock_item3 = lv_list_add_button(ui->screen_clock_list_clock, LV_SYMBOL_BELL, "19:00");

    //Write style state: LV_STATE_DEFAULT for &style_screen_clock_list_clock_main_main_default
    static lv_style_t style_screen_clock_list_clock_main_main_default;
    ui_init_style(&style_screen_clock_list_clock_main_main_default);

    lv_style_set_pad_top(&style_screen_clock_list_clock_main_main_default, 5);
    lv_style_set_pad_left(&style_screen_clock_list_clock_main_main_default, 5);
    lv_style_set_pad_right(&style_screen_clock_list_clock_main_main_default, 5);
    lv_style_set_pad_bottom(&style_screen_clock_list_clock_main_main_default, 5);
    lv_style_set_bg_opa(&style_screen_clock_list_clock_main_main_default, 255);
    lv_style_set_bg_color(&style_screen_clock_list_clock_main_main_default, lv_color_hex(0xffffff));
    lv_style_set_bg_grad_dir(&style_screen_clock_list_clock_main_main_default, LV_GRAD_DIR_NONE);
    lv_style_set_border_width(&style_screen_clock_list_clock_main_main_default, 1);
    lv_style_set_border_opa(&style_screen_clock_list_clock_main_main_default, 255);
    lv_style_set_border_color(&style_screen_clock_list_clock_main_main_default, lv_color_hex(0xe1e6ee));
    lv_style_set_border_side(&style_screen_clock_list_clock_main_main_default, LV_BORDER_SIDE_FULL);
    lv_style_set_radius(&style_screen_clock_list_clock_main_main_default, 3);
    lv_style_set_shadow_width(&style_screen_clock_list_clock_main_main_default, 0);
    lv_obj_add_style(ui->screen_clock_list_clock, &style_screen_clock_list_clock_main_main_default, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style state: LV_STATE_DEFAULT for &style_screen_clock_list_clock_main_scrollbar_default
    static lv_style_t style_screen_clock_list_clock_main_scrollbar_default;
    ui_init_style(&style_screen_clock_list_clock_main_scrollbar_default);

    lv_style_set_radius(&style_screen_clock_list_clock_main_scrollbar_default, 3);
    lv_style_set_bg_opa(&style_screen_clock_list_clock_main_scrollbar_default, 255);
    lv_style_set_bg_color(&style_screen_clock_list_clock_main_scrollbar_default, lv_color_hex(0xffffff));
    lv_style_set_bg_grad_dir(&style_screen_clock_list_clock_main_scrollbar_default, LV_GRAD_DIR_NONE);
    lv_obj_add_style(ui->screen_clock_list_clock, &style_screen_clock_list_clock_main_scrollbar_default, LV_PART_SCROLLBAR|LV_STATE_DEFAULT);

    //Write style state: LV_STATE_DEFAULT for &style_screen_clock_list_clock_extra_btns_main_default
    static lv_style_t style_screen_clock_list_clock_extra_btns_main_default;
    ui_init_style(&style_screen_clock_list_clock_extra_btns_main_default);

    lv_style_set_pad_top(&style_screen_clock_list_clock_extra_btns_main_default, 5);
    lv_style_set_pad_left(&style_screen_clock_list_clock_extra_btns_main_default, 5);
    lv_style_set_pad_right(&style_screen_clock_list_clock_extra_btns_main_default, 5);
    lv_style_set_pad_bottom(&style_screen_clock_list_clock_extra_btns_main_default, 5);
    lv_style_set_border_width(&style_screen_clock_list_clock_extra_btns_main_default, 0);
    lv_style_set_text_color(&style_screen_clock_list_clock_extra_btns_main_default, lv_color_hex(0x0D3055));
    lv_style_set_text_font(&style_screen_clock_list_clock_extra_btns_main_default, &lv_font_ZiTiQuanWeiJunHeiW22_22);
    lv_style_set_text_opa(&style_screen_clock_list_clock_extra_btns_main_default, 255);
    lv_style_set_radius(&style_screen_clock_list_clock_extra_btns_main_default, 3);
    lv_style_set_bg_opa(&style_screen_clock_list_clock_extra_btns_main_default, 255);
    lv_style_set_bg_color(&style_screen_clock_list_clock_extra_btns_main_default, lv_color_hex(0xffffff));
    lv_style_set_bg_grad_dir(&style_screen_clock_list_clock_extra_btns_main_default, LV_GRAD_DIR_NONE);
    lv_obj_add_style(ui->screen_clock_list_clock_item3, &style_screen_clock_list_clock_extra_btns_main_default, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_add_style(ui->screen_clock_list_clock_item2, &style_screen_clock_list_clock_extra_btns_main_default, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_add_style(ui->screen_clock_list_clock_item1, &style_screen_clock_list_clock_extra_btns_main_default, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_add_style(ui->screen_clock_list_clock_item0, &style_screen_clock_list_clock_extra_btns_main_default, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style state: LV_STATE_DEFAULT for &style_screen_clock_list_clock_extra_texts_main_default
    static lv_style_t style_screen_clock_list_clock_extra_texts_main_default;
    ui_init_style(&style_screen_clock_list_clock_extra_texts_main_default);

    lv_style_set_pad_top(&style_screen_clock_list_clock_extra_texts_main_default, 5);
    lv_style_set_pad_left(&style_screen_clock_list_clock_extra_texts_main_default, 5);
    lv_style_set_pad_right(&style_screen_clock_list_clock_extra_texts_main_default, 5);
    lv_style_set_pad_bottom(&style_screen_clock_list_clock_extra_texts_main_default, 5);
    lv_style_set_border_width(&style_screen_clock_list_clock_extra_texts_main_default, 0);
    lv_style_set_text_color(&style_screen_clock_list_clock_extra_texts_main_default, lv_color_hex(0x0D3055));
    lv_style_set_text_font(&style_screen_clock_list_clock_extra_texts_main_default, &lv_font_ZiTiQuanWeiJunHeiW22_12);
    lv_style_set_text_opa(&style_screen_clock_list_clock_extra_texts_main_default, 255);
    lv_style_set_radius(&style_screen_clock_list_clock_extra_texts_main_default, 3);
    lv_style_set_transform_width(&style_screen_clock_list_clock_extra_texts_main_default, 0);
    lv_style_set_bg_opa(&style_screen_clock_list_clock_extra_texts_main_default, 255);
    lv_style_set_bg_color(&style_screen_clock_list_clock_extra_texts_main_default, lv_color_hex(0xffffff));
    lv_style_set_bg_grad_dir(&style_screen_clock_list_clock_extra_texts_main_default, LV_GRAD_DIR_NONE);

    //Write codes screen_clock_sw_4
    ui->screen_clock_sw_4 = lv_switch_create(ui->screen_clock);
    lv_obj_set_pos(ui->screen_clock_sw_4, 148, 175);
    lv_obj_set_size(ui->screen_clock_sw_4, 45, 25);

    //Write style for screen_clock_sw_4, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_clock_sw_4, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_clock_sw_4, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_clock_sw_4, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->screen_clock_sw_4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_clock_sw_4, 10, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_clock_sw_4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style for screen_clock_sw_4, Part: LV_PART_INDICATOR, State: LV_STATE_CHECKED.
    lv_obj_set_style_bg_opa(ui->screen_clock_sw_4, 255, LV_PART_INDICATOR|LV_STATE_CHECKED);
    lv_obj_set_style_bg_color(ui->screen_clock_sw_4, lv_color_hex(0x2195f6), LV_PART_INDICATOR|LV_STATE_CHECKED);
    lv_obj_set_style_bg_grad_dir(ui->screen_clock_sw_4, LV_GRAD_DIR_NONE, LV_PART_INDICATOR|LV_STATE_CHECKED);
    lv_obj_set_style_border_width(ui->screen_clock_sw_4, 0, LV_PART_INDICATOR|LV_STATE_CHECKED);

    //Write style for screen_clock_sw_4, Part: LV_PART_KNOB, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_clock_sw_4, 255, LV_PART_KNOB|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_clock_sw_4, lv_color_hex(0xffab00), LV_PART_KNOB|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_clock_sw_4, LV_GRAD_DIR_NONE, LV_PART_KNOB|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->screen_clock_sw_4, 0, LV_PART_KNOB|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_clock_sw_4, 10, LV_PART_KNOB|LV_STATE_DEFAULT);

    //Write codes screen_clock_label_1
    ui->screen_clock_label_1 = lv_label_create(ui->screen_clock);
    lv_obj_set_pos(ui->screen_clock_label_1, 58, 19);
    lv_obj_set_size(ui->screen_clock_label_1, 110, 28);
    lv_label_set_text(ui->screen_clock_label_1, "设置闹钟");
    lv_label_set_long_mode(ui->screen_clock_label_1, LV_LABEL_LONG_WRAP);

    //Write style for screen_clock_label_1, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_clock_label_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_clock_label_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_clock_label_1, lv_color_hex(0xf9f9f9), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_clock_label_1, &lv_font_ZiTiQuanWeiJunHeiW22_24, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_clock_label_1, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->screen_clock_label_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->screen_clock_label_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_clock_label_1, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_clock_label_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_clock_label_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_clock_label_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_clock_label_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_clock_label_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_clock_label_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_clock_btn_return
    ui->screen_clock_btn_return = lv_button_create(ui->screen_clock);
    lv_obj_set_pos(ui->screen_clock_btn_return, 79, 226);
    lv_obj_set_size(ui->screen_clock_btn_return, 76, 35);
    ui->screen_clock_btn_return_label = lv_label_create(ui->screen_clock_btn_return);
    lv_label_set_text(ui->screen_clock_btn_return_label, "返回");
    lv_label_set_long_mode(ui->screen_clock_btn_return_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(ui->screen_clock_btn_return_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(ui->screen_clock_btn_return, 0, LV_STATE_DEFAULT);
    lv_obj_set_width(ui->screen_clock_btn_return_label, LV_PCT(100));

    //Write style for screen_clock_btn_return, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_clock_btn_return, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_clock_btn_return, lv_color_hex(0x2195f6), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_clock_btn_return, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->screen_clock_btn_return, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_clock_btn_return, 5, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_clock_btn_return, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_clock_btn_return, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_clock_btn_return, &lv_font_ZiTiQuanWeiJunHeiW22_18, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_clock_btn_return, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_clock_btn_return, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_clock_sw_3
    ui->screen_clock_sw_3 = lv_switch_create(ui->screen_clock);
    lv_obj_set_pos(ui->screen_clock_sw_3, 148, 132);
    lv_obj_set_size(ui->screen_clock_sw_3, 45, 25);

    //Write style for screen_clock_sw_3, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_clock_sw_3, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_clock_sw_3, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_clock_sw_3, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->screen_clock_sw_3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_clock_sw_3, 10, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_clock_sw_3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style for screen_clock_sw_3, Part: LV_PART_INDICATOR, State: LV_STATE_CHECKED.
    lv_obj_set_style_bg_opa(ui->screen_clock_sw_3, 255, LV_PART_INDICATOR|LV_STATE_CHECKED);
    lv_obj_set_style_bg_color(ui->screen_clock_sw_3, lv_color_hex(0x2195f6), LV_PART_INDICATOR|LV_STATE_CHECKED);
    lv_obj_set_style_bg_grad_dir(ui->screen_clock_sw_3, LV_GRAD_DIR_NONE, LV_PART_INDICATOR|LV_STATE_CHECKED);
    lv_obj_set_style_border_width(ui->screen_clock_sw_3, 0, LV_PART_INDICATOR|LV_STATE_CHECKED);

    //Write style for screen_clock_sw_3, Part: LV_PART_KNOB, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_clock_sw_3, 255, LV_PART_KNOB|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_clock_sw_3, lv_color_hex(0xffab00), LV_PART_KNOB|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_clock_sw_3, LV_GRAD_DIR_NONE, LV_PART_KNOB|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->screen_clock_sw_3, 0, LV_PART_KNOB|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_clock_sw_3, 10, LV_PART_KNOB|LV_STATE_DEFAULT);

    //Write codes screen_clock_sw_2
    ui->screen_clock_sw_2 = lv_switch_create(ui->screen_clock);
    lv_obj_set_pos(ui->screen_clock_sw_2, 148, 95);
    lv_obj_set_size(ui->screen_clock_sw_2, 45, 25);

    //Write style for screen_clock_sw_2, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_clock_sw_2, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_clock_sw_2, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_clock_sw_2, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->screen_clock_sw_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_clock_sw_2, 10, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_clock_sw_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style for screen_clock_sw_2, Part: LV_PART_INDICATOR, State: LV_STATE_CHECKED.
    lv_obj_set_style_bg_opa(ui->screen_clock_sw_2, 255, LV_PART_INDICATOR|LV_STATE_CHECKED);
    lv_obj_set_style_bg_color(ui->screen_clock_sw_2, lv_color_hex(0x2195f6), LV_PART_INDICATOR|LV_STATE_CHECKED);
    lv_obj_set_style_bg_grad_dir(ui->screen_clock_sw_2, LV_GRAD_DIR_NONE, LV_PART_INDICATOR|LV_STATE_CHECKED);
    lv_obj_set_style_border_width(ui->screen_clock_sw_2, 0, LV_PART_INDICATOR|LV_STATE_CHECKED);

    //Write style for screen_clock_sw_2, Part: LV_PART_KNOB, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_clock_sw_2, 255, LV_PART_KNOB|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_clock_sw_2, lv_color_hex(0xffab00), LV_PART_KNOB|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_clock_sw_2, LV_GRAD_DIR_NONE, LV_PART_KNOB|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->screen_clock_sw_2, 0, LV_PART_KNOB|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_clock_sw_2, 10, LV_PART_KNOB|LV_STATE_DEFAULT);

    //Write codes screen_clock_sw_1
    ui->screen_clock_sw_1 = lv_switch_create(ui->screen_clock);
    lv_obj_set_pos(ui->screen_clock_sw_1, 148, 59);
    lv_obj_set_size(ui->screen_clock_sw_1, 45, 25);

    //Write style for screen_clock_sw_1, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_clock_sw_1, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_clock_sw_1, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_clock_sw_1, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->screen_clock_sw_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_clock_sw_1, 10, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_clock_sw_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style for screen_clock_sw_1, Part: LV_PART_INDICATOR, State: LV_STATE_CHECKED.
    lv_obj_set_style_bg_opa(ui->screen_clock_sw_1, 255, LV_PART_INDICATOR|LV_STATE_CHECKED);
    lv_obj_set_style_bg_color(ui->screen_clock_sw_1, lv_color_hex(0x2195f6), LV_PART_INDICATOR|LV_STATE_CHECKED);
    lv_obj_set_style_bg_grad_dir(ui->screen_clock_sw_1, LV_GRAD_DIR_NONE, LV_PART_INDICATOR|LV_STATE_CHECKED);
    lv_obj_set_style_border_width(ui->screen_clock_sw_1, 0, LV_PART_INDICATOR|LV_STATE_CHECKED);

    //Write style for screen_clock_sw_1, Part: LV_PART_KNOB, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_clock_sw_1, 255, LV_PART_KNOB|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_clock_sw_1, lv_color_hex(0xffab00), LV_PART_KNOB|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_clock_sw_1, LV_GRAD_DIR_NONE, LV_PART_KNOB|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->screen_clock_sw_1, 0, LV_PART_KNOB|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_clock_sw_1, 10, LV_PART_KNOB|LV_STATE_DEFAULT);

    //The custom code of screen_clock.


    //Update current screen layout.
    lv_obj_update_layout(ui->screen_clock);

    //Init events for screen.
    events_init_screen_clock(ui);
}
