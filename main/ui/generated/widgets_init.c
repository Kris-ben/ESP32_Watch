/*
* Copyright 2026 NXP
* NXP Proprietary. This software is owned or controlled by NXP and may only be used strictly in
* accordance with the applicable license terms. By expressly accepting such terms or by downloading, installing,
* activating and/or otherwise using the software, you are agreeing that you have read, and that you agree to
* comply with and are bound by, such license terms.  If you do not agree to be bound by the applicable license
* terms, then you may not retain, install, activate or otherwise use the software.
*/

#include "lvgl.h"
#include "gui_guider.h"
#include "widgets_init.h"
#include <stdlib.h>
#include <string.h>
#include <time.h>


__attribute__((unused)) void kb_event_cb (lv_event_t *e) {
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t *kb = lv_event_get_target(e);
    if(code == LV_EVENT_READY || code == LV_EVENT_CANCEL) {
        lv_obj_add_flag(kb, LV_OBJ_FLAG_HIDDEN);
    }
}

__attribute__((unused)) void ta_event_cb (lv_event_t *e) {
#if LV_USE_KEYBOARD
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * ta = lv_event_get_target(e);
    lv_obj_t * kb = lv_event_get_user_data(e);

    /* 键盘挂在顶层(lv_layer_top)不会被删除，但它记住的输入框属于“每次切页面都会被删掉”
       的界面（screen_wifi_connect）。界面切走后 ta 已经是悬空指针，
       再对它调 lv_obj_remove_state/set_textarea 就是写已释放内存。 */
    if(ta == NULL || !lv_obj_is_valid(ta)) {
        if(kb) {
            lv_keyboard_set_textarea(kb, NULL);
            lv_obj_add_flag(kb, LV_OBJ_FLAG_HIDDEN);
        }
        return;
    }

    if(code == LV_EVENT_FOCUSED) {
        if(lv_indev_get_type(lv_indev_active()) != LV_INDEV_TYPE_KEYPAD) {
            lv_keyboard_set_textarea(kb, ta);
            lv_obj_remove_flag(kb, LV_OBJ_FLAG_HIDDEN);
        }
    } else if(code == LV_EVENT_READY) {
        lv_obj_add_flag(kb, LV_OBJ_FLAG_HIDDEN);
        lv_obj_remove_state(ta, LV_STATE_FOCUSED);
        lv_indev_reset(NULL, ta);
    } else if(code == LV_EVENT_DEFOCUSED) {
        lv_keyboard_set_textarea(kb, NULL);
        lv_obj_add_flag(kb, LV_OBJ_FLAG_HIDDEN);
    }
#endif
}

void clock_count(int *hour, int *min, int *sec)
{
    (*sec)++;
    if(*sec == 60)
    {
        *sec = 0;
        (*min)++;
    }
    if(*min == 60)
    {
        *min = 0;
        if(*hour < 12)
        {
            (*hour)++;
        } else {
            (*hour)++;
            *hour = *hour %12;
        }
    }
}

void digital_clock_count(int * hour, int * minute, int * seconds, char * meridiem)
{

    (*seconds)++;
    if(*seconds == 60) {
        *seconds = 0;
        (*minute)++;
    }
    if(*minute == 60) {
        *minute = 0;
        if(*hour < 12) {
            (*hour)++;
        }
        else {
            (*hour)++;
            (*hour) = (*hour) % 12;
        }
    }
    if(*hour == 12 && *seconds == 0 && *minute == 0) {
        if((lv_strcmp(meridiem, "PM") == 0)) {
            lv_strcpy(meridiem, "AM");
        }
        else {
            lv_strcpy(meridiem, "PM");
        }
    }
}


extern void set_home_time(lv_ui *ui, int year, int month, int day,
                          int w_day, int hour, int min, int sec);

