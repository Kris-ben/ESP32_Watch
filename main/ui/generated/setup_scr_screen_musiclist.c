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
#include "sd_card_fs.h"
#include "music_page.h"



int screen_musiclist_digital_clock_1_min_value = 25;
int screen_musiclist_digital_clock_1_hour_value = 11;
int screen_musiclist_digital_clock_1_sec_value = 50;
void setup_scr_screen_musiclist(lv_ui *ui)
{
    music_page_create_list(ui);
    return;
    //Write codes screen_musiclist
    ui->screen_musiclist = lv_obj_create(NULL);
    lv_obj_set_size(ui->screen_musiclist, 240, 284);
    lv_obj_set_scrollbar_mode(ui->screen_musiclist, LV_SCROLLBAR_MODE_OFF);

    //Write style for screen_musiclist, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_musiclist, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_musiclist, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_musiclist, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_musiclist_list_music
    ui->screen_musiclist_list_music = lv_list_create(ui->screen_musiclist);
    lv_obj_set_pos(ui->screen_musiclist_list_music, -1, 34);
    lv_obj_set_size(ui->screen_musiclist_list_music, 242, 154);
    lv_obj_set_scrollbar_mode(ui->screen_musiclist_list_music, LV_SCROLLBAR_MODE_OFF);
    ui->screen_musiclist_list_music_item0 = lv_list_add_button(ui->screen_musiclist_list_music, LV_SYMBOL_AUDIO, "save");
    ui->screen_musiclist_list_music_item1 = lv_list_add_button(ui->screen_musiclist_list_music, LV_SYMBOL_AUDIO, "save_1");
    ui->screen_musiclist_list_music_item2 = lv_list_add_button(ui->screen_musiclist_list_music, LV_SYMBOL_AUDIO, "save_2");
    ui->screen_musiclist_list_music_item3 = lv_list_add_button(ui->screen_musiclist_list_music, LV_SYMBOL_AUDIO, "save_3");
    ui->screen_musiclist_list_music_item4 = lv_list_add_button(ui->screen_musiclist_list_music, LV_SYMBOL_AUDIO, "save_4");
    ui->screen_musiclist_list_music_item5 = lv_list_add_button(ui->screen_musiclist_list_music, LV_SYMBOL_AUDIO, "save_5");
    ui->screen_musiclist_list_music_item6 = lv_list_add_button(ui->screen_musiclist_list_music, LV_SYMBOL_AUDIO, "save_6");
    ui->screen_musiclist_list_music_item7 = lv_list_add_button(ui->screen_musiclist_list_music, LV_SYMBOL_AUDIO, "save_7");
    ui->screen_musiclist_list_music_item8 = lv_list_add_button(ui->screen_musiclist_list_music, LV_SYMBOL_AUDIO, "save_8");
    ui->screen_musiclist_list_music_item9 = lv_list_add_button(ui->screen_musiclist_list_music, LV_SYMBOL_AUDIO, "save_9");
    ui->screen_musiclist_list_music_item10 = lv_list_add_button(ui->screen_musiclist_list_music, LV_SYMBOL_AUDIO, "save_10");

    //Write style state: LV_STATE_DEFAULT for &style_screen_musiclist_list_music_main_main_default
    static lv_style_t style_screen_musiclist_list_music_main_main_default;
    ui_init_style(&style_screen_musiclist_list_music_main_main_default);

    lv_style_set_pad_top(&style_screen_musiclist_list_music_main_main_default, 5);
    lv_style_set_pad_left(&style_screen_musiclist_list_music_main_main_default, 5);
    lv_style_set_pad_right(&style_screen_musiclist_list_music_main_main_default, 5);
    lv_style_set_pad_bottom(&style_screen_musiclist_list_music_main_main_default, 5);
    lv_style_set_bg_opa(&style_screen_musiclist_list_music_main_main_default, 255);
    lv_style_set_bg_color(&style_screen_musiclist_list_music_main_main_default, lv_color_hex(0xffffff));
    lv_style_set_bg_grad_dir(&style_screen_musiclist_list_music_main_main_default, LV_GRAD_DIR_NONE);
    lv_style_set_border_width(&style_screen_musiclist_list_music_main_main_default, 1);
    lv_style_set_border_opa(&style_screen_musiclist_list_music_main_main_default, 255);
    lv_style_set_border_color(&style_screen_musiclist_list_music_main_main_default, lv_color_hex(0xe1e6ee));
    lv_style_set_border_side(&style_screen_musiclist_list_music_main_main_default, LV_BORDER_SIDE_FULL);
    lv_style_set_radius(&style_screen_musiclist_list_music_main_main_default, 3);
    lv_style_set_shadow_width(&style_screen_musiclist_list_music_main_main_default, 0);
    lv_obj_add_style(ui->screen_musiclist_list_music, &style_screen_musiclist_list_music_main_main_default, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style state: LV_STATE_DEFAULT for &style_screen_musiclist_list_music_main_scrollbar_default
    static lv_style_t style_screen_musiclist_list_music_main_scrollbar_default;
    ui_init_style(&style_screen_musiclist_list_music_main_scrollbar_default);

    lv_style_set_radius(&style_screen_musiclist_list_music_main_scrollbar_default, 3);
    lv_style_set_bg_opa(&style_screen_musiclist_list_music_main_scrollbar_default, 255);
    lv_style_set_bg_color(&style_screen_musiclist_list_music_main_scrollbar_default, lv_color_hex(0xffffff));
    lv_style_set_bg_grad_dir(&style_screen_musiclist_list_music_main_scrollbar_default, LV_GRAD_DIR_NONE);
    lv_obj_add_style(ui->screen_musiclist_list_music, &style_screen_musiclist_list_music_main_scrollbar_default, LV_PART_SCROLLBAR|LV_STATE_DEFAULT);

    //Write style state: LV_STATE_DEFAULT for &style_screen_musiclist_list_music_extra_btns_main_default
    static lv_style_t style_screen_musiclist_list_music_extra_btns_main_default;
    ui_init_style(&style_screen_musiclist_list_music_extra_btns_main_default);

    lv_style_set_pad_top(&style_screen_musiclist_list_music_extra_btns_main_default, 5);
    lv_style_set_pad_left(&style_screen_musiclist_list_music_extra_btns_main_default, 5);
    lv_style_set_pad_right(&style_screen_musiclist_list_music_extra_btns_main_default, 5);
    lv_style_set_pad_bottom(&style_screen_musiclist_list_music_extra_btns_main_default, 5);
    lv_style_set_border_width(&style_screen_musiclist_list_music_extra_btns_main_default, 0);
    lv_style_set_text_color(&style_screen_musiclist_list_music_extra_btns_main_default, lv_color_hex(0x0D3055));
    lv_style_set_text_font(&style_screen_musiclist_list_music_extra_btns_main_default, &lv_font_ZiTiQuanWeiJunHeiW22_24);
    lv_style_set_text_opa(&style_screen_musiclist_list_music_extra_btns_main_default, 255);
    lv_style_set_radius(&style_screen_musiclist_list_music_extra_btns_main_default, 3);
    lv_style_set_bg_opa(&style_screen_musiclist_list_music_extra_btns_main_default, 255);
    lv_style_set_bg_color(&style_screen_musiclist_list_music_extra_btns_main_default, lv_color_hex(0xffffff));
    lv_style_set_bg_grad_dir(&style_screen_musiclist_list_music_extra_btns_main_default, LV_GRAD_DIR_NONE);
    lv_obj_add_style(ui->screen_musiclist_list_music_item10, &style_screen_musiclist_list_music_extra_btns_main_default, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_add_style(ui->screen_musiclist_list_music_item9, &style_screen_musiclist_list_music_extra_btns_main_default, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_add_style(ui->screen_musiclist_list_music_item8, &style_screen_musiclist_list_music_extra_btns_main_default, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_add_style(ui->screen_musiclist_list_music_item7, &style_screen_musiclist_list_music_extra_btns_main_default, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_add_style(ui->screen_musiclist_list_music_item6, &style_screen_musiclist_list_music_extra_btns_main_default, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_add_style(ui->screen_musiclist_list_music_item5, &style_screen_musiclist_list_music_extra_btns_main_default, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_add_style(ui->screen_musiclist_list_music_item4, &style_screen_musiclist_list_music_extra_btns_main_default, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_add_style(ui->screen_musiclist_list_music_item3, &style_screen_musiclist_list_music_extra_btns_main_default, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_add_style(ui->screen_musiclist_list_music_item2, &style_screen_musiclist_list_music_extra_btns_main_default, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_add_style(ui->screen_musiclist_list_music_item1, &style_screen_musiclist_list_music_extra_btns_main_default, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_add_style(ui->screen_musiclist_list_music_item0, &style_screen_musiclist_list_music_extra_btns_main_default, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style state: LV_STATE_DEFAULT for &style_screen_musiclist_list_music_extra_texts_main_default
    static lv_style_t style_screen_musiclist_list_music_extra_texts_main_default;
    ui_init_style(&style_screen_musiclist_list_music_extra_texts_main_default);

    lv_style_set_pad_top(&style_screen_musiclist_list_music_extra_texts_main_default, 5);
    lv_style_set_pad_left(&style_screen_musiclist_list_music_extra_texts_main_default, 5);
    lv_style_set_pad_right(&style_screen_musiclist_list_music_extra_texts_main_default, 5);
    lv_style_set_pad_bottom(&style_screen_musiclist_list_music_extra_texts_main_default, 5);
    lv_style_set_border_width(&style_screen_musiclist_list_music_extra_texts_main_default, 0);
    lv_style_set_text_color(&style_screen_musiclist_list_music_extra_texts_main_default, lv_color_hex(0x0D3055));
    lv_style_set_text_font(&style_screen_musiclist_list_music_extra_texts_main_default, &lv_font_ZiTiQuanWeiJunHeiW22_12);
    lv_style_set_text_opa(&style_screen_musiclist_list_music_extra_texts_main_default, 255);
    lv_style_set_radius(&style_screen_musiclist_list_music_extra_texts_main_default, 3);
    lv_style_set_transform_width(&style_screen_musiclist_list_music_extra_texts_main_default, 0);
    lv_style_set_bg_opa(&style_screen_musiclist_list_music_extra_texts_main_default, 255);
    lv_style_set_bg_color(&style_screen_musiclist_list_music_extra_texts_main_default, lv_color_hex(0xffffff));
    lv_style_set_bg_grad_dir(&style_screen_musiclist_list_music_extra_texts_main_default, LV_GRAD_DIR_NONE);

    //Write codes screen_musiclist_img_wifi
    ui->screen_musiclist_img_wifi = lv_image_create(ui->screen_musiclist);
    lv_obj_set_pos(ui->screen_musiclist_img_wifi, 138, 9);
    lv_obj_set_size(ui->screen_musiclist_img_wifi, 20, 20);
    lv_obj_add_flag(ui->screen_musiclist_img_wifi, LV_OBJ_FLAG_CLICKABLE);
    lv_image_set_src(ui->screen_musiclist_img_wifi, &_wifi_close_RGB565A8_20x20);
    lv_image_set_pivot(ui->screen_musiclist_img_wifi, 50,50);
    lv_image_set_rotation(ui->screen_musiclist_img_wifi, 0);

    //Write style for screen_musiclist_img_wifi, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_image_recolor_opa(ui->screen_musiclist_img_wifi, 26, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_image_recolor(ui->screen_musiclist_img_wifi, lv_color_hex(0x090000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_image_opa(ui->screen_musiclist_img_wifi, 255, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_musiclist_digital_clock_1
    static bool screen_musiclist_digital_clock_1_timer_enabled = false;
    ui->screen_musiclist_digital_clock_1 = lv_label_create(ui->screen_musiclist);
    lv_obj_set_pos(ui->screen_musiclist_digital_clock_1, 7, 1);
    lv_obj_set_size(ui->screen_musiclist_digital_clock_1, 53, 22);
    lv_label_set_text(ui->screen_musiclist_digital_clock_1, "11:25");
    if (!screen_musiclist_digital_clock_1_timer_enabled) {
        lv_timer_create(screen_musiclist_digital_clock_1_timer, 1000, NULL);
        screen_musiclist_digital_clock_1_timer_enabled = true;
    }

    //Write style for screen_musiclist_digital_clock_1, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_radius(ui->screen_musiclist_digital_clock_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_musiclist_digital_clock_1, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_musiclist_digital_clock_1, &lv_font_ZiTiQuanWeiJunHeiW22_18, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_musiclist_digital_clock_1, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->screen_musiclist_digital_clock_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_musiclist_digital_clock_1, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_musiclist_digital_clock_1, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_musiclist_digital_clock_1, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_musiclist_digital_clock_1, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_musiclist_digital_clock_1, 7, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_musiclist_digital_clock_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_musiclist_digital_clock_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_musiclist_digital_clock_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_musiclist_digital_clock_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_musiclist_list_battery
    ui->screen_musiclist_list_battery = lv_list_create(ui->screen_musiclist);
    lv_obj_set_pos(ui->screen_musiclist_list_battery, 164, 1);
    lv_obj_set_size(ui->screen_musiclist_list_battery, 67, 33);
    lv_obj_set_scrollbar_mode(ui->screen_musiclist_list_battery, LV_SCROLLBAR_MODE_OFF);
    ui->screen_musiclist_list_battery_item0 = lv_list_add_button(ui->screen_musiclist_list_battery, NULL, "--");

    //Write style state: LV_STATE_DEFAULT for &style_screen_musiclist_list_battery_main_main_default
    static lv_style_t style_screen_musiclist_list_battery_main_main_default;
    ui_init_style(&style_screen_musiclist_list_battery_main_main_default);

    lv_style_set_pad_top(&style_screen_musiclist_list_battery_main_main_default, 5);
    lv_style_set_pad_left(&style_screen_musiclist_list_battery_main_main_default, 5);
    lv_style_set_pad_right(&style_screen_musiclist_list_battery_main_main_default, 5);
    lv_style_set_pad_bottom(&style_screen_musiclist_list_battery_main_main_default, 5);
    lv_style_set_bg_opa(&style_screen_musiclist_list_battery_main_main_default, 255);
    lv_style_set_bg_color(&style_screen_musiclist_list_battery_main_main_default, lv_color_hex(0x020202));
    lv_style_set_bg_grad_dir(&style_screen_musiclist_list_battery_main_main_default, LV_GRAD_DIR_NONE);
    lv_style_set_border_width(&style_screen_musiclist_list_battery_main_main_default, 1);
    lv_style_set_border_opa(&style_screen_musiclist_list_battery_main_main_default, 255);
    lv_style_set_border_color(&style_screen_musiclist_list_battery_main_main_default, lv_color_hex(0x000000));
    lv_style_set_border_side(&style_screen_musiclist_list_battery_main_main_default, LV_BORDER_SIDE_FULL);
    lv_style_set_radius(&style_screen_musiclist_list_battery_main_main_default, 3);
    lv_style_set_shadow_width(&style_screen_musiclist_list_battery_main_main_default, 0);
    lv_obj_add_style(ui->screen_musiclist_list_battery, &style_screen_musiclist_list_battery_main_main_default, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style state: LV_STATE_DEFAULT for &style_screen_musiclist_list_battery_main_scrollbar_default
    static lv_style_t style_screen_musiclist_list_battery_main_scrollbar_default;
    ui_init_style(&style_screen_musiclist_list_battery_main_scrollbar_default);

    lv_style_set_radius(&style_screen_musiclist_list_battery_main_scrollbar_default, 3);
    lv_style_set_bg_opa(&style_screen_musiclist_list_battery_main_scrollbar_default, 255);
    lv_style_set_bg_color(&style_screen_musiclist_list_battery_main_scrollbar_default, lv_color_hex(0x000000));
    lv_style_set_bg_grad_dir(&style_screen_musiclist_list_battery_main_scrollbar_default, LV_GRAD_DIR_NONE);
    lv_obj_add_style(ui->screen_musiclist_list_battery, &style_screen_musiclist_list_battery_main_scrollbar_default, LV_PART_SCROLLBAR|LV_STATE_DEFAULT);

    //Write style state: LV_STATE_DEFAULT for &style_screen_musiclist_list_battery_extra_btns_main_default
    static lv_style_t style_screen_musiclist_list_battery_extra_btns_main_default;
    ui_init_style(&style_screen_musiclist_list_battery_extra_btns_main_default);

    lv_style_set_pad_top(&style_screen_musiclist_list_battery_extra_btns_main_default, 5);
    lv_style_set_pad_left(&style_screen_musiclist_list_battery_extra_btns_main_default, 5);
    lv_style_set_pad_right(&style_screen_musiclist_list_battery_extra_btns_main_default, 5);
    lv_style_set_pad_bottom(&style_screen_musiclist_list_battery_extra_btns_main_default, 5);
    lv_style_set_border_width(&style_screen_musiclist_list_battery_extra_btns_main_default, 0);
    lv_style_set_text_color(&style_screen_musiclist_list_battery_extra_btns_main_default, lv_color_hex(0xfbfbfb));
    lv_style_set_text_font(&style_screen_musiclist_list_battery_extra_btns_main_default, &lv_font_ZiTiQuanWeiJunHeiW22_12);
    lv_style_set_text_opa(&style_screen_musiclist_list_battery_extra_btns_main_default, 255);
    lv_style_set_radius(&style_screen_musiclist_list_battery_extra_btns_main_default, 3);
    lv_style_set_bg_opa(&style_screen_musiclist_list_battery_extra_btns_main_default, 255);
    lv_style_set_bg_color(&style_screen_musiclist_list_battery_extra_btns_main_default, lv_color_hex(0x000000));
    lv_style_set_bg_grad_dir(&style_screen_musiclist_list_battery_extra_btns_main_default, LV_GRAD_DIR_NONE);
    lv_obj_add_style(ui->screen_musiclist_list_battery_item0, &style_screen_musiclist_list_battery_extra_btns_main_default, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style state: LV_STATE_DEFAULT for &style_screen_musiclist_list_battery_extra_texts_main_default
    static lv_style_t style_screen_musiclist_list_battery_extra_texts_main_default;
    ui_init_style(&style_screen_musiclist_list_battery_extra_texts_main_default);

    lv_style_set_pad_top(&style_screen_musiclist_list_battery_extra_texts_main_default, 5);
    lv_style_set_pad_left(&style_screen_musiclist_list_battery_extra_texts_main_default, 5);
    lv_style_set_pad_right(&style_screen_musiclist_list_battery_extra_texts_main_default, 5);
    lv_style_set_pad_bottom(&style_screen_musiclist_list_battery_extra_texts_main_default, 5);
    lv_style_set_border_width(&style_screen_musiclist_list_battery_extra_texts_main_default, 0);
    lv_style_set_text_color(&style_screen_musiclist_list_battery_extra_texts_main_default, lv_color_hex(0xf9f9f9));
    lv_style_set_text_font(&style_screen_musiclist_list_battery_extra_texts_main_default, &lv_font_ZiTiQuanWeiJunHeiW22_12);
    lv_style_set_text_opa(&style_screen_musiclist_list_battery_extra_texts_main_default, 255);
    lv_style_set_radius(&style_screen_musiclist_list_battery_extra_texts_main_default, 3);
    lv_style_set_transform_width(&style_screen_musiclist_list_battery_extra_texts_main_default, 0);
    lv_style_set_bg_opa(&style_screen_musiclist_list_battery_extra_texts_main_default, 255);
    lv_style_set_bg_color(&style_screen_musiclist_list_battery_extra_texts_main_default, lv_color_hex(0xffffff));
    lv_style_set_bg_grad_dir(&style_screen_musiclist_list_battery_extra_texts_main_default, LV_GRAD_DIR_NONE);

    //Write codes screen_musiclist_btn_return
    ui->screen_musiclist_btn_return = lv_button_create(ui->screen_musiclist);
    lv_obj_set_pos(ui->screen_musiclist_btn_return, 76, 211);
    lv_obj_set_size(ui->screen_musiclist_btn_return, 76, 29);
    ui->screen_musiclist_btn_return_label = lv_label_create(ui->screen_musiclist_btn_return);
    lv_label_set_text(ui->screen_musiclist_btn_return_label, "返回");
    lv_label_set_long_mode(ui->screen_musiclist_btn_return_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(ui->screen_musiclist_btn_return_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(ui->screen_musiclist_btn_return, 0, LV_STATE_DEFAULT);
    lv_obj_set_width(ui->screen_musiclist_btn_return_label, LV_PCT(100));

    //Write style for screen_musiclist_btn_return, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_musiclist_btn_return, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_musiclist_btn_return, lv_color_hex(0x2195f6), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_musiclist_btn_return, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->screen_musiclist_btn_return, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_musiclist_btn_return, 5, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_musiclist_btn_return, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_musiclist_btn_return, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_musiclist_btn_return, &lv_font_ZiTiQuanWeiJunHeiW22_18, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_musiclist_btn_return, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_musiclist_btn_return, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);

    //The custom code of screen_musiclist.
    char song_names[11][SD_MUSIC_NAME_MAX] = {0};
    int song_count = sd_card_fs_get_music_files(song_names, 11);

    lv_obj_clean(ui->screen_musiclist_list_music);

    lv_obj_t **list_items[11] = {
        &ui->screen_musiclist_list_music_item0,
        &ui->screen_musiclist_list_music_item1,
        &ui->screen_musiclist_list_music_item2,
        &ui->screen_musiclist_list_music_item3,
        &ui->screen_musiclist_list_music_item4,
        &ui->screen_musiclist_list_music_item5,
        &ui->screen_musiclist_list_music_item6,
        &ui->screen_musiclist_list_music_item7,
        &ui->screen_musiclist_list_music_item8,
        &ui->screen_musiclist_list_music_item9,
        &ui->screen_musiclist_list_music_item10,
    };

    for (int i = 0; i < 11; i++) {
        const char *name = (i < song_count) ? song_names[i] : (song_count == 0 && i == 0 ? "未找到歌曲" : "-");
        *list_items[i] = lv_list_add_button(ui->screen_musiclist_list_music, LV_SYMBOL_AUDIO, name);
        lv_obj_add_style(*list_items[i], &style_screen_musiclist_list_music_extra_btns_main_default, LV_PART_MAIN|LV_STATE_DEFAULT);
    }

    music_ui_bind_musiclist(ui);
    lv_obj_null_on_delete(&ui->screen_musiclist_digital_clock_1);

    //Update current screen layout.
    lv_obj_update_layout(ui->screen_musiclist);

    //Init events for screen.
    events_init_screen_musiclist(ui);
}
