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
#include "ai_status_page.h"



int screen_AI_digital_clock_1_min_value = 25;
int screen_AI_digital_clock_1_hour_value = 11;
int screen_AI_digital_clock_1_sec_value = 50;
void setup_scr_screen_AI(lv_ui *ui)
{
    //Write codes screen_AI
    ui->screen_AI = lv_obj_create(NULL);
    lv_obj_set_size(ui->screen_AI, 240, 284);
    lv_obj_set_scrollbar_mode(ui->screen_AI, LV_SCROLLBAR_MODE_OFF);

    //Write style for screen_AI, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_AI, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_AI, lv_color_hex(0xebebeb), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_AI, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_AI_btn_spreak
    ui->screen_AI_btn_spreak = lv_button_create(ui->screen_AI);
    lv_obj_set_pos(ui->screen_AI_btn_spreak, 12, 218);
    lv_obj_set_size(ui->screen_AI_btn_spreak, 214, 50);
    ui->screen_AI_btn_spreak_label = lv_label_create(ui->screen_AI_btn_spreak);
    lv_label_set_text(ui->screen_AI_btn_spreak_label, "按下说话");
    lv_label_set_long_mode(ui->screen_AI_btn_spreak_label, LV_LABEL_LONG_SCROLL);
    lv_obj_align(ui->screen_AI_btn_spreak_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(ui->screen_AI_btn_spreak, 0, LV_STATE_DEFAULT);
    lv_obj_set_width(ui->screen_AI_btn_spreak_label, LV_PCT(100));

    //Write style for screen_AI_btn_spreak, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_AI_btn_spreak, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_AI_btn_spreak, lv_color_hex(0x2195f6), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_AI_btn_spreak, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->screen_AI_btn_spreak, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_AI_btn_spreak, 5, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_AI_btn_spreak, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_AI_btn_spreak, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_AI_btn_spreak, &lv_font_ZiTiQuanWeiJunHeiW22_18, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_AI_btn_spreak, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_AI_btn_spreak, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_AI_label_ai
    ui->screen_AI_label_ai = lv_label_create(ui->screen_AI);
    lv_obj_set_pos(ui->screen_AI_label_ai, 72, 68);
    lv_obj_set_size(ui->screen_AI_label_ai, 155, 41);
    lv_label_set_text(ui->screen_AI_label_ai, "");
    lv_label_set_long_mode(ui->screen_AI_label_ai, LV_LABEL_LONG_WRAP);

    //Write style for screen_AI_label_ai, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_AI_label_ai, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_AI_label_ai, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_AI_label_ai, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_AI_label_ai, &lv_font_ZiTiQuanWeiJunHeiW22_12, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_AI_label_ai, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->screen_AI_label_ai, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->screen_AI_label_ai, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_AI_label_ai, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_AI_label_ai, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_AI_label_ai, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_AI_label_ai, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_AI_label_ai, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_AI_label_ai, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_AI_label_ai, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_AI_label_ai, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_AI_label_ai, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_AI_label_user
    ui->screen_AI_label_user = lv_label_create(ui->screen_AI);
    lv_obj_set_pos(ui->screen_AI_label_user, 18, 166);
    lv_obj_set_size(ui->screen_AI_label_user, 150, 38);
    lv_label_set_text(ui->screen_AI_label_user, "");
    lv_label_set_long_mode(ui->screen_AI_label_user, LV_LABEL_LONG_WRAP);

    //Write style for screen_AI_label_user, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_AI_label_user, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_AI_label_user, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_AI_label_user, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_AI_label_user, &lv_font_ZiTiQuanWeiJunHeiW22_12, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_AI_label_user, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->screen_AI_label_user, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->screen_AI_label_user, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_AI_label_user, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_AI_label_user, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_AI_label_user, lv_color_hex(0x1bd73b), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_AI_label_user, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_AI_label_user, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_AI_label_user, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_AI_label_user, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_AI_label_user, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_AI_label_user, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_AI_img_ai
    ui->screen_AI_img_ai = lv_image_create(ui->screen_AI);
    lv_obj_set_pos(ui->screen_AI_img_ai, 5, 65);
    lv_obj_set_size(ui->screen_AI_img_ai, 54, 46);
    lv_obj_add_flag(ui->screen_AI_img_ai, LV_OBJ_FLAG_CLICKABLE);
    lv_image_set_src(ui->screen_AI_img_ai, &_ai_label_RGB565A8_54x46);
    lv_image_set_pivot(ui->screen_AI_img_ai, 50,50);
    lv_image_set_rotation(ui->screen_AI_img_ai, 0);

    //Write style for screen_AI_img_ai, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_image_recolor_opa(ui->screen_AI_img_ai, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_image_opa(ui->screen_AI_img_ai, 255, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_AI_img_user
    ui->screen_AI_img_user = lv_image_create(ui->screen_AI);
    lv_obj_set_pos(ui->screen_AI_img_user, 179, 163);
    lv_obj_set_size(ui->screen_AI_img_user, 54, 46);
    lv_obj_add_flag(ui->screen_AI_img_user, LV_OBJ_FLAG_CLICKABLE);
    lv_image_set_src(ui->screen_AI_img_user, &_User_RGB565A8_54x46);
    lv_image_set_pivot(ui->screen_AI_img_user, 50,50);
    lv_image_set_rotation(ui->screen_AI_img_user, 0);

    //Write style for screen_AI_img_user, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_image_recolor_opa(ui->screen_AI_img_user, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_image_opa(ui->screen_AI_img_user, 255, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_AI_label_title
    ui->screen_AI_label_title = lv_label_create(ui->screen_AI);
    lv_obj_set_pos(ui->screen_AI_label_title, 28, 18);
    lv_obj_set_size(ui->screen_AI_label_title, 211, 30);
    lv_label_set_text(ui->screen_AI_label_title, "AI聊天");
    lv_label_set_long_mode(ui->screen_AI_label_title, LV_LABEL_LONG_SCROLL_CIRCULAR);

    //Write style for screen_AI_label_title, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_AI_label_title, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_AI_label_title, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_AI_label_title, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_AI_label_title, &lv_font_ZiTiQuanWeiJunHeiW22_24, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_AI_label_title, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->screen_AI_label_title, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->screen_AI_label_title, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_AI_label_title, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_AI_label_title, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_AI_label_title, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_AI_label_title, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_AI_label_title, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_AI_label_title, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_AI_label_title, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_AI_label_title, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_AI_label_title, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_AI_imgbtn_return
    ui->screen_AI_imgbtn_return = lv_imagebutton_create(ui->screen_AI);
    lv_obj_set_pos(ui->screen_AI_imgbtn_return, 1, 18);
    lv_obj_set_size(ui->screen_AI_imgbtn_return, 27, 30);
    lv_obj_add_flag(ui->screen_AI_imgbtn_return, LV_OBJ_FLAG_CHECKABLE);
    lv_imagebutton_set_src(ui->screen_AI_imgbtn_return, LV_IMAGEBUTTON_STATE_RELEASED, &_return_RGB565A8_27x30, NULL, NULL);
    lv_imagebutton_set_src(ui->screen_AI_imgbtn_return, LV_IMAGEBUTTON_STATE_PRESSED, &_return_press_RGB565A8_27x30, NULL, NULL);
    lv_imagebutton_set_src(ui->screen_AI_imgbtn_return, LV_IMAGEBUTTON_STATE_CHECKED_RELEASED, &_return_RGB565A8_27x30, NULL, NULL);
    lv_imagebutton_set_src(ui->screen_AI_imgbtn_return, LV_IMAGEBUTTON_STATE_CHECKED_PRESSED, &_return_press_RGB565A8_27x30, NULL, NULL);
    ui->screen_AI_imgbtn_return_label = lv_label_create(ui->screen_AI_imgbtn_return);
    lv_label_set_text(ui->screen_AI_imgbtn_return_label, "");
    lv_label_set_long_mode(ui->screen_AI_imgbtn_return_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(ui->screen_AI_imgbtn_return_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(ui->screen_AI_imgbtn_return, 0, LV_STATE_DEFAULT);

    //Write style for screen_AI_imgbtn_return, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_text_color(ui->screen_AI_imgbtn_return, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_AI_imgbtn_return, &lv_font_ZiTiQuanWeiJunHeiW22_12, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_AI_imgbtn_return, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_AI_imgbtn_return, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_AI_imgbtn_return, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style for screen_AI_imgbtn_return, Part: LV_PART_MAIN, State: LV_STATE_PRESSED.
    lv_obj_set_style_image_recolor_opa(ui->screen_AI_imgbtn_return, 0, LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_image_opa(ui->screen_AI_imgbtn_return, 255, LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_text_color(ui->screen_AI_imgbtn_return, lv_color_hex(0xFF33FF), LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_text_font(ui->screen_AI_imgbtn_return, &lv_font_ZiTiQuanWeiJunHeiW22_12, LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_text_opa(ui->screen_AI_imgbtn_return, 255, LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_style_shadow_width(ui->screen_AI_imgbtn_return, 0, LV_PART_MAIN|LV_STATE_PRESSED);

    //Write style for screen_AI_imgbtn_return, Part: LV_PART_MAIN, State: LV_STATE_CHECKED.
    lv_obj_set_style_image_recolor_opa(ui->screen_AI_imgbtn_return, 0, LV_PART_MAIN|LV_STATE_CHECKED);
    lv_obj_set_style_image_opa(ui->screen_AI_imgbtn_return, 255, LV_PART_MAIN|LV_STATE_CHECKED);
    lv_obj_set_style_text_color(ui->screen_AI_imgbtn_return, lv_color_hex(0xFF33FF), LV_PART_MAIN|LV_STATE_CHECKED);
    lv_obj_set_style_text_font(ui->screen_AI_imgbtn_return, &lv_font_ZiTiQuanWeiJunHeiW22_12, LV_PART_MAIN|LV_STATE_CHECKED);
    lv_obj_set_style_text_opa(ui->screen_AI_imgbtn_return, 255, LV_PART_MAIN|LV_STATE_CHECKED);
    lv_obj_set_style_shadow_width(ui->screen_AI_imgbtn_return, 0, LV_PART_MAIN|LV_STATE_CHECKED);

    //Write style for screen_AI_imgbtn_return, Part: LV_PART_MAIN, State: LV_IMAGEBUTTON_STATE_RELEASED.
    lv_obj_set_style_image_recolor_opa(ui->screen_AI_imgbtn_return, 0, LV_PART_MAIN|LV_IMAGEBUTTON_STATE_RELEASED);
    lv_obj_set_style_image_opa(ui->screen_AI_imgbtn_return, 255, LV_PART_MAIN|LV_IMAGEBUTTON_STATE_RELEASED);

    //Write codes screen_AI_cont_1
    ui->screen_AI_cont_1 = lv_obj_create(ui->screen_AI);
    lv_obj_set_pos(ui->screen_AI_cont_1, -2, -11);
    lv_obj_set_size(ui->screen_AI_cont_1, 244, 28);
    lv_obj_set_scrollbar_mode(ui->screen_AI_cont_1, LV_SCROLLBAR_MODE_OFF);

    //Write style for screen_AI_cont_1, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_AI_cont_1, 2, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(ui->screen_AI_cont_1, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(ui->screen_AI_cont_1, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_side(ui->screen_AI_cont_1, LV_BORDER_SIDE_FULL, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_AI_cont_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_AI_cont_1, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_AI_cont_1, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_AI_cont_1, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_AI_cont_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_AI_cont_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_AI_cont_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_AI_cont_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_AI_cont_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_AI_list_battery
    ui->screen_AI_list_battery = lv_list_create(ui->screen_AI);
    lv_obj_set_pos(ui->screen_AI_list_battery, 161, -10);
    lv_obj_set_size(ui->screen_AI_list_battery, 64, 25);
    lv_obj_set_scrollbar_mode(ui->screen_AI_list_battery, LV_SCROLLBAR_MODE_OFF);
    ui->screen_AI_list_battery_item0 = lv_list_add_button(ui->screen_AI_list_battery, NULL, "--");

    //Write style state: LV_STATE_DEFAULT for &style_screen_AI_list_battery_main_main_default
    static lv_style_t style_screen_AI_list_battery_main_main_default;
    ui_init_style(&style_screen_AI_list_battery_main_main_default);

    lv_style_set_pad_top(&style_screen_AI_list_battery_main_main_default, 5);
    lv_style_set_pad_left(&style_screen_AI_list_battery_main_main_default, 5);
    lv_style_set_pad_right(&style_screen_AI_list_battery_main_main_default, 5);
    lv_style_set_pad_bottom(&style_screen_AI_list_battery_main_main_default, 5);
    lv_style_set_bg_opa(&style_screen_AI_list_battery_main_main_default, 255);
    lv_style_set_bg_color(&style_screen_AI_list_battery_main_main_default, lv_color_hex(0x050505));
    lv_style_set_bg_grad_dir(&style_screen_AI_list_battery_main_main_default, LV_GRAD_DIR_NONE);
    lv_style_set_border_width(&style_screen_AI_list_battery_main_main_default, 1);
    lv_style_set_border_opa(&style_screen_AI_list_battery_main_main_default, 255);
    lv_style_set_border_color(&style_screen_AI_list_battery_main_main_default, lv_color_hex(0x000000));
    lv_style_set_border_side(&style_screen_AI_list_battery_main_main_default, LV_BORDER_SIDE_FULL);
    lv_style_set_radius(&style_screen_AI_list_battery_main_main_default, 3);
    lv_style_set_shadow_width(&style_screen_AI_list_battery_main_main_default, 0);
    lv_obj_add_style(ui->screen_AI_list_battery, &style_screen_AI_list_battery_main_main_default, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style state: LV_STATE_DEFAULT for &style_screen_AI_list_battery_main_scrollbar_default
    static lv_style_t style_screen_AI_list_battery_main_scrollbar_default;
    ui_init_style(&style_screen_AI_list_battery_main_scrollbar_default);

    lv_style_set_radius(&style_screen_AI_list_battery_main_scrollbar_default, 3);
    lv_style_set_bg_opa(&style_screen_AI_list_battery_main_scrollbar_default, 255);
    lv_style_set_bg_color(&style_screen_AI_list_battery_main_scrollbar_default, lv_color_hex(0x000000));
    lv_style_set_bg_grad_dir(&style_screen_AI_list_battery_main_scrollbar_default, LV_GRAD_DIR_NONE);
    lv_obj_add_style(ui->screen_AI_list_battery, &style_screen_AI_list_battery_main_scrollbar_default, LV_PART_SCROLLBAR|LV_STATE_DEFAULT);

    //Write style state: LV_STATE_DEFAULT for &style_screen_AI_list_battery_extra_btns_main_default
    static lv_style_t style_screen_AI_list_battery_extra_btns_main_default;
    ui_init_style(&style_screen_AI_list_battery_extra_btns_main_default);

    lv_style_set_pad_top(&style_screen_AI_list_battery_extra_btns_main_default, 5);
    lv_style_set_pad_left(&style_screen_AI_list_battery_extra_btns_main_default, 5);
    lv_style_set_pad_right(&style_screen_AI_list_battery_extra_btns_main_default, 5);
    lv_style_set_pad_bottom(&style_screen_AI_list_battery_extra_btns_main_default, 5);
    lv_style_set_border_width(&style_screen_AI_list_battery_extra_btns_main_default, 0);
    lv_style_set_text_color(&style_screen_AI_list_battery_extra_btns_main_default, lv_color_hex(0xfbfbfb));
    lv_style_set_text_font(&style_screen_AI_list_battery_extra_btns_main_default, &lv_font_ZiTiQuanWeiJunHeiW22_12);
    lv_style_set_text_opa(&style_screen_AI_list_battery_extra_btns_main_default, 255);
    lv_style_set_radius(&style_screen_AI_list_battery_extra_btns_main_default, 3);
    lv_style_set_bg_opa(&style_screen_AI_list_battery_extra_btns_main_default, 255);
    lv_style_set_bg_color(&style_screen_AI_list_battery_extra_btns_main_default, lv_color_hex(0x000000));
    lv_style_set_bg_grad_dir(&style_screen_AI_list_battery_extra_btns_main_default, LV_GRAD_DIR_NONE);
    lv_obj_add_style(ui->screen_AI_list_battery_item0, &style_screen_AI_list_battery_extra_btns_main_default, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style state: LV_STATE_DEFAULT for &style_screen_AI_list_battery_extra_texts_main_default
    static lv_style_t style_screen_AI_list_battery_extra_texts_main_default;
    ui_init_style(&style_screen_AI_list_battery_extra_texts_main_default);

    lv_style_set_pad_top(&style_screen_AI_list_battery_extra_texts_main_default, 5);
    lv_style_set_pad_left(&style_screen_AI_list_battery_extra_texts_main_default, 5);
    lv_style_set_pad_right(&style_screen_AI_list_battery_extra_texts_main_default, 5);
    lv_style_set_pad_bottom(&style_screen_AI_list_battery_extra_texts_main_default, 5);
    lv_style_set_border_width(&style_screen_AI_list_battery_extra_texts_main_default, 0);
    lv_style_set_text_color(&style_screen_AI_list_battery_extra_texts_main_default, lv_color_hex(0xf9f9f9));
    lv_style_set_text_font(&style_screen_AI_list_battery_extra_texts_main_default, &lv_font_ZiTiQuanWeiJunHeiW22_12);
    lv_style_set_text_opa(&style_screen_AI_list_battery_extra_texts_main_default, 255);
    lv_style_set_radius(&style_screen_AI_list_battery_extra_texts_main_default, 3);
    lv_style_set_transform_width(&style_screen_AI_list_battery_extra_texts_main_default, 0);
    lv_style_set_bg_opa(&style_screen_AI_list_battery_extra_texts_main_default, 255);
    lv_style_set_bg_color(&style_screen_AI_list_battery_extra_texts_main_default, lv_color_hex(0xffffff));
    lv_style_set_bg_grad_dir(&style_screen_AI_list_battery_extra_texts_main_default, LV_GRAD_DIR_NONE);

    //Write codes screen_AI_img_wifi
    ui->screen_AI_img_wifi = lv_image_create(ui->screen_AI);
    lv_obj_set_pos(ui->screen_AI_img_wifi, 138, -4);
    lv_obj_set_size(ui->screen_AI_img_wifi, 20, 20);
    lv_obj_add_flag(ui->screen_AI_img_wifi, LV_OBJ_FLAG_CLICKABLE);
    lv_image_set_src(ui->screen_AI_img_wifi, &_wifi_close_RGB565A8_20x20);
    lv_image_set_pivot(ui->screen_AI_img_wifi, 50,50);
    lv_image_set_rotation(ui->screen_AI_img_wifi, 0);

    //Write style for screen_AI_img_wifi, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_image_recolor_opa(ui->screen_AI_img_wifi, 26, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_image_recolor(ui->screen_AI_img_wifi, lv_color_hex(0x090000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_image_opa(ui->screen_AI_img_wifi, 255, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_AI_digital_clock_1
    static bool screen_AI_digital_clock_1_timer_enabled = false;
    ui->screen_AI_digital_clock_1 = lv_label_create(ui->screen_AI);
    lv_obj_set_pos(ui->screen_AI_digital_clock_1, 5, -7);
    lv_obj_set_size(ui->screen_AI_digital_clock_1, 53, 22);
    lv_label_set_text(ui->screen_AI_digital_clock_1, "11:25");
    if (!screen_AI_digital_clock_1_timer_enabled) {
        lv_timer_create(screen_AI_digital_clock_1_timer, 1000, NULL);
        screen_AI_digital_clock_1_timer_enabled = true;
    }

    //Write style for screen_AI_digital_clock_1, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_radius(ui->screen_AI_digital_clock_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_AI_digital_clock_1, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_AI_digital_clock_1, &lv_font_ZiTiQuanWeiJunHeiW22_18, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_AI_digital_clock_1, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->screen_AI_digital_clock_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_AI_digital_clock_1, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_AI_digital_clock_1, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_AI_digital_clock_1, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_AI_digital_clock_1, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_AI_digital_clock_1, 7, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_AI_digital_clock_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_AI_digital_clock_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_AI_digital_clock_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_AI_digital_clock_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //The custom code of screen_AI.
    lv_obj_null_on_delete(&ui->screen_AI_digital_clock_1);
    ai_status_page_configure(ui);

    //Update current screen layout.
    lv_obj_update_layout(ui->screen_AI);

    //Init events for screen.
    events_init_screen_AI(ui);
}
