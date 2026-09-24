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



void setup_scr_screen_selete(lv_ui *ui)
{
    //Write codes screen_selete
    ui->screen_selete = lv_obj_create(NULL);
    lv_obj_set_size(ui->screen_selete, 240, 284);
    lv_obj_set_scrollbar_mode(ui->screen_selete, LV_SCROLLBAR_MODE_OFF);

    //Write style for screen_selete, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_selete, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_selete, lv_color_hex(0x0a0a0a), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_selete, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_selete_imgbtn_wifi
    ui->screen_selete_imgbtn_wifi = lv_imagebutton_create(ui->screen_selete);
    lv_obj_set_pos(ui->screen_selete_imgbtn_wifi, 25, 27);
    lv_obj_set_size(ui->screen_selete_imgbtn_wifi, 77, 59);
    lv_obj_add_flag(ui->screen_selete_imgbtn_wifi, LV_OBJ_FLAG_CHECKABLE);
    lv_imagebutton_set_src(ui->screen_selete_imgbtn_wifi, LV_IMAGEBUTTON_STATE_RELEASED, &_WiFi2_RGB565A8_77x59, NULL, NULL);
    lv_imagebutton_set_src(ui->screen_selete_imgbtn_wifi, LV_IMAGEBUTTON_STATE_PRESSED, &_wifi_press_RGB565A8_77x59, NULL, NULL);
    lv_imagebutton_set_src(ui->screen_selete_imgbtn_wifi, LV_IMAGEBUTTON_STATE_CHECKED_RELEASED, &_WiFi2_RGB565A8_77x59, NULL, NULL);
    lv_imagebutton_set_src(ui->screen_selete_imgbtn_wifi, LV_IMAGEBUTTON_STATE_CHECKED_PRESSED, &_wifi_press_RGB565A8_77x59, NULL, NULL);
    ui->screen_selete_imgbtn_wifi_label = lv_label_create(ui->screen_selete_imgbtn_wifi);
    lv_label_set_text(ui->screen_selete_imgbtn_wifi_label, "");
    lv_label_set_long_mode(ui->screen_selete_imgbtn_wifi_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(ui->screen_selete_imgbtn_wifi_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(ui->screen_selete_imgbtn_wifi, 0, LV_STATE_DEFAULT);

    //Write style for screen_selete_imgbtn_wifi, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_text_color(ui->screen_selete_imgbtn_wifi, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_selete_imgbtn_wifi, &lv_font_ZiTiQuanWeiJunHeiW22_12, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_selete_imgbtn_wifi, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_selete_imgbtn_wifi, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_selete_imgbtn_wifi, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style for screen_selete_imgbtn_wifi, Part: LV_PART_MAIN, State: LV_STATE_PRESSED.
    lv_obj_set_style_image_recolor_opa(ui->screen_selete_imgbtn_wifi, 0, LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_image_opa(ui->screen_selete_imgbtn_wifi, 255, LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_text_color(ui->screen_selete_imgbtn_wifi, lv_color_hex(0xFF33FF), LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_text_font(ui->screen_selete_imgbtn_wifi, &lv_font_ZiTiQuanWeiJunHeiW22_12, LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_text_opa(ui->screen_selete_imgbtn_wifi, 255, LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_shadow_width(ui->screen_selete_imgbtn_wifi, 0, LV_PART_MAIN|LV_STATE_PRESSED);

    //Write style for screen_selete_imgbtn_wifi, Part: LV_PART_MAIN, State: LV_STATE_CHECKED.
    lv_obj_set_style_image_recolor_opa(ui->screen_selete_imgbtn_wifi, 0, LV_PART_MAIN|LV_STATE_CHECKED);
    lv_obj_set_style_image_opa(ui->screen_selete_imgbtn_wifi, 255, LV_PART_MAIN|LV_STATE_CHECKED);
    lv_obj_set_style_text_color(ui->screen_selete_imgbtn_wifi, lv_color_hex(0xFF33FF), LV_PART_MAIN|LV_STATE_CHECKED);
    lv_obj_set_style_text_font(ui->screen_selete_imgbtn_wifi, &lv_font_ZiTiQuanWeiJunHeiW22_12, LV_PART_MAIN|LV_STATE_CHECKED);
    lv_obj_set_style_text_opa(ui->screen_selete_imgbtn_wifi, 255, LV_PART_MAIN|LV_STATE_CHECKED);
    lv_obj_set_style_shadow_width(ui->screen_selete_imgbtn_wifi, 0, LV_PART_MAIN|LV_STATE_CHECKED);

    //Write style for screen_selete_imgbtn_wifi, Part: LV_PART_MAIN, State: LV_IMAGEBUTTON_STATE_RELEASED.
    lv_obj_set_style_image_recolor_opa(ui->screen_selete_imgbtn_wifi, 0, LV_PART_MAIN|LV_IMAGEBUTTON_STATE_RELEASED);
    lv_obj_set_style_image_opa(ui->screen_selete_imgbtn_wifi, 255, LV_PART_MAIN|LV_IMAGEBUTTON_STATE_RELEASED);

    //Write codes screen_selete_imgbtn_weather
    ui->screen_selete_imgbtn_weather = lv_imagebutton_create(ui->screen_selete);
    lv_obj_set_pos(ui->screen_selete_imgbtn_weather, 136, 27);
    lv_obj_set_size(ui->screen_selete_imgbtn_weather, 77, 59);
    lv_obj_add_flag(ui->screen_selete_imgbtn_weather, LV_OBJ_FLAG_CHECKABLE);
    lv_imagebutton_set_src(ui->screen_selete_imgbtn_weather, LV_IMAGEBUTTON_STATE_RELEASED, &_weather_RGB565A8_77x59, NULL, NULL);
    lv_imagebutton_set_src(ui->screen_selete_imgbtn_weather, LV_IMAGEBUTTON_STATE_PRESSED, &_wether_press_RGB565A8_77x59, NULL, NULL);
    lv_imagebutton_set_src(ui->screen_selete_imgbtn_weather, LV_IMAGEBUTTON_STATE_CHECKED_RELEASED, &_weather_RGB565A8_77x59, NULL, NULL);
    lv_imagebutton_set_src(ui->screen_selete_imgbtn_weather, LV_IMAGEBUTTON_STATE_CHECKED_PRESSED, &_wether_press_RGB565A8_77x59, NULL, NULL);
    ui->screen_selete_imgbtn_weather_label = lv_label_create(ui->screen_selete_imgbtn_weather);
    lv_label_set_text(ui->screen_selete_imgbtn_weather_label, "");
    lv_label_set_long_mode(ui->screen_selete_imgbtn_weather_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(ui->screen_selete_imgbtn_weather_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(ui->screen_selete_imgbtn_weather, 0, LV_STATE_DEFAULT);

    //Write style for screen_selete_imgbtn_weather, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_text_color(ui->screen_selete_imgbtn_weather, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_selete_imgbtn_weather, &lv_font_ZiTiQuanWeiJunHeiW22_12, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_selete_imgbtn_weather, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_selete_imgbtn_weather, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_selete_imgbtn_weather, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style for screen_selete_imgbtn_weather, Part: LV_PART_MAIN, State: LV_STATE_PRESSED.
    lv_obj_set_style_image_recolor_opa(ui->screen_selete_imgbtn_weather, 0, LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_image_opa(ui->screen_selete_imgbtn_weather, 255, LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_text_color(ui->screen_selete_imgbtn_weather, lv_color_hex(0xFF33FF), LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_text_font(ui->screen_selete_imgbtn_weather, &lv_font_ZiTiQuanWeiJunHeiW22_12, LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_text_opa(ui->screen_selete_imgbtn_weather, 255, LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_shadow_width(ui->screen_selete_imgbtn_weather, 0, LV_PART_MAIN|LV_STATE_PRESSED);

    //Write style for screen_selete_imgbtn_weather, Part: LV_PART_MAIN, State: LV_STATE_CHECKED.
    lv_obj_set_style_image_recolor_opa(ui->screen_selete_imgbtn_weather, 0, LV_PART_MAIN|LV_STATE_CHECKED);
    lv_obj_set_style_image_opa(ui->screen_selete_imgbtn_weather, 255, LV_PART_MAIN|LV_STATE_CHECKED);
    lv_obj_set_style_text_color(ui->screen_selete_imgbtn_weather, lv_color_hex(0xFF33FF), LV_PART_MAIN|LV_STATE_CHECKED);
    lv_obj_set_style_text_font(ui->screen_selete_imgbtn_weather, &lv_font_ZiTiQuanWeiJunHeiW22_12, LV_PART_MAIN|LV_STATE_CHECKED);
    lv_obj_set_style_text_opa(ui->screen_selete_imgbtn_weather, 255, LV_PART_MAIN|LV_STATE_CHECKED);
    lv_obj_set_style_shadow_width(ui->screen_selete_imgbtn_weather, 0, LV_PART_MAIN|LV_STATE_CHECKED);

    //Write style for screen_selete_imgbtn_weather, Part: LV_PART_MAIN, State: LV_IMAGEBUTTON_STATE_RELEASED.
    lv_obj_set_style_image_recolor_opa(ui->screen_selete_imgbtn_weather, 0, LV_PART_MAIN|LV_IMAGEBUTTON_STATE_RELEASED);
    lv_obj_set_style_image_opa(ui->screen_selete_imgbtn_weather, 255, LV_PART_MAIN|LV_IMAGEBUTTON_STATE_RELEASED);

    //Write codes screen_selete_imgbtn_rli
    ui->screen_selete_imgbtn_rli = lv_imagebutton_create(ui->screen_selete);
    lv_obj_set_pos(ui->screen_selete_imgbtn_rli, 25, 112);
    lv_obj_set_size(ui->screen_selete_imgbtn_rli, 77, 55);
    lv_obj_add_flag(ui->screen_selete_imgbtn_rli, LV_OBJ_FLAG_CHECKABLE);
    lv_imagebutton_set_src(ui->screen_selete_imgbtn_rli, LV_IMAGEBUTTON_STATE_RELEASED, &_rli_RGB565A8_77x55, NULL, NULL);
    lv_imagebutton_set_src(ui->screen_selete_imgbtn_rli, LV_IMAGEBUTTON_STATE_PRESSED, &_rli_press_RGB565A8_77x55, NULL, NULL);
    lv_imagebutton_set_src(ui->screen_selete_imgbtn_rli, LV_IMAGEBUTTON_STATE_CHECKED_RELEASED, &_rli_RGB565A8_77x55, NULL, NULL);
    lv_imagebutton_set_src(ui->screen_selete_imgbtn_rli, LV_IMAGEBUTTON_STATE_CHECKED_PRESSED, &_rli_press_RGB565A8_77x55, NULL, NULL);
    ui->screen_selete_imgbtn_rli_label = lv_label_create(ui->screen_selete_imgbtn_rli);
    lv_label_set_text(ui->screen_selete_imgbtn_rli_label, "");
    lv_label_set_long_mode(ui->screen_selete_imgbtn_rli_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(ui->screen_selete_imgbtn_rli_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(ui->screen_selete_imgbtn_rli, 0, LV_STATE_DEFAULT);

    //Write style for screen_selete_imgbtn_rli, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_text_color(ui->screen_selete_imgbtn_rli, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_selete_imgbtn_rli, &lv_font_ZiTiQuanWeiJunHeiW22_12, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_selete_imgbtn_rli, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_selete_imgbtn_rli, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_selete_imgbtn_rli, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style for screen_selete_imgbtn_rli, Part: LV_PART_MAIN, State: LV_STATE_PRESSED.
    lv_obj_set_style_image_recolor_opa(ui->screen_selete_imgbtn_rli, 0, LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_image_opa(ui->screen_selete_imgbtn_rli, 255, LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_text_color(ui->screen_selete_imgbtn_rli, lv_color_hex(0xFF33FF), LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_text_font(ui->screen_selete_imgbtn_rli, &lv_font_ZiTiQuanWeiJunHeiW22_12, LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_text_opa(ui->screen_selete_imgbtn_rli, 255, LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_shadow_width(ui->screen_selete_imgbtn_rli, 0, LV_PART_MAIN|LV_STATE_PRESSED);

    //Write style for screen_selete_imgbtn_rli, Part: LV_PART_MAIN, State: LV_STATE_CHECKED.
    lv_obj_set_style_image_recolor_opa(ui->screen_selete_imgbtn_rli, 0, LV_PART_MAIN|LV_STATE_CHECKED);
    lv_obj_set_style_image_opa(ui->screen_selete_imgbtn_rli, 255, LV_PART_MAIN|LV_STATE_CHECKED);
    lv_obj_set_style_text_color(ui->screen_selete_imgbtn_rli, lv_color_hex(0xFF33FF), LV_PART_MAIN|LV_STATE_CHECKED);
    lv_obj_set_style_text_font(ui->screen_selete_imgbtn_rli, &lv_font_ZiTiQuanWeiJunHeiW22_12, LV_PART_MAIN|LV_STATE_CHECKED);
    lv_obj_set_style_text_opa(ui->screen_selete_imgbtn_rli, 255, LV_PART_MAIN|LV_STATE_CHECKED);
    lv_obj_set_style_shadow_width(ui->screen_selete_imgbtn_rli, 0, LV_PART_MAIN|LV_STATE_CHECKED);

    //Write style for screen_selete_imgbtn_rli, Part: LV_PART_MAIN, State: LV_IMAGEBUTTON_STATE_RELEASED.
    lv_obj_set_style_image_recolor_opa(ui->screen_selete_imgbtn_rli, 0, LV_PART_MAIN|LV_IMAGEBUTTON_STATE_RELEASED);
    lv_obj_set_style_image_opa(ui->screen_selete_imgbtn_rli, 255, LV_PART_MAIN|LV_IMAGEBUTTON_STATE_RELEASED);

    //Write codes screen_selete_imgbtn_AI
    ui->screen_selete_imgbtn_AI = lv_imagebutton_create(ui->screen_selete);
    lv_obj_set_pos(ui->screen_selete_imgbtn_AI, 136, 112);
    lv_obj_set_size(ui->screen_selete_imgbtn_AI, 77, 55);
    lv_obj_add_flag(ui->screen_selete_imgbtn_AI, LV_OBJ_FLAG_CHECKABLE);
    lv_imagebutton_set_src(ui->screen_selete_imgbtn_AI, LV_IMAGEBUTTON_STATE_RELEASED, &_AI_RGB565A8_77x55, NULL, NULL);
    lv_imagebutton_set_src(ui->screen_selete_imgbtn_AI, LV_IMAGEBUTTON_STATE_PRESSED, &_AI_Press_RGB565A8_77x55, NULL, NULL);
    lv_imagebutton_set_src(ui->screen_selete_imgbtn_AI, LV_IMAGEBUTTON_STATE_CHECKED_RELEASED, &_AI_RGB565A8_77x55, NULL, NULL);
    lv_imagebutton_set_src(ui->screen_selete_imgbtn_AI, LV_IMAGEBUTTON_STATE_CHECKED_PRESSED, &_AI_Press_RGB565A8_77x55, NULL, NULL);
    ui->screen_selete_imgbtn_AI_label = lv_label_create(ui->screen_selete_imgbtn_AI);
    lv_label_set_text(ui->screen_selete_imgbtn_AI_label, "");
    lv_label_set_long_mode(ui->screen_selete_imgbtn_AI_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(ui->screen_selete_imgbtn_AI_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(ui->screen_selete_imgbtn_AI, 0, LV_STATE_DEFAULT);

    //Write style for screen_selete_imgbtn_AI, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_text_color(ui->screen_selete_imgbtn_AI, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_selete_imgbtn_AI, &lv_font_ZiTiQuanWeiJunHeiW22_12, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_selete_imgbtn_AI, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_selete_imgbtn_AI, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_selete_imgbtn_AI, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style for screen_selete_imgbtn_AI, Part: LV_PART_MAIN, State: LV_STATE_PRESSED.
    lv_obj_set_style_image_recolor_opa(ui->screen_selete_imgbtn_AI, 0, LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_image_opa(ui->screen_selete_imgbtn_AI, 255, LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_text_color(ui->screen_selete_imgbtn_AI, lv_color_hex(0xFF33FF), LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_text_font(ui->screen_selete_imgbtn_AI, &lv_font_ZiTiQuanWeiJunHeiW22_12, LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_text_opa(ui->screen_selete_imgbtn_AI, 255, LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_shadow_width(ui->screen_selete_imgbtn_AI, 0, LV_PART_MAIN|LV_STATE_PRESSED);

    //Write style for screen_selete_imgbtn_AI, Part: LV_PART_MAIN, State: LV_STATE_CHECKED.
    lv_obj_set_style_image_recolor_opa(ui->screen_selete_imgbtn_AI, 0, LV_PART_MAIN|LV_STATE_CHECKED);
    lv_obj_set_style_image_opa(ui->screen_selete_imgbtn_AI, 255, LV_PART_MAIN|LV_STATE_CHECKED);
    lv_obj_set_style_text_color(ui->screen_selete_imgbtn_AI, lv_color_hex(0xFF33FF), LV_PART_MAIN|LV_STATE_CHECKED);
    lv_obj_set_style_text_font(ui->screen_selete_imgbtn_AI, &lv_font_ZiTiQuanWeiJunHeiW22_12, LV_PART_MAIN|LV_STATE_CHECKED);
    lv_obj_set_style_text_opa(ui->screen_selete_imgbtn_AI, 255, LV_PART_MAIN|LV_STATE_CHECKED);
    lv_obj_set_style_shadow_width(ui->screen_selete_imgbtn_AI, 0, LV_PART_MAIN|LV_STATE_CHECKED);

    //Write style for screen_selete_imgbtn_AI, Part: LV_PART_MAIN, State: LV_IMAGEBUTTON_STATE_RELEASED.
    lv_obj_set_style_image_recolor_opa(ui->screen_selete_imgbtn_AI, 0, LV_PART_MAIN|LV_IMAGEBUTTON_STATE_RELEASED);
    lv_obj_set_style_image_opa(ui->screen_selete_imgbtn_AI, 255, LV_PART_MAIN|LV_IMAGEBUTTON_STATE_RELEASED);

    //Write codes screen_selete_imgbtn_clock
    ui->screen_selete_imgbtn_clock = lv_imagebutton_create(ui->screen_selete);
    lv_obj_set_pos(ui->screen_selete_imgbtn_clock, 136, 194);
    lv_obj_set_size(ui->screen_selete_imgbtn_clock, 77, 64);
    lv_obj_add_flag(ui->screen_selete_imgbtn_clock, LV_OBJ_FLAG_CHECKABLE);
    lv_imagebutton_set_src(ui->screen_selete_imgbtn_clock, LV_IMAGEBUTTON_STATE_RELEASED, &_clock_RGB565A8_77x64, NULL, NULL);
    lv_imagebutton_set_src(ui->screen_selete_imgbtn_clock, LV_IMAGEBUTTON_STATE_PRESSED, &_clopress_RGB565A8_77x64, NULL, NULL);
    lv_imagebutton_set_src(ui->screen_selete_imgbtn_clock, LV_IMAGEBUTTON_STATE_CHECKED_RELEASED, &_clock_RGB565A8_77x64, NULL, NULL);
    lv_imagebutton_set_src(ui->screen_selete_imgbtn_clock, LV_IMAGEBUTTON_STATE_CHECKED_PRESSED, &_clopress_RGB565A8_77x64, NULL, NULL);
    ui->screen_selete_imgbtn_clock_label = lv_label_create(ui->screen_selete_imgbtn_clock);
    lv_label_set_text(ui->screen_selete_imgbtn_clock_label, "");
    lv_label_set_long_mode(ui->screen_selete_imgbtn_clock_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(ui->screen_selete_imgbtn_clock_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(ui->screen_selete_imgbtn_clock, 0, LV_STATE_DEFAULT);

    //Write style for screen_selete_imgbtn_clock, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_text_color(ui->screen_selete_imgbtn_clock, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_selete_imgbtn_clock, &lv_font_ZiTiQuanWeiJunHeiW22_12, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_selete_imgbtn_clock, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_selete_imgbtn_clock, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_selete_imgbtn_clock, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style for screen_selete_imgbtn_clock, Part: LV_PART_MAIN, State: LV_STATE_PRESSED.
    lv_obj_set_style_image_recolor_opa(ui->screen_selete_imgbtn_clock, 0, LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_image_opa(ui->screen_selete_imgbtn_clock, 255, LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_text_color(ui->screen_selete_imgbtn_clock, lv_color_hex(0xFF33FF), LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_text_font(ui->screen_selete_imgbtn_clock, &lv_font_ZiTiQuanWeiJunHeiW22_12, LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_text_opa(ui->screen_selete_imgbtn_clock, 255, LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_shadow_width(ui->screen_selete_imgbtn_clock, 0, LV_PART_MAIN|LV_STATE_PRESSED);

    //Write style for screen_selete_imgbtn_clock, Part: LV_PART_MAIN, State: LV_STATE_CHECKED.
    lv_obj_set_style_image_recolor_opa(ui->screen_selete_imgbtn_clock, 0, LV_PART_MAIN|LV_STATE_CHECKED);
    lv_obj_set_style_image_opa(ui->screen_selete_imgbtn_clock, 255, LV_PART_MAIN|LV_STATE_CHECKED);
    lv_obj_set_style_text_color(ui->screen_selete_imgbtn_clock, lv_color_hex(0xFF33FF), LV_PART_MAIN|LV_STATE_CHECKED);
    lv_obj_set_style_text_font(ui->screen_selete_imgbtn_clock, &lv_font_ZiTiQuanWeiJunHeiW22_12, LV_PART_MAIN|LV_STATE_CHECKED);
    lv_obj_set_style_text_opa(ui->screen_selete_imgbtn_clock, 255, LV_PART_MAIN|LV_STATE_CHECKED);
    lv_obj_set_style_shadow_width(ui->screen_selete_imgbtn_clock, 0, LV_PART_MAIN|LV_STATE_CHECKED);

    //Write style for screen_selete_imgbtn_clock, Part: LV_PART_MAIN, State: LV_IMAGEBUTTON_STATE_RELEASED.
    lv_obj_set_style_image_recolor_opa(ui->screen_selete_imgbtn_clock, 0, LV_PART_MAIN|LV_IMAGEBUTTON_STATE_RELEASED);
    lv_obj_set_style_image_opa(ui->screen_selete_imgbtn_clock, 255, LV_PART_MAIN|LV_IMAGEBUTTON_STATE_RELEASED);

    //Write codes screen_selete_imgbtn_music
    ui->screen_selete_imgbtn_music = lv_imagebutton_create(ui->screen_selete);
    lv_obj_set_pos(ui->screen_selete_imgbtn_music, 25, 194);
    lv_obj_set_size(ui->screen_selete_imgbtn_music, 77, 66);
    lv_obj_add_flag(ui->screen_selete_imgbtn_music, LV_OBJ_FLAG_CHECKABLE);
    lv_imagebutton_set_src(ui->screen_selete_imgbtn_music, LV_IMAGEBUTTON_STATE_RELEASED, &_music_RGB565A8_77x66, NULL, NULL);
    lv_imagebutton_set_src(ui->screen_selete_imgbtn_music, LV_IMAGEBUTTON_STATE_PRESSED, &_musicpress_RGB565A8_77x66, NULL, NULL);
    lv_imagebutton_set_src(ui->screen_selete_imgbtn_music, LV_IMAGEBUTTON_STATE_CHECKED_RELEASED, &_music_RGB565A8_77x66, NULL, NULL);
    lv_imagebutton_set_src(ui->screen_selete_imgbtn_music, LV_IMAGEBUTTON_STATE_CHECKED_PRESSED, &_musicpress_RGB565A8_77x66, NULL, NULL);
    ui->screen_selete_imgbtn_music_label = lv_label_create(ui->screen_selete_imgbtn_music);
    lv_label_set_text(ui->screen_selete_imgbtn_music_label, "");
    lv_label_set_long_mode(ui->screen_selete_imgbtn_music_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(ui->screen_selete_imgbtn_music_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(ui->screen_selete_imgbtn_music, 0, LV_STATE_DEFAULT);

    //Write style for screen_selete_imgbtn_music, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_text_color(ui->screen_selete_imgbtn_music, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_selete_imgbtn_music, &lv_font_ZiTiQuanWeiJunHeiW22_12, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_selete_imgbtn_music, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_selete_imgbtn_music, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_selete_imgbtn_music, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style for screen_selete_imgbtn_music, Part: LV_PART_MAIN, State: LV_STATE_PRESSED.
    lv_obj_set_style_image_recolor_opa(ui->screen_selete_imgbtn_music, 0, LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_image_opa(ui->screen_selete_imgbtn_music, 255, LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_text_color(ui->screen_selete_imgbtn_music, lv_color_hex(0xFF33FF), LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_text_font(ui->screen_selete_imgbtn_music, &lv_font_ZiTiQuanWeiJunHeiW22_12, LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_text_opa(ui->screen_selete_imgbtn_music, 255, LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_shadow_width(ui->screen_selete_imgbtn_music, 0, LV_PART_MAIN|LV_STATE_PRESSED);

    //Write style for screen_selete_imgbtn_music, Part: LV_PART_MAIN, State: LV_STATE_CHECKED.
    lv_obj_set_style_image_recolor_opa(ui->screen_selete_imgbtn_music, 0, LV_PART_MAIN|LV_STATE_CHECKED);
    lv_obj_set_style_image_opa(ui->screen_selete_imgbtn_music, 255, LV_PART_MAIN|LV_STATE_CHECKED);
    lv_obj_set_style_text_color(ui->screen_selete_imgbtn_music, lv_color_hex(0xFF33FF), LV_PART_MAIN|LV_STATE_CHECKED);
    lv_obj_set_style_text_font(ui->screen_selete_imgbtn_music, &lv_font_ZiTiQuanWeiJunHeiW22_12, LV_PART_MAIN|LV_STATE_CHECKED);
    lv_obj_set_style_text_opa(ui->screen_selete_imgbtn_music, 255, LV_PART_MAIN|LV_STATE_CHECKED);
    lv_obj_set_style_shadow_width(ui->screen_selete_imgbtn_music, 0, LV_PART_MAIN|LV_STATE_CHECKED);

    //Write style for screen_selete_imgbtn_music, Part: LV_PART_MAIN, State: LV_IMAGEBUTTON_STATE_RELEASED.
    lv_obj_set_style_image_recolor_opa(ui->screen_selete_imgbtn_music, 0, LV_PART_MAIN|LV_IMAGEBUTTON_STATE_RELEASED);
    lv_obj_set_style_image_opa(ui->screen_selete_imgbtn_music, 255, LV_PART_MAIN|LV_IMAGEBUTTON_STATE_RELEASED);

    //The custom code of screen_selete.


    //Update current screen layout.
    lv_obj_update_layout(ui->screen_selete);

    //Init events for screen.
    events_init_screen_selete(ui);
}