void screen_home_digital_clock_timer(lv_timer_t *timer)
{
    (void)timer;
    time_t now = time(NULL);
    struct tm local_time;
    localtime_r(&now, &local_time);
    set_home_time(&guider_ui, local_time.tm_year + 1900,
                  local_time.tm_mon + 1, local_time.tm_mday, local_time.tm_wday,
                  local_time.tm_hour, local_time.tm_min, local_time.tm_sec);
}
void screen_Rli_calendar_1_draw_part_begin_event_cb(lv_event_t * e)
{
    lv_obj_t * obj = lv_event_get_target(e);
    lv_draw_task_t * draw_task = lv_event_get_param(e);
    lv_draw_dsc_base_t * base_dsc = (lv_draw_dsc_base_t *)lv_draw_task_get_draw_dsc(draw_task);
    if(base_dsc->part != LV_PART_ITEMS) return;

    lv_draw_label_dsc_t * label_dsc = lv_draw_task_get_label_dsc(draw_task);
    lv_draw_border_dsc_t * border_dsc = lv_draw_task_get_border_dsc(draw_task);
    lv_draw_fill_dsc_t * fill_dsc = lv_draw_task_get_fill_dsc(draw_task);

    if(base_dsc->id1 < 7) {
        if(label_dsc) label_dsc->color = lv_color_hex(0x0D3055);
        if(label_dsc) label_dsc->font = &lv_font_ZiTiQuanWeiJunHeiW22_12;
    } else if (lv_buttonmatrix_has_button_ctrl(obj, base_dsc->id1, LV_BUTTONMATRIX_CTRL_DISABLED)) {
        if(label_dsc) label_dsc->color = lv_color_hex(0xA9A2A2);
        if(label_dsc) label_dsc->font = &lv_font_ZiTiQuanWeiJunHeiW22_12;
        if(fill_dsc) fill_dsc->opa = 255;
        if(fill_dsc) fill_dsc->color = lv_color_hex(0xffffff);
    }
    if(lv_buttonmatrix_has_button_ctrl(obj, base_dsc->id1, LV_BUTTONMATRIX_CTRL_CUSTOM_1)) {
        if(label_dsc) label_dsc->color = lv_color_hex(0x0D3055);
        if(label_dsc) label_dsc->font = &lv_font_ZiTiQuanWeiJunHeiW22_18;
        if(fill_dsc) fill_dsc->opa = 255;
        if(fill_dsc) fill_dsc->color = lv_color_hex(0x01a2b1);
        if(border_dsc) border_dsc->opa = 255;
        if(border_dsc) border_dsc->width = 1;
        if(border_dsc) border_dsc->color = lv_color_hex(0xc0c0c0);
    }
    if(lv_buttonmatrix_has_button_ctrl(obj, base_dsc->id1, LV_BUTTONMATRIX_CTRL_CUSTOM_2)) {
        if(label_dsc) label_dsc->color = lv_color_hex(0x0D3055);
        if(label_dsc) label_dsc->font = &lv_font_ZiTiQuanWeiJunHeiW22_12;
        if(fill_dsc) fill_dsc->opa = 255;
        if(fill_dsc) fill_dsc->color = lv_color_hex(0x2195f6);
    } else {
    }
}

void screen_Rli_calendar_1_event_handler(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_VALUE_CHANGED) {
        lv_obj_t * obj = lv_event_get_current_target(e);
        lv_calendar_date_t date;
        /* lv_calendar_get_pressed_date 失败时不会写 date，必须先判返回值，
           否则会把未初始化的栈数据当日期用。 */
        if (lv_calendar_get_pressed_date(obj, &date) == LV_RESULT_OK) {
            /* LVGL 只保存指针，这块内存必须是静态的，不能是局部变量 */
            static lv_calendar_date_t highlighted[1];
            highlighted[0] = date;
            lv_calendar_set_highlighted_dates(obj, highlighted, 1);
        }
    }
}

extern int screen_AI_digital_clock_1_hour_value;
extern int screen_AI_digital_clock_1_min_value;
extern int screen_AI_digital_clock_1_sec_value;

void screen_AI_digital_clock_1_timer(lv_timer_t *timer)
{
    clock_count(&screen_AI_digital_clock_1_hour_value, &screen_AI_digital_clock_1_min_value, &screen_AI_digital_clock_1_sec_value);
    if (lv_obj_is_valid(guider_ui.screen_AI_digital_clock_1))
    {
        lv_label_set_text_fmt(guider_ui.screen_AI_digital_clock_1, "%d:%02d", screen_AI_digital_clock_1_hour_value, screen_AI_digital_clock_1_min_value);
    }
}

extern int screen_wifi_digital_clock_time_hour_value;
extern int screen_wifi_digital_clock_time_min_value;
extern int screen_wifi_digital_clock_time_sec_value;

void screen_wifi_digital_clock_time_timer(lv_timer_t *timer)
{
    clock_count(&screen_wifi_digital_clock_time_hour_value, &screen_wifi_digital_clock_time_min_value, &screen_wifi_digital_clock_time_sec_value);
    if (lv_obj_is_valid(guider_ui.screen_wifi_digital_clock_time))
    {
        lv_label_set_text_fmt(guider_ui.screen_wifi_digital_clock_time, "%d:%02d", screen_wifi_digital_clock_time_hour_value, screen_wifi_digital_clock_time_min_value);
    }
}
static lv_obj_t * screen_weather_datetext_1_calendar;

void screen_weather_datetext_1_event_handler(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * btn = lv_event_get_target(e);
    if(code == LV_EVENT_FOCUSED) {
        char * s = lv_label_get_text(btn);
        if(screen_weather_datetext_1_calendar == NULL) {
            screen_weather_datetext_1_init_calendar(btn, s);
        }
    }
}

