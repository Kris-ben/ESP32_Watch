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



void setup_scr_screen_down(lv_ui *ui)
{
    //Write codes screen_down
    ui->screen_down = lv_obj_create(NULL);
    lv_obj_set_size(ui->screen_down, 240, 284);
    lv_obj_set_scrollbar_mode(ui->screen_down, LV_SCROLLBAR_MODE_OFF);

    //Write style for screen_down, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_down, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_down, lv_color_hex(0x010101), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_down, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_down_list_voice
    ui->screen_down_list_voice = lv_list_create(ui->screen_down);
    lv_obj_set_pos(ui->screen_down_list_voice, 11, 24);
    lv_obj_set_size(ui->screen_down_list_voice, 35, 28);
    lv_obj_set_scrollbar_mode(ui->screen_down_list_voice, LV_SCROLLBAR_MODE_OFF);
    ui->screen_down_list_voice_item0 = lv_list_add_button(ui->screen_down_list_voice, LV_SYMBOL_VOLUME_MAX, "");

    //Write style state: LV_STATE_DEFAULT for &style_screen_down_list_voice_main_main_default
    static lv_style_t style_screen_down_list_voice_main_main_default;
    ui_init_style(&style_screen_down_list_voice_main_main_default);

    lv_style_set_pad_top(&style_screen_down_list_voice_main_main_default, 5);
    lv_style_set_pad_left(&style_screen_down_list_voice_main_main_default, 5);
    lv_style_set_pad_right(&style_screen_down_list_voice_main_main_default, 5);
    lv_style_set_pad_bottom(&style_screen_down_list_voice_main_main_default, 5);
    lv_style_set_bg_opa(&style_screen_down_list_voice_main_main_default, 255);
    lv_style_set_bg_color(&style_screen_down_list_voice_main_main_default, lv_color_hex(0x000000));
    lv_style_set_bg_grad_dir(&style_screen_down_list_voice_main_main_default, LV_GRAD_DIR_NONE);
    lv_style_set_border_width(&style_screen_down_list_voice_main_main_default, 1);
    lv_style_set_border_opa(&style_screen_down_list_voice_main_main_default, 255);
    lv_style_set_border_color(&style_screen_down_list_voice_main_main_default, lv_color_hex(0x050505));
    lv_style_set_border_side(&style_screen_down_list_voice_main_main_default, LV_BORDER_SIDE_FULL);
    lv_style_set_radius(&style_screen_down_list_voice_main_main_default, 3);
    lv_style_set_shadow_width(&style_screen_down_list_voice_main_main_default, 0);
    lv_obj_add_style(ui->screen_down_list_voice, &style_screen_down_list_voice_main_main_default, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style state: LV_STATE_DEFAULT for &style_screen_down_list_voice_main_scrollbar_default
    static lv_style_t style_screen_down_list_voice_main_scrollbar_default;
    ui_init_style(&style_screen_down_list_voice_main_scrollbar_default);

    lv_style_set_radius(&style_screen_down_list_voice_main_scrollbar_default, 3);
    lv_style_set_bg_opa(&style_screen_down_list_voice_main_scrollbar_default, 255);
    lv_style_set_bg_color(&style_screen_down_list_voice_main_scrollbar_default, lv_color_hex(0x000000));
    lv_style_set_bg_grad_dir(&style_screen_down_list_voice_main_scrollbar_default, LV_GRAD_DIR_NONE);
    lv_obj_add_style(ui->screen_down_list_voice, &style_screen_down_list_voice_main_scrollbar_default, LV_PART_SCROLLBAR|LV_STATE_DEFAULT);

    //Write style state: LV_STATE_DEFAULT for &style_screen_down_list_voice_extra_btns_main_default
    static lv_style_t style_screen_down_list_voice_extra_btns_main_default;
    ui_init_style(&style_screen_down_list_voice_extra_btns_main_default);

    lv_style_set_pad_top(&style_screen_down_list_voice_extra_btns_main_default, 5);
    lv_style_set_pad_left(&style_screen_down_list_voice_extra_btns_main_default, 5);
    lv_style_set_pad_right(&style_screen_down_list_voice_extra_btns_main_default, 5);
    lv_style_set_pad_bottom(&style_screen_down_list_voice_extra_btns_main_default, 5);
    lv_style_set_border_width(&style_screen_down_list_voice_extra_btns_main_default, 0);
    lv_style_set_text_color(&style_screen_down_list_voice_extra_btns_main_default, lv_color_hex(0xe8e8e8));
    lv_style_set_text_font(&style_screen_down_list_voice_extra_btns_main_default, &lv_font_ZiTiQuanWeiJunHeiW22_16);
    lv_style_set_text_opa(&style_screen_down_list_voice_extra_btns_main_default, 255);
    lv_style_set_radius(&style_screen_down_list_voice_extra_btns_main_default, 3);
    lv_style_set_bg_opa(&style_screen_down_list_voice_extra_btns_main_default, 255);
    lv_style_set_bg_color(&style_screen_down_list_voice_extra_btns_main_default, lv_color_hex(0x070707));
    lv_style_set_bg_grad_dir(&style_screen_down_list_voice_extra_btns_main_default, LV_GRAD_DIR_NONE);
    lv_obj_add_style(ui->screen_down_list_voice_item0, &style_screen_down_list_voice_extra_btns_main_default, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style state: LV_STATE_DEFAULT for &style_screen_down_list_voice_extra_texts_main_default
    static lv_style_t style_screen_down_list_voice_extra_texts_main_default;
    ui_init_style(&style_screen_down_list_voice_extra_texts_main_default);

    lv_style_set_pad_top(&style_screen_down_list_voice_extra_texts_main_default, 5);
    lv_style_set_pad_left(&style_screen_down_list_voice_extra_texts_main_default, 5);
    lv_style_set_pad_right(&style_screen_down_list_voice_extra_texts_main_default, 5);
    lv_style_set_pad_bottom(&style_screen_down_list_voice_extra_texts_main_default, 5);
    lv_style_set_border_width(&style_screen_down_list_voice_extra_texts_main_default, 0);
    lv_style_set_text_color(&style_screen_down_list_voice_extra_texts_main_default, lv_color_hex(0x0b0b0b));
    lv_style_set_text_font(&style_screen_down_list_voice_extra_texts_main_default, &lv_font_ZiTiQuanWeiJunHeiW22_12);
    lv_style_set_text_opa(&style_screen_down_list_voice_extra_texts_main_default, 255);
    lv_style_set_radius(&style_screen_down_list_voice_extra_texts_main_default, 3);
    lv_style_set_transform_width(&style_screen_down_list_voice_extra_texts_main_default, 0);
    lv_style_set_bg_opa(&style_screen_down_list_voice_extra_texts_main_default, 255);
    lv_style_set_bg_color(&style_screen_down_list_voice_extra_texts_main_default, lv_color_hex(0x000000));
    lv_style_set_bg_grad_dir(&style_screen_down_list_voice_extra_texts_main_default, LV_GRAD_DIR_NONE);

    //Write codes screen_down_list_brightness
    ui->screen_down_list_brightness = lv_list_create(ui->screen_down);
    lv_obj_set_pos(ui->screen_down_list_brightness, 11, 67);
    lv_obj_set_size(ui->screen_down_list_brightness, 35, 28);
    lv_obj_set_scrollbar_mode(ui->screen_down_list_brightness, LV_SCROLLBAR_MODE_OFF);
    ui->screen_down_list_brightness_item0 = lv_list_add_button(ui->screen_down_list_brightness, LV_SYMBOL_GPS, "");

    //Write style state: LV_STATE_DEFAULT for &style_screen_down_list_brightness_main_main_default
    static lv_style_t style_screen_down_list_brightness_main_main_default;
    ui_init_style(&style_screen_down_list_brightness_main_main_default);

    lv_style_set_pad_top(&style_screen_down_list_brightness_main_main_default, 5);
    lv_style_set_pad_left(&style_screen_down_list_brightness_main_main_default, 5);
    lv_style_set_pad_right(&style_screen_down_list_brightness_main_main_default, 5);
    lv_style_set_pad_bottom(&style_screen_down_list_brightness_main_main_default, 5);
    lv_style_set_bg_opa(&style_screen_down_list_brightness_main_main_default, 255);
    lv_style_set_bg_color(&style_screen_down_list_brightness_main_main_default, lv_color_hex(0x000000));
    lv_style_set_bg_grad_dir(&style_screen_down_list_brightness_main_main_default, LV_GRAD_DIR_NONE);
    lv_style_set_border_width(&style_screen_down_list_brightness_main_main_default, 1);
    lv_style_set_border_opa(&style_screen_down_list_brightness_main_main_default, 255);
    lv_style_set_border_color(&style_screen_down_list_brightness_main_main_default, lv_color_hex(0x050505));
    lv_style_set_border_side(&style_screen_down_list_brightness_main_main_default, LV_BORDER_SIDE_FULL);
    lv_style_set_radius(&style_screen_down_list_brightness_main_main_default, 3);
    lv_style_set_shadow_width(&style_screen_down_list_brightness_main_main_default, 0);
    lv_obj_add_style(ui->screen_down_list_brightness, &style_screen_down_list_brightness_main_main_default, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style state: LV_STATE_DEFAULT for &style_screen_down_list_brightness_main_scrollbar_default
    static lv_style_t style_screen_down_list_brightness_main_scrollbar_default;
    ui_init_style(&style_screen_down_list_brightness_main_scrollbar_default);

    lv_style_set_radius(&style_screen_down_list_brightness_main_scrollbar_default, 3);
    lv_style_set_bg_opa(&style_screen_down_list_brightness_main_scrollbar_default, 255);
    lv_style_set_bg_color(&style_screen_down_list_brightness_main_scrollbar_default, lv_color_hex(0x000000));
    lv_style_set_bg_grad_dir(&style_screen_down_list_brightness_main_scrollbar_default, LV_GRAD_DIR_NONE);
    lv_obj_add_style(ui->screen_down_list_brightness, &style_screen_down_list_brightness_main_scrollbar_default, LV_PART_SCROLLBAR|LV_STATE_DEFAULT);

    //Write style state: LV_STATE_DEFAULT for &style_screen_down_list_brightness_extra_btns_main_default
    static lv_style_t style_screen_down_list_brightness_extra_btns_main_default;
    ui_init_style(&style_screen_down_list_brightness_extra_btns_main_default);

    lv_style_set_pad_top(&style_screen_down_list_brightness_extra_btns_main_default, 5);
    lv_style_set_pad_left(&style_screen_down_list_brightness_extra_btns_main_default, 5);
    lv_style_set_pad_right(&style_screen_down_list_brightness_extra_btns_main_default, 5);
    lv_style_set_pad_bottom(&style_screen_down_list_brightness_extra_btns_main_default, 5);
    lv_style_set_border_width(&style_screen_down_list_brightness_extra_btns_main_default, 0);
    lv_style_set_text_color(&style_screen_down_list_brightness_extra_btns_main_default, lv_color_hex(0xe8e8e8));
    lv_style_set_text_font(&style_screen_down_list_brightness_extra_btns_main_default, &lv_font_ZiTiQuanWeiJunHeiW22_16);
    lv_style_set_text_opa(&style_screen_down_list_brightness_extra_btns_main_default, 255);
    lv_style_set_radius(&style_screen_down_list_brightness_extra_btns_main_default, 3);
    lv_style_set_bg_opa(&style_screen_down_list_brightness_extra_btns_main_default, 255);
    lv_style_set_bg_color(&style_screen_down_list_brightness_extra_btns_main_default, lv_color_hex(0x070707));
    lv_style_set_bg_grad_dir(&style_screen_down_list_brightness_extra_btns_main_default, LV_GRAD_DIR_NONE);
    lv_obj_add_style(ui->screen_down_list_brightness_item0, &style_screen_down_list_brightness_extra_btns_main_default, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style state: LV_STATE_DEFAULT for &style_screen_down_list_brightness_extra_texts_main_default
    static lv_style_t style_screen_down_list_brightness_extra_texts_main_default;
    ui_init_style(&style_screen_down_list_brightness_extra_texts_main_default);

    lv_style_set_pad_top(&style_screen_down_list_brightness_extra_texts_main_default, 5);
    lv_style_set_pad_left(&style_screen_down_list_brightness_extra_texts_main_default, 5);
    lv_style_set_pad_right(&style_screen_down_list_brightness_extra_texts_main_default, 5);
    lv_style_set_pad_bottom(&style_screen_down_list_brightness_extra_texts_main_default, 5);
    lv_style_set_border_width(&style_screen_down_list_brightness_extra_texts_main_default, 0);
    lv_style_set_text_color(&style_screen_down_list_brightness_extra_texts_main_default, lv_color_hex(0x0b0b0b));
    lv_style_set_text_font(&style_screen_down_list_brightness_extra_texts_main_default, &lv_font_ZiTiQuanWeiJunHeiW22_12);
    lv_style_set_text_opa(&style_screen_down_list_brightness_extra_texts_main_default, 255);
    lv_style_set_radius(&style_screen_down_list_brightness_extra_texts_main_default, 3);
    lv_style_set_transform_width(&style_screen_down_list_brightness_extra_texts_main_default, 0);
    lv_style_set_bg_opa(&style_screen_down_list_brightness_extra_texts_main_default, 255);
    lv_style_set_bg_color(&style_screen_down_list_brightness_extra_texts_main_default, lv_color_hex(0x000000));
    lv_style_set_bg_grad_dir(&style_screen_down_list_brightness_extra_texts_main_default, LV_GRAD_DIR_NONE);

    //Write codes screen_down_img_wifi
    ui->screen_down_img_wifi = lv_image_create(ui->screen_down);
    lv_obj_set_pos(ui->screen_down_img_wifi, 126, 128);
    lv_obj_set_size(ui->screen_down_img_wifi, 100, 100);
    lv_obj_add_flag(ui->screen_down_img_wifi, LV_OBJ_FLAG_CLICKABLE);
    lv_image_set_src(ui->screen_down_img_wifi, &_WiFi2_RGB565A8_100x100);
    lv_image_set_pivot(ui->screen_down_img_wifi, 50,50);
    lv_image_set_rotation(ui->screen_down_img_wifi, 0);

    //Write style for screen_down_img_wifi, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_image_recolor_opa(ui->screen_down_img_wifi, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_image_opa(ui->screen_down_img_wifi, 255, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_down_slider_brightness
    ui->screen_down_slider_brightness = lv_slider_create(ui->screen_down);
    lv_obj_set_pos(ui->screen_down_slider_brightness, 76, 75);
    lv_obj_set_size(ui->screen_down_slider_brightness, 146, 17);
    lv_slider_set_range(ui->screen_down_slider_brightness, 0, 100);
    lv_slider_set_mode(ui->screen_down_slider_brightness, LV_SLIDER_MODE_NORMAL);
    lv_slider_set_value(ui->screen_down_slider_brightness, 50, LV_ANIM_OFF);

    //Write style for screen_down_slider_brightness, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_down_slider_brightness, 60, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_down_slider_brightness, lv_color_hex(0xe3dada), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_down_slider_brightness, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_down_slider_brightness, 8, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_outline_width(ui->screen_down_slider_brightness, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_down_slider_brightness, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style for screen_down_slider_brightness, Part: LV_PART_INDICATOR, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_down_slider_brightness, 255, LV_PART_INDICATOR|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_down_slider_brightness, lv_color_hex(0xf2f2f2), LV_PART_INDICATOR|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_down_slider_brightness, LV_GRAD_DIR_NONE, LV_PART_INDICATOR|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_down_slider_brightness, 8, LV_PART_INDICATOR|LV_STATE_DEFAULT);

    //Write style for screen_down_slider_brightness, Part: LV_PART_KNOB, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_down_slider_brightness, 255, LV_PART_KNOB|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_down_slider_brightness, lv_color_hex(0xefeeee), LV_PART_KNOB|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_down_slider_brightness, LV_GRAD_DIR_NONE, LV_PART_KNOB|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_down_slider_brightness, 8, LV_PART_KNOB|LV_STATE_DEFAULT);

    //Write codes screen_down_slider_1
    ui->screen_down_slider_1 = lv_slider_create(ui->screen_down);
    lv_obj_set_pos(ui->screen_down_slider_1, 76, 36);
    lv_obj_set_size(ui->screen_down_slider_1, 146, 17);
    lv_slider_set_range(ui->screen_down_slider_1, 0, 100);
    lv_slider_set_mode(ui->screen_down_slider_1, LV_SLIDER_MODE_NORMAL);
    lv_slider_set_value(ui->screen_down_slider_1, 50, LV_ANIM_OFF);

    //Write style for screen_down_slider_1, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_down_slider_1, 60, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_down_slider_1, lv_color_hex(0xe3dada), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_down_slider_1, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_down_slider_1, 8, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_outline_width(ui->screen_down_slider_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_down_slider_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style for screen_down_slider_1, Part: LV_PART_INDICATOR, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_down_slider_1, 255, LV_PART_INDICATOR|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_down_slider_1, lv_color_hex(0xf2f2f2), LV_PART_INDICATOR|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_down_slider_1, LV_GRAD_DIR_NONE, LV_PART_INDICATOR|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_down_slider_1, 8, LV_PART_INDICATOR|LV_STATE_DEFAULT);

    //Write style for screen_down_slider_1, Part: LV_PART_KNOB, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_down_slider_1, 255, LV_PART_KNOB|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_down_slider_1, lv_color_hex(0xefeeee), LV_PART_KNOB|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_down_slider_1, LV_GRAD_DIR_NONE, LV_PART_KNOB|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_down_slider_1, 8, LV_PART_KNOB|LV_STATE_DEFAULT);

    //Write codes screen_down_img_musci
    ui->screen_down_img_musci = lv_image_create(ui->screen_down);
    lv_obj_set_pos(ui->screen_down_img_musci, 11, 128);
    lv_obj_set_size(ui->screen_down_img_musci, 100, 100);
    lv_obj_add_flag(ui->screen_down_img_musci, LV_OBJ_FLAG_CLICKABLE);
    lv_image_set_src(ui->screen_down_img_musci, &_musci2_RGB565A8_100x100);
    lv_image_set_pivot(ui->screen_down_img_musci, 50,50);
    lv_image_set_rotation(ui->screen_down_img_musci, 0);

    //Write style for screen_down_img_musci, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_image_recolor_opa(ui->screen_down_img_musci, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_image_opa(ui->screen_down_img_musci, 255, LV_PART_MAIN|LV_STATE_DEFAULT);

    //The custom code of screen_down.


    //Update current screen layout.
    lv_obj_update_layout(ui->screen_down);

    //Init events for screen.
    events_init_screen_down(ui);
}
