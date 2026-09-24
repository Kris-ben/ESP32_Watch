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
#include "music_page.h"



int screen_play_digital_clock_1_min_value = 25;
int screen_play_digital_clock_1_hour_value = 11;
int screen_play_digital_clock_1_sec_value = 50;
void setup_scr_screen_play(lv_ui *ui)
{
    music_page_create_player(ui);
    return;
    //Write codes screen_play
    ui->screen_play = lv_obj_create(NULL);
    lv_obj_set_size(ui->screen_play, 240, 284);
    lv_obj_set_scrollbar_mode(ui->screen_play, LV_SCROLLBAR_MODE_OFF);

    //Write style for screen_play, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_play, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_play, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_play, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_play_img_background
    ui->screen_play_img_background = lv_image_create(ui->screen_play);
    lv_obj_set_pos(ui->screen_play_img_background, 61, 44);
    lv_obj_set_size(ui->screen_play_img_background, 100, 100);
    lv_obj_add_flag(ui->screen_play_img_background, LV_OBJ_FLAG_CLICKABLE);
    lv_image_set_src(ui->screen_play_img_background, &_music_RGB565A8_100x100);
    lv_image_set_pivot(ui->screen_play_img_background, 50,50);
    lv_image_set_rotation(ui->screen_play_img_background, 0);

    //Write style for screen_play_img_background, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_image_recolor_opa(ui->screen_play_img_background, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_image_opa(ui->screen_play_img_background, 255, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_play_slider_musictime
    ui->screen_play_slider_musictime = lv_slider_create(ui->screen_play);
    lv_obj_set_pos(ui->screen_play_slider_musictime, 38, 156);
    lv_obj_set_size(ui->screen_play_slider_musictime, 151, 8);
    lv_slider_set_range(ui->screen_play_slider_musictime, 0, 100);
    lv_slider_set_mode(ui->screen_play_slider_musictime, LV_SLIDER_MODE_NORMAL);
    lv_slider_set_value(ui->screen_play_slider_musictime, 50, LV_ANIM_OFF);

    //Write style for screen_play_slider_musictime, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_play_slider_musictime, 60, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_play_slider_musictime, lv_color_hex(0xf9f9f9), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_play_slider_musictime, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_play_slider_musictime, 8, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_outline_width(ui->screen_play_slider_musictime, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_play_slider_musictime, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style for screen_play_slider_musictime, Part: LV_PART_INDICATOR, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_play_slider_musictime, 255, LV_PART_INDICATOR|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_play_slider_musictime, lv_color_hex(0x2195f6), LV_PART_INDICATOR|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_play_slider_musictime, LV_GRAD_DIR_NONE, LV_PART_INDICATOR|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_play_slider_musictime, 8, LV_PART_INDICATOR|LV_STATE_DEFAULT);

    //Write style for screen_play_slider_musictime, Part: LV_PART_KNOB, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_play_slider_musictime, 255, LV_PART_KNOB|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_play_slider_musictime, lv_color_hex(0x2195f6), LV_PART_KNOB|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_play_slider_musictime, LV_GRAD_DIR_NONE, LV_PART_KNOB|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_play_slider_musictime, 8, LV_PART_KNOB|LV_STATE_DEFAULT);

    //Write codes screen_play_imgbtn_up
    ui->screen_play_imgbtn_up = lv_imagebutton_create(ui->screen_play);
    lv_obj_set_pos(ui->screen_play_imgbtn_up, 35, 178);
    lv_obj_set_size(ui->screen_play_imgbtn_up, 41, 43);
    lv_obj_add_flag(ui->screen_play_imgbtn_up, LV_OBJ_FLAG_CHECKABLE);
    lv_imagebutton_set_src(ui->screen_play_imgbtn_up, LV_IMAGEBUTTON_STATE_RELEASED, &_up_RGB565A8_41x43, NULL, NULL);
    lv_imagebutton_set_src(ui->screen_play_imgbtn_up, LV_IMAGEBUTTON_STATE_PRESSED, &_up_press_RGB565A8_41x43, NULL, NULL);
    lv_imagebutton_set_src(ui->screen_play_imgbtn_up, LV_IMAGEBUTTON_STATE_CHECKED_RELEASED, &_up_RGB565A8_41x43, NULL, NULL);
    lv_imagebutton_set_src(ui->screen_play_imgbtn_up, LV_IMAGEBUTTON_STATE_CHECKED_PRESSED, &_up_press_RGB565A8_41x43, NULL, NULL);
    ui->screen_play_imgbtn_up_label = lv_label_create(ui->screen_play_imgbtn_up);
    lv_label_set_text(ui->screen_play_imgbtn_up_label, "");
    lv_label_set_long_mode(ui->screen_play_imgbtn_up_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(ui->screen_play_imgbtn_up_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(ui->screen_play_imgbtn_up, 0, LV_STATE_DEFAULT);

    //Write style for screen_play_imgbtn_up, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_text_color(ui->screen_play_imgbtn_up, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_play_imgbtn_up, &lv_font_ZiTiQuanWeiJunHeiW22_12, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_play_imgbtn_up, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_play_imgbtn_up, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_play_imgbtn_up, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style for screen_play_imgbtn_up, Part: LV_PART_MAIN, State: LV_STATE_PRESSED.
    lv_obj_set_style_image_recolor_opa(ui->screen_play_imgbtn_up, 0, LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_image_opa(ui->screen_play_imgbtn_up, 255, LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_text_color(ui->screen_play_imgbtn_up, lv_color_hex(0xFF33FF), LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_text_font(ui->screen_play_imgbtn_up, &lv_font_ZiTiQuanWeiJunHeiW22_12, LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_text_opa(ui->screen_play_imgbtn_up, 255, LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_shadow_width(ui->screen_play_imgbtn_up, 0, LV_PART_MAIN|LV_STATE_PRESSED);

    //Write style for screen_play_imgbtn_up, Part: LV_PART_MAIN, State: LV_STATE_CHECKED.
    lv_obj_set_style_image_recolor_opa(ui->screen_play_imgbtn_up, 0, LV_PART_MAIN|LV_STATE_CHECKED);
    lv_obj_set_style_image_opa(ui->screen_play_imgbtn_up, 255, LV_PART_MAIN|LV_STATE_CHECKED);
    lv_obj_set_style_text_color(ui->screen_play_imgbtn_up, lv_color_hex(0xFF33FF), LV_PART_MAIN|LV_STATE_CHECKED);
    lv_obj_set_style_text_font(ui->screen_play_imgbtn_up, &lv_font_ZiTiQuanWeiJunHeiW22_12, LV_PART_MAIN|LV_STATE_CHECKED);
    lv_obj_set_style_text_opa(ui->screen_play_imgbtn_up, 255, LV_PART_MAIN|LV_STATE_CHECKED);
    lv_obj_set_style_shadow_width(ui->screen_play_imgbtn_up, 0, LV_PART_MAIN|LV_STATE_CHECKED);

    //Write style for screen_play_imgbtn_up, Part: LV_PART_MAIN, State: LV_IMAGEBUTTON_STATE_RELEASED.
    lv_obj_set_style_image_recolor_opa(ui->screen_play_imgbtn_up, 0, LV_PART_MAIN|LV_IMAGEBUTTON_STATE_RELEASED);
    lv_obj_set_style_image_opa(ui->screen_play_imgbtn_up, 255, LV_PART_MAIN|LV_IMAGEBUTTON_STATE_RELEASED);

    //Write codes screen_play_imgbtn_down
    ui->screen_play_imgbtn_down = lv_imagebutton_create(ui->screen_play);
    lv_obj_set_pos(ui->screen_play_imgbtn_down, 144, 178);
    lv_obj_set_size(ui->screen_play_imgbtn_down, 41, 43);
    lv_obj_add_flag(ui->screen_play_imgbtn_down, LV_OBJ_FLAG_CHECKABLE);
    lv_imagebutton_set_src(ui->screen_play_imgbtn_down, LV_IMAGEBUTTON_STATE_RELEASED, &_last_RGB565A8_41x43, NULL, NULL);
    lv_imagebutton_set_src(ui->screen_play_imgbtn_down, LV_IMAGEBUTTON_STATE_PRESSED, &_down_press_RGB565A8_41x43, NULL, NULL);
    lv_imagebutton_set_src(ui->screen_play_imgbtn_down, LV_IMAGEBUTTON_STATE_CHECKED_RELEASED, &_last_RGB565A8_41x43, NULL, NULL);
    lv_imagebutton_set_src(ui->screen_play_imgbtn_down, LV_IMAGEBUTTON_STATE_CHECKED_PRESSED, &_down_press_RGB565A8_41x43, NULL, NULL);
    ui->screen_play_imgbtn_down_label = lv_label_create(ui->screen_play_imgbtn_down);
    lv_label_set_text(ui->screen_play_imgbtn_down_label, "");
    lv_label_set_long_mode(ui->screen_play_imgbtn_down_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(ui->screen_play_imgbtn_down_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(ui->screen_play_imgbtn_down, 0, LV_STATE_DEFAULT);

    //Write style for screen_play_imgbtn_down, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_text_color(ui->screen_play_imgbtn_down, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_play_imgbtn_down, &lv_font_ZiTiQuanWeiJunHeiW22_12, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_play_imgbtn_down, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_play_imgbtn_down, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_play_imgbtn_down, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style for screen_play_imgbtn_down, Part: LV_PART_MAIN, State: LV_STATE_PRESSED.
    lv_obj_set_style_image_recolor_opa(ui->screen_play_imgbtn_down, 0, LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_image_opa(ui->screen_play_imgbtn_down, 255, LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_text_color(ui->screen_play_imgbtn_down, lv_color_hex(0xFF33FF), LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_text_font(ui->screen_play_imgbtn_down, &lv_font_ZiTiQuanWeiJunHeiW22_12, LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_text_opa(ui->screen_play_imgbtn_down, 255, LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_shadow_width(ui->screen_play_imgbtn_down, 0, LV_PART_MAIN|LV_STATE_PRESSED);

    //Write style for screen_play_imgbtn_down, Part: LV_PART_MAIN, State: LV_STATE_CHECKED.
    lv_obj_set_style_image_recolor_opa(ui->screen_play_imgbtn_down, 0, LV_PART_MAIN|LV_STATE_CHECKED);
    lv_obj_set_style_image_opa(ui->screen_play_imgbtn_down, 255, LV_PART_MAIN|LV_STATE_CHECKED);
    lv_obj_set_style_text_color(ui->screen_play_imgbtn_down, lv_color_hex(0xFF33FF), LV_PART_MAIN|LV_STATE_CHECKED);
    lv_obj_set_style_text_font(ui->screen_play_imgbtn_down, &lv_font_ZiTiQuanWeiJunHeiW22_12, LV_PART_MAIN|LV_STATE_CHECKED);
    lv_obj_set_style_text_opa(ui->screen_play_imgbtn_down, 255, LV_PART_MAIN|LV_STATE_CHECKED);
    lv_obj_set_style_shadow_width(ui->screen_play_imgbtn_down, 0, LV_PART_MAIN|LV_STATE_CHECKED);

    //Write style for screen_play_imgbtn_down, Part: LV_PART_MAIN, State: LV_IMAGEBUTTON_STATE_RELEASED.
    lv_obj_set_style_image_recolor_opa(ui->screen_play_imgbtn_down, 0, LV_PART_MAIN|LV_IMAGEBUTTON_STATE_RELEASED);
    lv_obj_set_style_image_opa(ui->screen_play_imgbtn_down, 255, LV_PART_MAIN|LV_IMAGEBUTTON_STATE_RELEASED);

    //Write codes screen_play_list_play
    ui->screen_play_list_play = lv_list_create(ui->screen_play);
    lv_obj_set_pos(ui->screen_play_list_play, 90, 178);
    lv_obj_set_size(ui->screen_play_list_play, 43, 43);
    lv_obj_set_scrollbar_mode(ui->screen_play_list_play, LV_SCROLLBAR_MODE_OFF);
    ui->screen_play_list_play_item0 = lv_list_add_button(ui->screen_play_list_play, LV_SYMBOL_PLAY, "");

    //Write style state: LV_STATE_DEFAULT for &style_screen_play_list_play_main_main_default
    static lv_style_t style_screen_play_list_play_main_main_default;
    ui_init_style(&style_screen_play_list_play_main_main_default);

    lv_style_set_pad_top(&style_screen_play_list_play_main_main_default, 5);
    lv_style_set_pad_left(&style_screen_play_list_play_main_main_default, 5);
    lv_style_set_pad_right(&style_screen_play_list_play_main_main_default, 5);
    lv_style_set_pad_bottom(&style_screen_play_list_play_main_main_default, 5);
    lv_style_set_bg_opa(&style_screen_play_list_play_main_main_default, 255);
    lv_style_set_bg_color(&style_screen_play_list_play_main_main_default, lv_color_hex(0x000000));
    lv_style_set_bg_grad_dir(&style_screen_play_list_play_main_main_default, LV_GRAD_DIR_NONE);
    lv_style_set_border_width(&style_screen_play_list_play_main_main_default, 1);
    lv_style_set_border_opa(&style_screen_play_list_play_main_main_default, 255);
    lv_style_set_border_color(&style_screen_play_list_play_main_main_default, lv_color_hex(0x000000));
    lv_style_set_border_side(&style_screen_play_list_play_main_main_default, LV_BORDER_SIDE_FULL);
    lv_style_set_radius(&style_screen_play_list_play_main_main_default, 3);
    lv_style_set_shadow_width(&style_screen_play_list_play_main_main_default, 0);
    lv_obj_add_style(ui->screen_play_list_play, &style_screen_play_list_play_main_main_default, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style state: LV_STATE_DEFAULT for &style_screen_play_list_play_main_scrollbar_default
    static lv_style_t style_screen_play_list_play_main_scrollbar_default;
    ui_init_style(&style_screen_play_list_play_main_scrollbar_default);

    lv_style_set_radius(&style_screen_play_list_play_main_scrollbar_default, 3);
    lv_style_set_bg_opa(&style_screen_play_list_play_main_scrollbar_default, 255);
    lv_style_set_bg_color(&style_screen_play_list_play_main_scrollbar_default, lv_color_hex(0xffffff));
    lv_style_set_bg_grad_dir(&style_screen_play_list_play_main_scrollbar_default, LV_GRAD_DIR_NONE);
    lv_obj_add_style(ui->screen_play_list_play, &style_screen_play_list_play_main_scrollbar_default, LV_PART_SCROLLBAR|LV_STATE_DEFAULT);

    //Write style state: LV_STATE_DEFAULT for &style_screen_play_list_play_extra_btns_main_default
    static lv_style_t style_screen_play_list_play_extra_btns_main_default;
    ui_init_style(&style_screen_play_list_play_extra_btns_main_default);

    lv_style_set_pad_top(&style_screen_play_list_play_extra_btns_main_default, 5);
    lv_style_set_pad_left(&style_screen_play_list_play_extra_btns_main_default, 5);
    lv_style_set_pad_right(&style_screen_play_list_play_extra_btns_main_default, 5);
    lv_style_set_pad_bottom(&style_screen_play_list_play_extra_btns_main_default, 5);
    lv_style_set_border_width(&style_screen_play_list_play_extra_btns_main_default, 0);
    lv_style_set_text_color(&style_screen_play_list_play_extra_btns_main_default, lv_color_hex(0xfafafa));
    lv_style_set_text_font(&style_screen_play_list_play_extra_btns_main_default, &lv_font_ZiTiQuanWeiJunHeiW22_30);
    lv_style_set_text_opa(&style_screen_play_list_play_extra_btns_main_default, 255);
    lv_style_set_radius(&style_screen_play_list_play_extra_btns_main_default, 3);
    lv_style_set_bg_opa(&style_screen_play_list_play_extra_btns_main_default, 255);
    lv_style_set_bg_color(&style_screen_play_list_play_extra_btns_main_default, lv_color_hex(0x050000));
    lv_style_set_bg_grad_dir(&style_screen_play_list_play_extra_btns_main_default, LV_GRAD_DIR_NONE);
    lv_obj_add_style(ui->screen_play_list_play_item0, &style_screen_play_list_play_extra_btns_main_default, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style state: LV_STATE_DEFAULT for &style_screen_play_list_play_extra_texts_main_default
    static lv_style_t style_screen_play_list_play_extra_texts_main_default;
    ui_init_style(&style_screen_play_list_play_extra_texts_main_default);

    lv_style_set_pad_top(&style_screen_play_list_play_extra_texts_main_default, 5);
    lv_style_set_pad_left(&style_screen_play_list_play_extra_texts_main_default, 5);
    lv_style_set_pad_right(&style_screen_play_list_play_extra_texts_main_default, 5);
    lv_style_set_pad_bottom(&style_screen_play_list_play_extra_texts_main_default, 5);
    lv_style_set_border_width(&style_screen_play_list_play_extra_texts_main_default, 0);
    lv_style_set_text_color(&style_screen_play_list_play_extra_texts_main_default, lv_color_hex(0x0D3055));
    lv_style_set_text_font(&style_screen_play_list_play_extra_texts_main_default, &lv_font_ZiTiQuanWeiJunHeiW22_12);
    lv_style_set_text_opa(&style_screen_play_list_play_extra_texts_main_default, 255);
    lv_style_set_radius(&style_screen_play_list_play_extra_texts_main_default, 3);
    lv_style_set_transform_width(&style_screen_play_list_play_extra_texts_main_default, 0);
    lv_style_set_bg_opa(&style_screen_play_list_play_extra_texts_main_default, 255);
    lv_style_set_bg_color(&style_screen_play_list_play_extra_texts_main_default, lv_color_hex(0xffffff));
    lv_style_set_bg_grad_dir(&style_screen_play_list_play_extra_texts_main_default, LV_GRAD_DIR_NONE);

    //Write codes screen_play_list_aduio
    ui->screen_play_list_aduio = lv_list_create(ui->screen_play);
    lv_obj_set_pos(ui->screen_play_list_aduio, 29, 221);
    lv_obj_set_size(ui->screen_play_list_aduio, 35, 32);
    lv_obj_set_scrollbar_mode(ui->screen_play_list_aduio, LV_SCROLLBAR_MODE_OFF);
    ui->screen_play_list_aduio_item0 = lv_list_add_button(ui->screen_play_list_aduio, LV_SYMBOL_VOLUME_MAX, "");

    //Write style state: LV_STATE_DEFAULT for &style_screen_play_list_aduio_main_main_default
    static lv_style_t style_screen_play_list_aduio_main_main_default;
    ui_init_style(&style_screen_play_list_aduio_main_main_default);

    lv_style_set_pad_top(&style_screen_play_list_aduio_main_main_default, 5);
    lv_style_set_pad_left(&style_screen_play_list_aduio_main_main_default, 5);
    lv_style_set_pad_right(&style_screen_play_list_aduio_main_main_default, 5);
    lv_style_set_pad_bottom(&style_screen_play_list_aduio_main_main_default, 5);
    lv_style_set_bg_opa(&style_screen_play_list_aduio_main_main_default, 255);
    lv_style_set_bg_color(&style_screen_play_list_aduio_main_main_default, lv_color_hex(0x010101));
    lv_style_set_bg_grad_dir(&style_screen_play_list_aduio_main_main_default, LV_GRAD_DIR_NONE);
    lv_style_set_border_width(&style_screen_play_list_aduio_main_main_default, 1);
    lv_style_set_border_opa(&style_screen_play_list_aduio_main_main_default, 255);
    lv_style_set_border_color(&style_screen_play_list_aduio_main_main_default, lv_color_hex(0x000000));
    lv_style_set_border_side(&style_screen_play_list_aduio_main_main_default, LV_BORDER_SIDE_FULL);
    lv_style_set_radius(&style_screen_play_list_aduio_main_main_default, 3);
    lv_style_set_shadow_width(&style_screen_play_list_aduio_main_main_default, 0);
    lv_obj_add_style(ui->screen_play_list_aduio, &style_screen_play_list_aduio_main_main_default, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style state: LV_STATE_DEFAULT for &style_screen_play_list_aduio_main_scrollbar_default
    static lv_style_t style_screen_play_list_aduio_main_scrollbar_default;
    ui_init_style(&style_screen_play_list_aduio_main_scrollbar_default);

    lv_style_set_radius(&style_screen_play_list_aduio_main_scrollbar_default, 3);
    lv_style_set_bg_opa(&style_screen_play_list_aduio_main_scrollbar_default, 255);
    lv_style_set_bg_color(&style_screen_play_list_aduio_main_scrollbar_default, lv_color_hex(0xffffff));
    lv_style_set_bg_grad_dir(&style_screen_play_list_aduio_main_scrollbar_default, LV_GRAD_DIR_NONE);
    lv_obj_add_style(ui->screen_play_list_aduio, &style_screen_play_list_aduio_main_scrollbar_default, LV_PART_SCROLLBAR|LV_STATE_DEFAULT);

    //Write style state: LV_STATE_DEFAULT for &style_screen_play_list_aduio_extra_btns_main_default
    static lv_style_t style_screen_play_list_aduio_extra_btns_main_default;
    ui_init_style(&style_screen_play_list_aduio_extra_btns_main_default);

    lv_style_set_pad_top(&style_screen_play_list_aduio_extra_btns_main_default, 5);
    lv_style_set_pad_left(&style_screen_play_list_aduio_extra_btns_main_default, 5);
    lv_style_set_pad_right(&style_screen_play_list_aduio_extra_btns_main_default, 5);
    lv_style_set_pad_bottom(&style_screen_play_list_aduio_extra_btns_main_default, 5);
    lv_style_set_border_width(&style_screen_play_list_aduio_extra_btns_main_default, 0);
    lv_style_set_text_color(&style_screen_play_list_aduio_extra_btns_main_default, lv_color_hex(0xe6e8ea));
    lv_style_set_text_font(&style_screen_play_list_aduio_extra_btns_main_default, &lv_font_ZiTiQuanWeiJunHeiW22_16);
    lv_style_set_text_opa(&style_screen_play_list_aduio_extra_btns_main_default, 255);
    lv_style_set_radius(&style_screen_play_list_aduio_extra_btns_main_default, 3);
    lv_style_set_bg_opa(&style_screen_play_list_aduio_extra_btns_main_default, 255);
    lv_style_set_bg_color(&style_screen_play_list_aduio_extra_btns_main_default, lv_color_hex(0x000000));
    lv_style_set_bg_grad_dir(&style_screen_play_list_aduio_extra_btns_main_default, LV_GRAD_DIR_NONE);
    lv_obj_add_style(ui->screen_play_list_aduio_item0, &style_screen_play_list_aduio_extra_btns_main_default, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style state: LV_STATE_DEFAULT for &style_screen_play_list_aduio_extra_texts_main_default
    static lv_style_t style_screen_play_list_aduio_extra_texts_main_default;
    ui_init_style(&style_screen_play_list_aduio_extra_texts_main_default);

    lv_style_set_pad_top(&style_screen_play_list_aduio_extra_texts_main_default, 5);
    lv_style_set_pad_left(&style_screen_play_list_aduio_extra_texts_main_default, 5);
    lv_style_set_pad_right(&style_screen_play_list_aduio_extra_texts_main_default, 5);
    lv_style_set_pad_bottom(&style_screen_play_list_aduio_extra_texts_main_default, 5);
    lv_style_set_border_width(&style_screen_play_list_aduio_extra_texts_main_default, 0);
    lv_style_set_text_color(&style_screen_play_list_aduio_extra_texts_main_default, lv_color_hex(0x0D3055));
    lv_style_set_text_font(&style_screen_play_list_aduio_extra_texts_main_default, &lv_font_ZiTiQuanWeiJunHeiW22_12);
    lv_style_set_text_opa(&style_screen_play_list_aduio_extra_texts_main_default, 255);
    lv_style_set_radius(&style_screen_play_list_aduio_extra_texts_main_default, 3);
    lv_style_set_transform_width(&style_screen_play_list_aduio_extra_texts_main_default, 0);
    lv_style_set_bg_opa(&style_screen_play_list_aduio_extra_texts_main_default, 255);
    lv_style_set_bg_color(&style_screen_play_list_aduio_extra_texts_main_default, lv_color_hex(0xffffff));
    lv_style_set_bg_grad_dir(&style_screen_play_list_aduio_extra_texts_main_default, LV_GRAD_DIR_NONE);

    //Write codes screen_play_list_3
    ui->screen_play_list_3 = lv_list_create(ui->screen_play);
    lv_obj_set_pos(ui->screen_play_list_3, 165, 0);
    lv_obj_set_size(ui->screen_play_list_3, 64, 31);
    lv_obj_set_scrollbar_mode(ui->screen_play_list_3, LV_SCROLLBAR_MODE_OFF);
    ui->screen_play_list_3_item0 = lv_list_add_button(ui->screen_play_list_3, NULL, "--");

    //Write style state: LV_STATE_DEFAULT for &style_screen_play_list_3_main_main_default
    static lv_style_t style_screen_play_list_3_main_main_default;
    ui_init_style(&style_screen_play_list_3_main_main_default);

    lv_style_set_pad_top(&style_screen_play_list_3_main_main_default, 5);
    lv_style_set_pad_left(&style_screen_play_list_3_main_main_default, 5);
    lv_style_set_pad_right(&style_screen_play_list_3_main_main_default, 5);
    lv_style_set_pad_bottom(&style_screen_play_list_3_main_main_default, 5);
    lv_style_set_bg_opa(&style_screen_play_list_3_main_main_default, 255);
    lv_style_set_bg_color(&style_screen_play_list_3_main_main_default, lv_color_hex(0x050505));
    lv_style_set_bg_grad_dir(&style_screen_play_list_3_main_main_default, LV_GRAD_DIR_NONE);
    lv_style_set_border_width(&style_screen_play_list_3_main_main_default, 1);
    lv_style_set_border_opa(&style_screen_play_list_3_main_main_default, 255);
    lv_style_set_border_color(&style_screen_play_list_3_main_main_default, lv_color_hex(0x000000));
    lv_style_set_border_side(&style_screen_play_list_3_main_main_default, LV_BORDER_SIDE_FULL);
    lv_style_set_radius(&style_screen_play_list_3_main_main_default, 3);
    lv_style_set_shadow_width(&style_screen_play_list_3_main_main_default, 0);
    lv_obj_add_style(ui->screen_play_list_3, &style_screen_play_list_3_main_main_default, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style state: LV_STATE_DEFAULT for &style_screen_play_list_3_main_scrollbar_default
    static lv_style_t style_screen_play_list_3_main_scrollbar_default;
    ui_init_style(&style_screen_play_list_3_main_scrollbar_default);

    lv_style_set_radius(&style_screen_play_list_3_main_scrollbar_default, 3);
    lv_style_set_bg_opa(&style_screen_play_list_3_main_scrollbar_default, 255);
    lv_style_set_bg_color(&style_screen_play_list_3_main_scrollbar_default, lv_color_hex(0x000000));
    lv_style_set_bg_grad_dir(&style_screen_play_list_3_main_scrollbar_default, LV_GRAD_DIR_NONE);
    lv_obj_add_style(ui->screen_play_list_3, &style_screen_play_list_3_main_scrollbar_default, LV_PART_SCROLLBAR|LV_STATE_DEFAULT);

    //Write style state: LV_STATE_DEFAULT for &style_screen_play_list_3_extra_btns_main_default
    static lv_style_t style_screen_play_list_3_extra_btns_main_default;
    ui_init_style(&style_screen_play_list_3_extra_btns_main_default);

    lv_style_set_pad_top(&style_screen_play_list_3_extra_btns_main_default, 5);
    lv_style_set_pad_left(&style_screen_play_list_3_extra_btns_main_default, 5);
    lv_style_set_pad_right(&style_screen_play_list_3_extra_btns_main_default, 5);
    lv_style_set_pad_bottom(&style_screen_play_list_3_extra_btns_main_default, 5);
    lv_style_set_border_width(&style_screen_play_list_3_extra_btns_main_default, 0);
    lv_style_set_text_color(&style_screen_play_list_3_extra_btns_main_default, lv_color_hex(0xfbfbfb));
    lv_style_set_text_font(&style_screen_play_list_3_extra_btns_main_default, &lv_font_ZiTiQuanWeiJunHeiW22_12);
    lv_style_set_text_opa(&style_screen_play_list_3_extra_btns_main_default, 255);
    lv_style_set_radius(&style_screen_play_list_3_extra_btns_main_default, 3);
    lv_style_set_bg_opa(&style_screen_play_list_3_extra_btns_main_default, 255);
    lv_style_set_bg_color(&style_screen_play_list_3_extra_btns_main_default, lv_color_hex(0x000000));
    lv_style_set_bg_grad_dir(&style_screen_play_list_3_extra_btns_main_default, LV_GRAD_DIR_NONE);
    lv_obj_add_style(ui->screen_play_list_3_item0, &style_screen_play_list_3_extra_btns_main_default, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style state: LV_STATE_DEFAULT for &style_screen_play_list_3_extra_texts_main_default
    static lv_style_t style_screen_play_list_3_extra_texts_main_default;
    ui_init_style(&style_screen_play_list_3_extra_texts_main_default);

    lv_style_set_pad_top(&style_screen_play_list_3_extra_texts_main_default, 5);
    lv_style_set_pad_left(&style_screen_play_list_3_extra_texts_main_default, 5);
    lv_style_set_pad_right(&style_screen_play_list_3_extra_texts_main_default, 5);
    lv_style_set_pad_bottom(&style_screen_play_list_3_extra_texts_main_default, 5);
    lv_style_set_border_width(&style_screen_play_list_3_extra_texts_main_default, 0);
    lv_style_set_text_color(&style_screen_play_list_3_extra_texts_main_default, lv_color_hex(0xf9f9f9));
    lv_style_set_text_font(&style_screen_play_list_3_extra_texts_main_default, &lv_font_ZiTiQuanWeiJunHeiW22_12);
    lv_style_set_text_opa(&style_screen_play_list_3_extra_texts_main_default, 255);
    lv_style_set_radius(&style_screen_play_list_3_extra_texts_main_default, 3);
    lv_style_set_transform_width(&style_screen_play_list_3_extra_texts_main_default, 0);
    lv_style_set_bg_opa(&style_screen_play_list_3_extra_texts_main_default, 255);
    lv_style_set_bg_color(&style_screen_play_list_3_extra_texts_main_default, lv_color_hex(0xffffff));
    lv_style_set_bg_grad_dir(&style_screen_play_list_3_extra_texts_main_default, LV_GRAD_DIR_NONE);

    //Write codes screen_play_img_wifi
    ui->screen_play_img_wifi = lv_image_create(ui->screen_play);
    lv_obj_set_pos(ui->screen_play_img_wifi, 138, 9);
    lv_obj_set_size(ui->screen_play_img_wifi, 20, 20);
    lv_obj_add_flag(ui->screen_play_img_wifi, LV_OBJ_FLAG_CLICKABLE);
    lv_image_set_src(ui->screen_play_img_wifi, &_wifi_close_RGB565A8_20x20);
    lv_image_set_pivot(ui->screen_play_img_wifi, 50,50);
    lv_image_set_rotation(ui->screen_play_img_wifi, 0);

    //Write style for screen_play_img_wifi, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_image_recolor_opa(ui->screen_play_img_wifi, 26, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_image_recolor(ui->screen_play_img_wifi, lv_color_hex(0x090000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_image_opa(ui->screen_play_img_wifi, 255, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_play_digital_clock_1
    static bool screen_play_digital_clock_1_timer_enabled = false;
    ui->screen_play_digital_clock_1 = lv_label_create(ui->screen_play);
    lv_obj_set_pos(ui->screen_play_digital_clock_1, 8, 5);
    lv_obj_set_size(ui->screen_play_digital_clock_1, 53, 22);
    lv_label_set_text(ui->screen_play_digital_clock_1, "11:25");
    if (!screen_play_digital_clock_1_timer_enabled) {
        lv_timer_create(screen_play_digital_clock_1_timer, 1000, NULL);
        screen_play_digital_clock_1_timer_enabled = true;
    }

    //Write style for screen_play_digital_clock_1, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_radius(ui->screen_play_digital_clock_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_play_digital_clock_1, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_play_digital_clock_1, &lv_font_ZiTiQuanWeiJunHeiW22_18, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_play_digital_clock_1, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->screen_play_digital_clock_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_play_digital_clock_1, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_play_digital_clock_1, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_play_digital_clock_1, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_play_digital_clock_1, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_play_digital_clock_1, 7, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_play_digital_clock_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_play_digital_clock_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_play_digital_clock_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_play_digital_clock_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_play_list_battery
    ui->screen_play_list_battery = lv_list_create(ui->screen_play);
    lv_obj_set_pos(ui->screen_play_list_battery, 164, 1);
    lv_obj_set_size(ui->screen_play_list_battery, 68, 33);
    lv_obj_set_scrollbar_mode(ui->screen_play_list_battery, LV_SCROLLBAR_MODE_OFF);
    ui->screen_play_list_battery_item0 = lv_list_add_button(ui->screen_play_list_battery, NULL, "--");

    //Write style state: LV_STATE_DEFAULT for &style_screen_play_list_battery_main_main_default
    static lv_style_t style_screen_play_list_battery_main_main_default;
    ui_init_style(&style_screen_play_list_battery_main_main_default);

    lv_style_set_pad_top(&style_screen_play_list_battery_main_main_default, 5);
    lv_style_set_pad_left(&style_screen_play_list_battery_main_main_default, 5);
    lv_style_set_pad_right(&style_screen_play_list_battery_main_main_default, 5);
    lv_style_set_pad_bottom(&style_screen_play_list_battery_main_main_default, 5);
    lv_style_set_bg_opa(&style_screen_play_list_battery_main_main_default, 255);
    lv_style_set_bg_color(&style_screen_play_list_battery_main_main_default, lv_color_hex(0x050505));
    lv_style_set_bg_grad_dir(&style_screen_play_list_battery_main_main_default, LV_GRAD_DIR_NONE);
    lv_style_set_border_width(&style_screen_play_list_battery_main_main_default, 1);
    lv_style_set_border_opa(&style_screen_play_list_battery_main_main_default, 255);
    lv_style_set_border_color(&style_screen_play_list_battery_main_main_default, lv_color_hex(0x000000));
    lv_style_set_border_side(&style_screen_play_list_battery_main_main_default, LV_BORDER_SIDE_FULL);
    lv_style_set_radius(&style_screen_play_list_battery_main_main_default, 3);
    lv_style_set_shadow_width(&style_screen_play_list_battery_main_main_default, 0);
    lv_obj_add_style(ui->screen_play_list_battery, &style_screen_play_list_battery_main_main_default, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style state: LV_STATE_DEFAULT for &style_screen_play_list_battery_main_scrollbar_default
    static lv_style_t style_screen_play_list_battery_main_scrollbar_default;
    ui_init_style(&style_screen_play_list_battery_main_scrollbar_default);

    lv_style_set_radius(&style_screen_play_list_battery_main_scrollbar_default, 3);
    lv_style_set_bg_opa(&style_screen_play_list_battery_main_scrollbar_default, 255);
    lv_style_set_bg_color(&style_screen_play_list_battery_main_scrollbar_default, lv_color_hex(0x000000));
    lv_style_set_bg_grad_dir(&style_screen_play_list_battery_main_scrollbar_default, LV_GRAD_DIR_NONE);
    lv_obj_add_style(ui->screen_play_list_battery, &style_screen_play_list_battery_main_scrollbar_default, LV_PART_SCROLLBAR|LV_STATE_DEFAULT);

    //Write style state: LV_STATE_DEFAULT for &style_screen_play_list_battery_extra_btns_main_default
    static lv_style_t style_screen_play_list_battery_extra_btns_main_default;
    ui_init_style(&style_screen_play_list_battery_extra_btns_main_default);

    lv_style_set_pad_top(&style_screen_play_list_battery_extra_btns_main_default, 5);
    lv_style_set_pad_left(&style_screen_play_list_battery_extra_btns_main_default, 5);
    lv_style_set_pad_right(&style_screen_play_list_battery_extra_btns_main_default, 5);
    lv_style_set_pad_bottom(&style_screen_play_list_battery_extra_btns_main_default, 5);
    lv_style_set_border_width(&style_screen_play_list_battery_extra_btns_main_default, 0);
    lv_style_set_text_color(&style_screen_play_list_battery_extra_btns_main_default, lv_color_hex(0xfbfbfb));
    lv_style_set_text_font(&style_screen_play_list_battery_extra_btns_main_default, &lv_font_ZiTiQuanWeiJunHeiW22_12);
    lv_style_set_text_opa(&style_screen_play_list_battery_extra_btns_main_default, 255);
    lv_style_set_radius(&style_screen_play_list_battery_extra_btns_main_default, 3);
    lv_style_set_bg_opa(&style_screen_play_list_battery_extra_btns_main_default, 255);
    lv_style_set_bg_color(&style_screen_play_list_battery_extra_btns_main_default, lv_color_hex(0x000000));
    lv_style_set_bg_grad_dir(&style_screen_play_list_battery_extra_btns_main_default, LV_GRAD_DIR_NONE);
    lv_obj_add_style(ui->screen_play_list_battery_item0, &style_screen_play_list_battery_extra_btns_main_default, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style state: LV_STATE_DEFAULT for &style_screen_play_list_battery_extra_texts_main_default
    static lv_style_t style_screen_play_list_battery_extra_texts_main_default;
    ui_init_style(&style_screen_play_list_battery_extra_texts_main_default);

    lv_style_set_pad_top(&style_screen_play_list_battery_extra_texts_main_default, 5);
    lv_style_set_pad_left(&style_screen_play_list_battery_extra_texts_main_default, 5);
    lv_style_set_pad_right(&style_screen_play_list_battery_extra_texts_main_default, 5);
    lv_style_set_pad_bottom(&style_screen_play_list_battery_extra_texts_main_default, 5);
    lv_style_set_border_width(&style_screen_play_list_battery_extra_texts_main_default, 0);
    lv_style_set_text_color(&style_screen_play_list_battery_extra_texts_main_default, lv_color_hex(0xf9f9f9));
    lv_style_set_text_font(&style_screen_play_list_battery_extra_texts_main_default, &lv_font_ZiTiQuanWeiJunHeiW22_12);
    lv_style_set_text_opa(&style_screen_play_list_battery_extra_texts_main_default, 255);
    lv_style_set_radius(&style_screen_play_list_battery_extra_texts_main_default, 3);
    lv_style_set_transform_width(&style_screen_play_list_battery_extra_texts_main_default, 0);
    lv_style_set_bg_opa(&style_screen_play_list_battery_extra_texts_main_default, 255);
    lv_style_set_bg_color(&style_screen_play_list_battery_extra_texts_main_default, lv_color_hex(0xffffff));
    lv_style_set_bg_grad_dir(&style_screen_play_list_battery_extra_texts_main_default, LV_GRAD_DIR_NONE);

    //Write codes screen_play_slider_aduio
    ui->screen_play_slider_aduio = lv_slider_create(ui->screen_play);
    lv_obj_set_pos(ui->screen_play_slider_aduio, 70, 238);
    lv_obj_set_size(ui->screen_play_slider_aduio, 120, 4);
    lv_slider_set_range(ui->screen_play_slider_aduio, 0, 100);
    lv_slider_set_mode(ui->screen_play_slider_aduio, LV_SLIDER_MODE_NORMAL);
    lv_slider_set_value(ui->screen_play_slider_aduio, 50, LV_ANIM_OFF);

    //Write style for screen_play_slider_aduio, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_play_slider_aduio, 60, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_play_slider_aduio, lv_color_hex(0x878484), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_play_slider_aduio, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_play_slider_aduio, 8, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_outline_width(ui->screen_play_slider_aduio, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_play_slider_aduio, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style for screen_play_slider_aduio, Part: LV_PART_INDICATOR, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_play_slider_aduio, 255, LV_PART_INDICATOR|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_play_slider_aduio, lv_color_hex(0xf9f9f9), LV_PART_INDICATOR|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_play_slider_aduio, LV_GRAD_DIR_NONE, LV_PART_INDICATOR|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_play_slider_aduio, 3, LV_PART_INDICATOR|LV_STATE_DEFAULT);

    //Write style for screen_play_slider_aduio, Part: LV_PART_KNOB, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_play_slider_aduio, 255, LV_PART_KNOB|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_play_slider_aduio, lv_color_hex(0xf6f6f6), LV_PART_KNOB|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_play_slider_aduio, LV_GRAD_DIR_NONE, LV_PART_KNOB|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_play_slider_aduio, 8, LV_PART_KNOB|LV_STATE_DEFAULT);

    //The custom code of screen_play.
    lv_obj_null_on_delete(&ui->screen_play_digital_clock_1);
    music_ui_bind_play(ui);

    //Update current screen layout.
    lv_obj_update_layout(ui->screen_play);

    //Init events for screen.
    events_init_screen_play(ui);
}
