#include "alarm_list_page.h"

#include <stdint.h>
#include <stdio.h>

#include "alarm_clock.h"
#include "guider_customer_fonts.h"

static lv_obj_t *s_screen;
static lv_obj_t *s_time_labels[ALARM_COUNT];
static lv_obj_t *s_toggles[ALARM_COUNT];

static void alarm_screen_deleted(lv_event_t *event)
{
    if (lv_event_get_target_obj(event) != s_screen) return;
    s_screen = NULL;
    for (int i = 0; i < ALARM_COUNT; ++i) {
        s_time_labels[i] = NULL;
        s_toggles[i] = NULL;
    }
}

void alarm_list_page_refresh(lv_ui *ui)
{
    if (!ui || s_screen != ui->screen_clock || !lv_obj_is_valid(s_screen)) return;
    for (int i = 0; i < ALARM_COUNT; ++i) {
        uint8_t hour = 0;
        uint8_t minute = 0;
        if (alarm_get_time(i, &hour, &minute) != ESP_OK) continue;
        // 语音任务可能在页面切换动画期间触发刷新；旧页面对象已经失效时必须跳过。
        if (s_time_labels[i] && lv_obj_is_valid(s_time_labels[i])) {
            lv_label_set_text_fmt(s_time_labels[i], "%02u:%02u", hour, minute);
        }
        if (!s_toggles[i] || !lv_obj_is_valid(s_toggles[i])) continue;
        if (alarm_is_enabled(i)) lv_obj_add_state(s_toggles[i], LV_STATE_CHECKED);
        else lv_obj_remove_state(s_toggles[i], LV_STATE_CHECKED);
    }
}

static void edit_alarm_cb(lv_event_t *event)
{
    int index = (int)(intptr_t)lv_event_get_user_data(event);
    alarm_set_editing_index(index);
    ui_load_scr_animation(&guider_ui, &guider_ui.screen_set_clock,
                          guider_ui.screen_set_clock_del, &guider_ui.screen_clock_del,
                          setup_scr_screen_set_clock, LV_SCR_LOAD_ANIM_NONE,
                          0, 0, true, true);
}

static void toggle_alarm_cb(lv_event_t *event)
{
    int index = (int)(intptr_t)lv_event_get_user_data(event);
    lv_obj_t *toggle = lv_event_get_current_target_obj(event);
    alarm_set_enabled(index, lv_obj_has_state(toggle, LV_STATE_CHECKED));
}

static void make_alarm_row(lv_obj_t *screen, int index)
{
    const int row_y = 47 + index * 57;
    uint8_t hour = 0;
    uint8_t minute = 0;
    alarm_get_time(index, &hour, &minute);

    lv_obj_t *row = lv_obj_create(screen);
    lv_obj_set_pos(row, 8, row_y);
    lv_obj_set_size(row, 224, 51);
    lv_obj_set_style_radius(row, 10, 0);
    lv_obj_set_style_bg_color(row, lv_color_hex(0x26364d), 0);
    lv_obj_set_style_border_width(row, 0, 0);
    lv_obj_set_style_pad_all(row, 0, 0);
    lv_obj_remove_flag(row, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_t *time_area = lv_button_create(row);
    lv_obj_set_pos(time_area, 0, 0);
    lv_obj_set_size(time_area, 148, 51);
    lv_obj_set_style_bg_opa(time_area, LV_OPA_TRANSP, 0);
    lv_obj_set_style_shadow_width(time_area, 0, 0);
    lv_obj_set_style_radius(time_area, 0, 0);
    lv_obj_set_style_pad_all(time_area, 0, 0);
    lv_obj_add_event_cb(time_area, edit_alarm_cb, LV_EVENT_CLICKED,
                        (void *)(intptr_t)index);

    lv_obj_t *name = lv_label_create(time_area);
    lv_obj_set_pos(name, 12, 4);
    lv_label_set_text_fmt(name, "闹钟 %d", index + 1);
    lv_obj_set_style_text_font(name, &lv_customer_font_ZiTiQuanWeiJunHeiW22_18, 0);
    lv_obj_set_style_text_color(name, lv_color_hex(0xffffff), 0);

    lv_obj_t *time = lv_label_create(time_area);
    lv_obj_set_pos(time, 12, 25);
    lv_label_set_text_fmt(time, "%02u:%02u", hour, minute);
    s_time_labels[index] = time;
    lv_obj_set_style_text_font(time, &lv_font_ZiTiQuanWeiJunHeiW22_24, 0);
    lv_obj_set_style_text_color(time, lv_color_hex(0xffffff), 0);

    lv_obj_t *toggle = lv_switch_create(row);
    lv_obj_set_pos(toggle, 155, 10);
    lv_obj_set_size(toggle, 56, 31);
    lv_obj_set_style_bg_color(toggle, lv_color_hex(0x55657a), LV_PART_MAIN);
    lv_obj_set_style_bg_opa(toggle, LV_OPA_COVER, LV_PART_MAIN);
    lv_obj_set_style_bg_color(toggle, lv_color_hex(0x2195f6),
                              LV_PART_INDICATOR | LV_STATE_CHECKED);
    lv_obj_set_style_bg_opa(toggle, LV_OPA_COVER,
                            LV_PART_INDICATOR | LV_STATE_CHECKED);
    lv_obj_set_style_bg_color(toggle, lv_color_hex(0xffffff), LV_PART_KNOB);
    lv_obj_set_style_bg_opa(toggle, LV_OPA_COVER, LV_PART_KNOB);
    if (alarm_is_enabled(index)) lv_obj_add_state(toggle, LV_STATE_CHECKED);
    s_toggles[index] = toggle;
    lv_obj_add_event_cb(toggle, toggle_alarm_cb, LV_EVENT_VALUE_CHANGED,
                        (void *)(intptr_t)index);
}

void alarm_list_page_init(lv_ui *ui)
{
    if (!ui || !ui->screen_clock) return;
    s_screen = ui->screen_clock;
    lv_obj_add_event_cb(s_screen, alarm_screen_deleted, LV_EVENT_DELETE, NULL);

    lv_obj_add_flag(ui->screen_clock_list_clock, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(ui->screen_clock_sw_1, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(ui->screen_clock_sw_2, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(ui->screen_clock_sw_3, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(ui->screen_clock_sw_4, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(ui->screen_clock_label_1, LV_OBJ_FLAG_HIDDEN);

    lv_obj_t *title = lv_label_create(ui->screen_clock);
    lv_obj_set_pos(title, 16, 12);
    lv_label_set_text(title, "闹钟");
    lv_obj_set_style_text_font(title, &lv_font_ZiTiQuanWeiJunHeiW22_24, 0);
    lv_obj_set_style_text_color(title, lv_color_hex(0xffffff), 0);

    for (int i = 0; i < ALARM_COUNT; ++i) make_alarm_row(ui->screen_clock, i);
}