void screen_weather_datetext_1_init_calendar(lv_obj_t *obj, char * s)
{
    if (screen_weather_datetext_1_calendar == NULL) {
        lv_obj_add_flag(lv_layer_top(), LV_OBJ_FLAG_CLICKABLE);
        screen_weather_datetext_1_calendar = lv_calendar_create(lv_layer_top());
        if (screen_weather_datetext_1_calendar == NULL) {
            /* 内存不足建不出来：把顶层点击放回去，别留下半截状态 */
            lv_obj_remove_flag(lv_layer_top(), LV_OBJ_FLAG_CLICKABLE);
            return;
        }
        lv_obj_t * scr = lv_obj_get_screen(obj);
        int32_t scr_height = lv_obj_get_height(scr);
        int32_t scr_width = lv_obj_get_width(scr);
        lv_obj_set_size(screen_weather_datetext_1_calendar, scr_width * 0.8, scr_height * 0.8);
        /* strtok 会就地修改字符串，不能拿 label 自己的文本去切（会把界面上的日期改坏）。
           天气接口给的是 "2026-09-23"（横杠），所以 '-' 和 '/' 都要能切；
           以前只按 '/' 切，切不出来时 month/day 是 NULL，后面的 atoi(NULL) 直接崩芯片。 */
        char date_buf[32] = {0};
        if (s) {
            strncpy(date_buf, s, sizeof(date_buf) - 1);
        }
        char * year = strtok(date_buf, "/-");
        char * month = strtok(NULL, "/-");
        char * day = strtok(NULL, "/-");

        int iy = 0, im = 0, id = 0;
        bool date_ok = (year != NULL && month != NULL && day != NULL);
        if (date_ok) {
            iy = atoi(year);
            im = atoi(month);
            id = atoi(day);
            if (iy < 1900 || iy > 2200 || im < 1 || im > 12 || id < 1 || id > 31) {
                date_ok = false;
            }
        }
        if (date_ok) {
            lv_calendar_set_showed_date(screen_weather_datetext_1_calendar, iy, im);
            /* LVGL 只保存指针，所以这块内存必须是静态的（原来是局部变量 = 悬空指针） */
            static lv_calendar_date_t highlighted_days[1];
            highlighted_days[0].year = iy;
            highlighted_days[0].month = im;
            highlighted_days[0].day = id;
            lv_calendar_set_highlighted_dates(screen_weather_datetext_1_calendar, highlighted_days, 1);
        }
        lv_obj_align(screen_weather_datetext_1_calendar,LV_ALIGN_CENTER, 0, 0);

        lv_obj_add_event_cb(screen_weather_datetext_1_calendar, screen_weather_datetext_1_calendar_event_handler, LV_EVENT_ALL,NULL);
        lv_calendar_header_arrow_create(screen_weather_datetext_1_calendar);
        lv_obj_update_layout(scr);
    }
}

void screen_weather_datetext_1_calendar_event_handler(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * obj = lv_event_get_current_target(e);

    if (code == LV_EVENT_VALUE_CHANGED) {
        lv_calendar_date_t date;
        if (lv_calendar_get_pressed_date(obj, &date) != LV_RESULT_OK) {
            return;
        }
        char buf[16];
        lv_snprintf(buf,sizeof(buf),"%d/%02d/%02d", date.year, date.month,date.day);
        /* 这个 label 属于 screen_weather，而 screen_weather 每次切页面都会被删掉，
           但日历挂在顶层不会被删 —— 所以这里必须先判有效，否则会写已释放的对象。 */
        if (guider_ui.screen_weather_datetext_1 &&
            lv_obj_is_valid(guider_ui.screen_weather_datetext_1)) {
            lv_label_set_text(guider_ui.screen_weather_datetext_1, buf);
        }
        lv_obj_remove_flag(lv_layer_top(), LV_OBJ_FLAG_CLICKABLE);
        lv_obj_set_style_bg_opa(lv_layer_top(), LV_OPA_TRANSP, 0);
        lv_obj_delete(screen_weather_datetext_1_calendar);
        screen_weather_datetext_1_calendar = NULL;
    }
}


extern int screen_musiclist_digital_clock_1_hour_value;
extern int screen_musiclist_digital_clock_1_min_value;
extern int screen_musiclist_digital_clock_1_sec_value;

void screen_musiclist_digital_clock_1_timer(lv_timer_t *timer)
{
    clock_count(&screen_musiclist_digital_clock_1_hour_value, &screen_musiclist_digital_clock_1_min_value, &screen_musiclist_digital_clock_1_sec_value);
    if (lv_obj_is_valid(guider_ui.screen_musiclist_digital_clock_1))
    {
        lv_label_set_text_fmt(guider_ui.screen_musiclist_digital_clock_1, "%d:%02d", screen_musiclist_digital_clock_1_hour_value, screen_musiclist_digital_clock_1_min_value);
    }
}

extern int screen_play_digital_clock_1_hour_value;
extern int screen_play_digital_clock_1_min_value;
extern int screen_play_digital_clock_1_sec_value;

void screen_play_digital_clock_1_timer(lv_timer_t *timer)
{
    clock_count(&screen_play_digital_clock_1_hour_value, &screen_play_digital_clock_1_min_value, &screen_play_digital_clock_1_sec_value);
    if (lv_obj_is_valid(guider_ui.screen_play_digital_clock_1))
    {
        lv_label_set_text_fmt(guider_ui.screen_play_digital_clock_1, "%d:%02d", screen_play_digital_clock_1_hour_value, screen_play_digital_clock_1_min_value);
    }
}
