#include "app_theme.h"
#include "alarm_set_page.h"

#include <stdint.h>
#include <stdio.h>

#include "alarm_clock.h"
#include "guider_customer_fonts.h"

typedef struct {
    lv_ui *ui;
    lv_obj_t *hour_value;
    lv_obj_t *minute_value;
    int index;
    uint8_t hour;
    uint8_t minute;
} alarm_set_page_t;

static lv_obj_t *make_label(lv_obj_t *parent, const char *text, int x, int y,
                            int width, int height, uint32_t color, const lv_font_t *font)
{
    lv_obj_t *label = lv_label_create(parent);
    lv_obj_set_pos(label, x, y);
    lv_obj_set_size(label, width, height);
    lv_label_set_text(label, text);
    lv_obj_set_style_text_align(label, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_set_style_text_color(label, lv_color_hex(color), 0);
    lv_obj_set_style_text_font(label, font, 0);
    return label;
}

static void refresh_time(alarm_set_page_t *page)
{
    lv_label_set_text_fmt(page->hour_value, "%02u", page->hour);
    lv_label_set_text_fmt(page->minute_value, "%02u", page->minute);
    lv_label_set_text(page->ui->screen_set_clock_btn_set_label, "保存闹钟");
}

static void change_time_cb(lv_event_t *event)
{
    alarm_set_page_t *page = lv_event_get_user_data(event);
    lv_obj_t *button = lv_event_get_current_target_obj(event);
    int action = (int)(intptr_t)lv_obj_get_user_data(button);

    if (action == 1) page->hour = (page->hour + 1) % 24;
    else if (action == -1) page->hour = (page->hour + 23) % 24;
    else if (action == 2) page->minute = (page->minute + 10) % 60;
    else if (action == -2) page->minute = (page->minute + 50) % 60;
    refresh_time(page);
}

static void save_cb(lv_event_t *event)
{
    alarm_set_page_t *page = lv_event_get_user_data(event);
    if (alarm_set_time(page->index, page->hour, page->minute) == ESP_OK &&
        alarm_set_enabled(page->index, true) == ESP_OK) {
        lv_label_set_text(page->ui->screen_set_clock_btn_set_label, "已保存");
    }
}

static void page_delete_cb(lv_event_t *event)
{
    lv_free(lv_event_get_user_data(event));
}

static void make_adjust_button(lv_obj_t *parent, int x, int y, const char *text,
                               int action, alarm_set_page_t *page)
{
    lv_obj_t *button = lv_button_create(parent);
    lv_obj_set_pos(button, x, y);
    lv_obj_set_size(button, 100, 46);
    lv_obj_set_style_radius(button, 12, 0);
    lv_obj_set_style_bg_color(button, lv_color_hex(APP_THEME_RAISED), 0);
    lv_obj_set_style_shadow_width(button, 0, 0);
    lv_obj_set_user_data(button, (void *)(intptr_t)action);
    lv_obj_add_event_cb(button, change_time_cb, LV_EVENT_CLICKED, page);
    make_label(button, text, 0, 8, 100, 30, APP_THEME_TEXT,
               &lv_font_ZiTiQuanWeiJunHeiW22_24);
}

void alarm_set_page_init(lv_ui *ui)
{
    if (!ui || !ui->screen_set_clock) return;

    alarm_set_page_t *page = lv_malloc(sizeof(*page));
    if (!page) return;
    page->ui = ui;
    page->index = alarm_get_editing_index();
    page->hour = 0;
    page->minute = 0;
    if (page->index < 0 || page->index >= ALARM_COUNT ||
        alarm_get_time(page->index, &page->hour, &page->minute) != ESP_OK) {
        lv_free(page);
        return;
    }

    lv_obj_add_flag(ui->screen_set_clock_roller_hour, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(ui->screen_set_clock_roller_minute, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_event_cb(ui->screen_set_clock, page_delete_cb, LV_EVENT_DELETE, page);

    lv_obj_t *screen = ui->screen_set_clock;
    char title[32];
    snprintf(title, sizeof(title), "设置闹钟 %d", page->index + 1);
    make_label(screen, title, 10, 12, 220, 31, APP_THEME_TEXT,
               &lv_font_ZiTiQuanWeiJunHeiW22_24);
    make_label(screen, "小时", 12, 49, 100, 25, APP_THEME_TEXT,
               &lv_customer_font_ZiTiQuanWeiJunHeiW22_18);
    make_label(screen, "分钟", 128, 49, 100, 25, APP_THEME_TEXT,
               &lv_customer_font_ZiTiQuanWeiJunHeiW22_18);

    make_adjust_button(screen, 12, 76, "+", 1, page);
    make_adjust_button(screen, 128, 76, "+", 2, page);
    page->hour_value = make_label(screen, "00", 12, 132, 100, 39, APP_THEME_TEXT,
                                  &lv_font_ZiTiQuanWeiJunHeiW22_24);
    make_label(screen, ":", 112, 130, 16, 39, APP_THEME_ACCENT,
               &lv_font_ZiTiQuanWeiJunHeiW22_24);
    page->minute_value = make_label(screen, "00", 128, 132, 100, 39, APP_THEME_TEXT,
                                    &lv_font_ZiTiQuanWeiJunHeiW22_24);
    make_adjust_button(screen, 12, 178, "-", -1, page);
    make_adjust_button(screen, 128, 178, "-", -2, page);

    lv_obj_set_style_radius(ui->screen_set_clock_btn_set, 12, 0);
    lv_obj_set_style_text_font(ui->screen_set_clock_btn_set_label,
                               &lv_customer_font_ZiTiQuanWeiJunHeiW22_18, 0);
    lv_obj_add_event_cb(ui->screen_set_clock_btn_set, save_cb, LV_EVENT_CLICKED, page);
    refresh_time(page);
}
