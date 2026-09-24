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



lv_calendar_date_t screen_Rli_calendar_1_today;
lv_calendar_date_t screen_Rli_calendar_1_highlihted_days[1];
void setup_scr_screen_Rli(lv_ui *ui)
{
    //Write codes screen_Rli
    ui->screen_Rli = lv_obj_create(NULL);
    lv_obj_set_size(ui->screen_Rli, 240, 284);
    lv_obj_set_scrollbar_mode(ui->screen_Rli, LV_SCROLLBAR_MODE_OFF);

    //Write style for screen_Rli, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_Rli, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_Rli, lv_color_hex(0x020202), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_Rli, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_Rli_win_1
    ui->screen_Rli_win_1 = lv_win_create(ui->screen_Rli);
    lv_obj_set_pos(ui->screen_Rli_win_1, -685, 346);
    lv_obj_set_size(ui->screen_Rli_win_1, 244, 285);
    lv_obj_set_scrollbar_mode(ui->screen_Rli_win_1, LV_SCROLLBAR_MODE_OFF);
    lv_obj_t * screen_Rli_win_1_title = lv_win_add_title(ui->screen_Rli_win_1, "title");
    lv_obj_t * screen_Rli_win_1_header = lv_win_get_header(ui->screen_Rli_win_1);
    lv_obj_set_height(screen_Rli_win_1_header, 40);
    ui->screen_Rli_win_1_item0 = lv_win_add_button(ui->screen_Rli_win_1, LV_SYMBOL_CLOSE, 40);
    lv_obj_t *screen_Rli_win_1_label = lv_label_create(lv_win_get_content(ui->screen_Rli_win_1));
    lv_obj_set_scrollbar_mode(lv_win_get_content(ui->screen_Rli_win_1), LV_SCROLLBAR_MODE_OFF);
    lv_label_set_text(screen_Rli_win_1_label, "this is a \nlong text \nto show \nscrollbar. \nif \nit \nis not \nlong enough, \nadd more content");

    //Write style for screen_Rli_win_1, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_Rli_win_1, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_Rli_win_1, lv_color_hex(0xeeeef6), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_Rli_win_1, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_outline_width(ui->screen_Rli_win_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_Rli_win_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style state: LV_STATE_DEFAULT for &style_screen_Rli_win_1_extra_content_main_default
    static lv_style_t style_screen_Rli_win_1_extra_content_main_default;
    ui_init_style(&style_screen_Rli_win_1_extra_content_main_default);

    lv_style_set_bg_opa(&style_screen_Rli_win_1_extra_content_main_default, 255);
    lv_style_set_bg_color(&style_screen_Rli_win_1_extra_content_main_default, lv_color_hex(0xeeeef6));
    lv_style_set_bg_grad_dir(&style_screen_Rli_win_1_extra_content_main_default, LV_GRAD_DIR_NONE);
    lv_style_set_text_color(&style_screen_Rli_win_1_extra_content_main_default, lv_color_hex(0x393c41));
    lv_style_set_text_font(&style_screen_Rli_win_1_extra_content_main_default, &lv_font_ZiTiQuanWeiJunHeiW22_12);
    lv_style_set_text_opa(&style_screen_Rli_win_1_extra_content_main_default, 255);
    lv_style_set_text_letter_space(&style_screen_Rli_win_1_extra_content_main_default, 0);
    lv_style_set_text_line_space(&style_screen_Rli_win_1_extra_content_main_default, 2);
    lv_obj_add_style(lv_win_get_content(ui->screen_Rli_win_1), &style_screen_Rli_win_1_extra_content_main_default, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style state: LV_STATE_DEFAULT for &style_screen_Rli_win_1_extra_header_main_default
    static lv_style_t style_screen_Rli_win_1_extra_header_main_default;
    ui_init_style(&style_screen_Rli_win_1_extra_header_main_default);

    lv_style_set_bg_opa(&style_screen_Rli_win_1_extra_header_main_default, 255);
    lv_style_set_bg_color(&style_screen_Rli_win_1_extra_header_main_default, lv_color_hex(0xe6e6e6));
    lv_style_set_bg_grad_dir(&style_screen_Rli_win_1_extra_header_main_default, LV_GRAD_DIR_NONE);
    lv_style_set_text_color(&style_screen_Rli_win_1_extra_header_main_default, lv_color_hex(0x393c41));
    lv_style_set_text_font(&style_screen_Rli_win_1_extra_header_main_default, &lv_font_ZiTiQuanWeiJunHeiW22_12);
    lv_style_set_text_opa(&style_screen_Rli_win_1_extra_header_main_default, 255);
    lv_style_set_text_letter_space(&style_screen_Rli_win_1_extra_header_main_default, 0);
    lv_style_set_text_line_space(&style_screen_Rli_win_1_extra_header_main_default, 2);
    lv_style_set_pad_top(&style_screen_Rli_win_1_extra_header_main_default, 5);
    lv_style_set_pad_right(&style_screen_Rli_win_1_extra_header_main_default, 5);
    lv_style_set_pad_bottom(&style_screen_Rli_win_1_extra_header_main_default, 5);
    lv_style_set_pad_left(&style_screen_Rli_win_1_extra_header_main_default, 5);
    lv_style_set_pad_column(&style_screen_Rli_win_1_extra_header_main_default, 5);
    lv_obj_add_style(lv_win_get_header(ui->screen_Rli_win_1), &style_screen_Rli_win_1_extra_header_main_default, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style state: LV_STATE_DEFAULT for &style_screen_Rli_win_1_extra_btns_main_default
    static lv_style_t style_screen_Rli_win_1_extra_btns_main_default;
    ui_init_style(&style_screen_Rli_win_1_extra_btns_main_default);

    lv_style_set_radius(&style_screen_Rli_win_1_extra_btns_main_default, 8);
    lv_style_set_border_width(&style_screen_Rli_win_1_extra_btns_main_default, 0);
    lv_style_set_bg_opa(&style_screen_Rli_win_1_extra_btns_main_default, 255);
    lv_style_set_bg_color(&style_screen_Rli_win_1_extra_btns_main_default, lv_color_hex(0x2195f6));
    lv_style_set_bg_grad_dir(&style_screen_Rli_win_1_extra_btns_main_default, LV_GRAD_DIR_NONE);
    lv_style_set_shadow_width(&style_screen_Rli_win_1_extra_btns_main_default, 0);
    lv_obj_add_style(ui->screen_Rli_win_1_item0, &style_screen_Rli_win_1_extra_btns_main_default, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_Rli_calendar_1
    ui->screen_Rli_calendar_1 = lv_calendar_create(ui->screen_Rli);
    lv_obj_set_pos(ui->screen_Rli_calendar_1, 0, -1);
    lv_obj_set_size(ui->screen_Rli_calendar_1, 241, 215);
    screen_Rli_calendar_1_today.year = 2026;
    screen_Rli_calendar_1_today.month = 2;
    screen_Rli_calendar_1_today.day = 12;
    lv_calendar_set_today_date(ui->screen_Rli_calendar_1, screen_Rli_calendar_1_today.year, screen_Rli_calendar_1_today.month, screen_Rli_calendar_1_today.day);
    lv_calendar_set_showed_date(ui->screen_Rli_calendar_1, screen_Rli_calendar_1_today.year, screen_Rli_calendar_1_today.month);
    screen_Rli_calendar_1_highlihted_days[0].year = 2026;
    screen_Rli_calendar_1_highlihted_days[0].month = 2;
    screen_Rli_calendar_1_highlihted_days[0].day = 13;
    lv_calendar_set_highlighted_dates(ui->screen_Rli_calendar_1, screen_Rli_calendar_1_highlihted_days, 1);
    lv_obj_t *screen_Rli_calendar_1_header = lv_calendar_header_arrow_create(ui->screen_Rli_calendar_1);
    lv_obj_t *screen_Rli_calendar_1_btnm = lv_calendar_get_btnmatrix(ui->screen_Rli_calendar_1);
    lv_obj_add_event_cb(screen_Rli_calendar_1_btnm, screen_Rli_calendar_1_draw_part_begin_event_cb, LV_EVENT_DRAW_TASK_ADDED, NULL);
    lv_obj_add_event_cb(ui->screen_Rli_calendar_1, screen_Rli_calendar_1_event_handler, LV_EVENT_ALL, NULL);

    //Write style state: LV_STATE_DEFAULT for &style_screen_Rli_calendar_1_main_main_default
    static lv_style_t style_screen_Rli_calendar_1_main_main_default;
    ui_init_style(&style_screen_Rli_calendar_1_main_main_default);

    lv_style_set_border_width(&style_screen_Rli_calendar_1_main_main_default, 1);
    lv_style_set_border_opa(&style_screen_Rli_calendar_1_main_main_default, 255);
    lv_style_set_border_color(&style_screen_Rli_calendar_1_main_main_default, lv_color_hex(0xc0c0c0));
    lv_style_set_border_side(&style_screen_Rli_calendar_1_main_main_default, LV_BORDER_SIDE_FULL);
    lv_style_set_bg_opa(&style_screen_Rli_calendar_1_main_main_default, 255);
    lv_style_set_bg_color(&style_screen_Rli_calendar_1_main_main_default, lv_color_hex(0xffffff));
    lv_style_set_bg_grad_dir(&style_screen_Rli_calendar_1_main_main_default, LV_GRAD_DIR_NONE);
    lv_style_set_shadow_width(&style_screen_Rli_calendar_1_main_main_default, 0);
    lv_style_set_radius(&style_screen_Rli_calendar_1_main_main_default, 0);
    lv_obj_add_style(ui->screen_Rli_calendar_1, &style_screen_Rli_calendar_1_main_main_default, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style state: LV_STATE_DEFAULT for &style_screen_Rli_calendar_1_extra_header_main_default
    static lv_style_t style_screen_Rli_calendar_1_extra_header_main_default;
    ui_init_style(&style_screen_Rli_calendar_1_extra_header_main_default);

    lv_style_set_text_color(&style_screen_Rli_calendar_1_extra_header_main_default, lv_color_hex(0xffffff));
    lv_style_set_text_font(&style_screen_Rli_calendar_1_extra_header_main_default, &lv_font_ZiTiQuanWeiJunHeiW22_18);
    lv_style_set_text_opa(&style_screen_Rli_calendar_1_extra_header_main_default, 255);
    lv_style_set_bg_opa(&style_screen_Rli_calendar_1_extra_header_main_default, 255);
    lv_style_set_bg_color(&style_screen_Rli_calendar_1_extra_header_main_default, lv_color_hex(0x2195f6));
    lv_style_set_bg_grad_dir(&style_screen_Rli_calendar_1_extra_header_main_default, LV_GRAD_DIR_NONE);
    lv_obj_add_style(screen_Rli_calendar_1_header, &style_screen_Rli_calendar_1_extra_header_main_default, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style state: LV_STATE_DEFAULT for &style_screen_Rli_calendar_1_main_items_default
    static lv_style_t style_screen_Rli_calendar_1_main_items_default;
    ui_init_style(&style_screen_Rli_calendar_1_main_items_default);

    lv_style_set_bg_opa(&style_screen_Rli_calendar_1_main_items_default, 255);
    lv_style_set_bg_color(&style_screen_Rli_calendar_1_main_items_default, lv_color_hex(0xffffff));
    lv_style_set_bg_grad_dir(&style_screen_Rli_calendar_1_main_items_default, LV_GRAD_DIR_NONE);
    lv_style_set_border_width(&style_screen_Rli_calendar_1_main_items_default, 1);
    lv_style_set_border_opa(&style_screen_Rli_calendar_1_main_items_default, 255);
    lv_style_set_border_color(&style_screen_Rli_calendar_1_main_items_default, lv_color_hex(0xc0c0c0));
    lv_style_set_border_side(&style_screen_Rli_calendar_1_main_items_default, LV_BORDER_SIDE_FULL);
    lv_style_set_text_color(&style_screen_Rli_calendar_1_main_items_default, lv_color_hex(0x0D3055));
    lv_style_set_text_font(&style_screen_Rli_calendar_1_main_items_default, &lv_font_ZiTiQuanWeiJunHeiW22_12);
    lv_style_set_text_opa(&style_screen_Rli_calendar_1_main_items_default, 255);
    lv_obj_add_style(lv_calendar_get_btnmatrix(ui->screen_Rli_calendar_1), &style_screen_Rli_calendar_1_main_items_default, LV_PART_ITEMS|LV_STATE_DEFAULT);

    //Write codes screen_Rli_btn_return
    ui->screen_Rli_btn_return = lv_button_create(ui->screen_Rli);
    lv_obj_set_pos(ui->screen_Rli_btn_return, 79, 230);
    lv_obj_set_size(ui->screen_Rli_btn_return, 76, 29);
    ui->screen_Rli_btn_return_label = lv_label_create(ui->screen_Rli_btn_return);
    lv_label_set_text(ui->screen_Rli_btn_return_label, "返回");
    lv_label_set_long_mode(ui->screen_Rli_btn_return_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(ui->screen_Rli_btn_return_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(ui->screen_Rli_btn_return, 0, LV_STATE_DEFAULT);
    lv_obj_set_width(ui->screen_Rli_btn_return_label, LV_PCT(100));

    //Write style for screen_Rli_btn_return, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_Rli_btn_return, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_Rli_btn_return, lv_color_hex(0x2195f6), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_Rli_btn_return, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->screen_Rli_btn_return, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_Rli_btn_return, 5, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_Rli_btn_return, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_Rli_btn_return, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_Rli_btn_return, &lv_font_ZiTiQuanWeiJunHeiW22_18, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_Rli_btn_return, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_Rli_btn_return, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);

    //The custom code of screen_Rli.


    //Update current screen layout.
    lv_obj_update_layout(ui->screen_Rli);

    //Init events for screen.
    events_init_screen_Rli(ui);
}
