#include "calendar_page.h"

#include <stdio.h>
#include <stdint.h>
#include <time.h>

#include "clock_setting.h"
#include "guider_customer_fonts.h"
#include "lvgl.h"

#define CAL_BG       0x090f1a
#define CAL_TEXT     0xf1f6ff
#define CAL_MUTED    0x9bb0c9
#define CAL_CARD     0x172539
#define CAL_ACCENT   0x2777ca

typedef struct calendar_page calendar_page_t;

typedef struct {
    calendar_page_t *page;
    lv_obj_t *button;
    lv_obj_t *label;
    int day;
} calendar_cell_t;

struct calendar_page {
    lv_obj_t *screen;
    lv_obj_t *month_label;
    lv_obj_t *detail_label;
    lv_obj_t *previous_button;
    lv_obj_t *setting_panel;
    lv_obj_t *setting_values[5];
    lv_obj_t *setting_save_label;
    lv_timer_t *timer;
    calendar_cell_t cells[42];
    int year;
    int month;
    int selected_day;
    int today_year;
    int today_month;
    int today_day;
    int setting[5];
};

static const char *const s_weekdays[] = {
    "一", "二", "三", "四", "五", "六", "日",
};

static const char *const s_weekday_names[] = {
    "周日", "周一", "周二", "周三", "周四", "周五", "周六",
};

static bool read_today(struct tm *date)
{
    time_t now = time(NULL);
    if (now < 1704067200 || !localtime_r(&now, date)) return false;
    return date->tm_year + 1900 >= 2024;
}

static int days_in_month(int year, int month)
{
    static const int days[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    int count = days[month - 1];
    if (month == 2 && year % 4 == 0 && (year % 100 != 0 || year % 400 == 0)) {
        count = 29;
    }
    return count;
}

static lv_obj_t *make_label(lv_obj_t *parent, int x, int y, int width,
                            int height, const char *text, bool large,
                            uint32_t color)
{
    lv_obj_t *label = lv_label_create(parent);
    lv_obj_set_pos(label, x, y);
    lv_obj_set_size(label, width, height);
    lv_label_set_text(label, text);
    lv_obj_set_style_text_font(label,
        large ? &lv_customer_font_ZiTiQuanWeiJunHeiW22_18
              : &lv_customer_font_ZiTiQuanWeiJunHeiW22_12, 0);
    lv_obj_set_style_text_color(label, lv_color_hex(color), 0);
    lv_obj_set_style_text_align(label, LV_TEXT_ALIGN_CENTER, 0);
    return label;
}

static lv_obj_t *make_button(lv_obj_t *parent, int x, int y, int width,
                             int height, uint32_t color)
{
    lv_obj_t *button = lv_button_create(parent);
    lv_obj_set_pos(button, x, y);
    lv_obj_set_size(button, width, height);
    lv_obj_set_style_bg_color(button, lv_color_hex(color), 0);
    lv_obj_set_style_bg_opa(button, LV_OPA_COVER, 0);
    lv_obj_set_style_border_width(button, 0, 0);
    lv_obj_set_style_shadow_width(button, 0, 0);
    lv_obj_set_style_radius(button, 9, 0);
    lv_obj_set_style_pad_all(button, 0, 0);
    return button;
}

static void refresh_today(calendar_page_t *page)
{
    struct tm date;
    if (!read_today(&date)) return;
    bool first_sync = page->year == 0;
    page->today_year = date.tm_year + 1900;
    page->today_month = date.tm_mon + 1;
    page->today_day = date.tm_mday;
    if (first_sync) {
        page->year = page->today_year;
        page->month = page->today_month;
        page->selected_day = page->today_day;
    }
}

static void render(calendar_page_t *page)
{
    if (page->year == 0) {
        lv_label_set_text(page->month_label, "等待校时");
        lv_label_set_text(page->detail_label, "连接WiFi后显示当前日期");
        for (int i = 0; i < 42; ++i) {
            lv_obj_add_flag(page->cells[i].button, LV_OBJ_FLAG_HIDDEN);
        }
        return;
    }

    lv_label_set_text_fmt(page->month_label, "%04d年%02d月", page->year, page->month);

    struct tm first = {0};
    first.tm_year = page->year - 1900;
    first.tm_mon = page->month - 1;
    first.tm_mday = 1;
    first.tm_isdst = -1;
    (void)mktime(&first);
    int offset = (first.tm_wday + 6) % 7;
    int month_days = days_in_month(page->year, page->month);

    for (int i = 0; i < 42; ++i) {
        calendar_cell_t *cell = &page->cells[i];
        int day = i - offset + 1;
        if (day < 1 || day > month_days) {
            cell->day = 0;
            lv_obj_add_flag(cell->button, LV_OBJ_FLAG_HIDDEN);
            continue;
        }
        cell->day = day;
        lv_obj_remove_flag(cell->button, LV_OBJ_FLAG_HIDDEN);
        lv_label_set_text_fmt(cell->label, "%d", day);
        bool selected = day == page->selected_day;
        bool today = day == page->today_day &&
                     page->year == page->today_year &&
                     page->month == page->today_month;
        lv_obj_set_style_bg_color(cell->button,
            lv_color_hex(selected ? CAL_ACCENT : CAL_BG), 0);
        lv_obj_set_style_border_width(cell->button, today ? 2 : 0, 0);
        lv_obj_set_style_border_color(cell->button, lv_color_hex(0x65d5c0), 0);
        lv_obj_set_style_text_color(cell->label,
            lv_color_hex(selected ? 0xffffff : i % 7 >= 5 ? 0xe0c093 : CAL_TEXT), 0);
    }

    if (page->selected_day == 0) {
        lv_label_set_text(page->detail_label, "点击日期查看");
        return;
    }
    struct tm selected = first;
    selected.tm_mday = page->selected_day;
    (void)mktime(&selected);
    lv_label_set_text_fmt(page->detail_label, "%02d月%02d日  %s",
                          page->month, page->selected_day,
                          s_weekday_names[selected.tm_wday]);
}

static void day_clicked_cb(lv_event_t *event)
{
    calendar_cell_t *cell = lv_event_get_user_data(event);
    if (cell->day == 0) return;
    cell->page->selected_day = cell->day;
    render(cell->page);
}

static void month_clicked_cb(lv_event_t *event)
{
    calendar_page_t *page = lv_event_get_user_data(event);
    if (page->year == 0) return;
    int direction = lv_event_get_current_target_obj(event) ==
                    page->previous_button ? -1 : 1;
    page->month += direction;
    if (page->month < 1) { page->month = 12; --page->year; }
    if (page->month > 12) { page->month = 1; ++page->year; }
    page->selected_day = 0;
    render(page);
}

static void today_clicked_cb(lv_event_t *event)
{
    calendar_page_t *page = lv_event_get_user_data(event);
    refresh_today(page);
    if (page->today_year) {
        page->year = page->today_year;
        page->month = page->today_month;
        page->selected_day = page->today_day;
    }
    render(page);
}

static void setting_refresh(calendar_page_t *page)
{
    for (int i = 0; i < 5; ++i) {
        if (page->setting_values[i]) {
            lv_label_set_text_fmt(page->setting_values[i], i == 0 ? "%04d" : "%02d",
                                  page->setting[i]);
        }
    }
}

static void setting_adjust_cb(lv_event_t *event)
{
    calendar_page_t *page = lv_event_get_user_data(event);
    int action = (int)(intptr_t)lv_obj_get_user_data(lv_event_get_current_target_obj(event));
    int field = action / 2;
    int step = action % 2 == 0 ? 1 : -1;
    int *value = &page->setting[field];
    static const int minimum[] = {2024, 1, 1, 0, 0};
    static const int maximum[] = {2099, 12, 31, 23, 59};
    int upper = field == 2 ? days_in_month(page->setting[0], page->setting[1])
                           : maximum[field];
    *value += step;
    if (*value > upper) *value = minimum[field];
    if (*value < minimum[field]) *value = upper;
    int last_day = days_in_month(page->setting[0], page->setting[1]);
    if (page->setting[2] > last_day) page->setting[2] = last_day;
    setting_refresh(page);
    lv_label_set_text(page->setting_save_label, "保存到 RTC");
}

static void setting_close_cb(lv_event_t *event)
{
    calendar_page_t *page = lv_event_get_user_data(event);
    lv_obj_delete(page->setting_panel);
    page->setting_panel = NULL;
    for (int i = 0; i < 5; ++i) page->setting_values[i] = NULL;
    page->setting_save_label = NULL;
}

static void setting_save_cb(lv_event_t *event)
{
    calendar_page_t *page = lv_event_get_user_data(event);
    bool rtc_saved = false;
    esp_err_t err = clock_setting_set_local(page->setting[0], page->setting[1],
                                            page->setting[2], page->setting[3],
                                            page->setting[4], &rtc_saved);
    if (err != ESP_OK) {
        lv_label_set_text(page->setting_save_label, "日期无效");
        return;
    }
    lv_label_set_text(page->setting_save_label,
                      rtc_saved ? "已保存到 RTC" : "仅本次有效");
    refresh_today(page);
    page->year = page->today_year;
    page->month = page->today_month;
    page->selected_day = page->today_day;
    render(page);
}

static void setting_open_cb(lv_event_t *event)
{
    calendar_page_t *page = lv_event_get_user_data(event);
    if (page->setting_panel) return;
    time_t now = time(NULL);
    struct tm local;
    if (!localtime_r(&now, &local)) return;
    page->setting[0] = local.tm_year + 1900;
    if (page->setting[0] < 2024 || page->setting[0] > 2099) page->setting[0] = 2024;
    page->setting[1] = local.tm_mon + 1;
    page->setting[2] = local.tm_mday;
    page->setting[3] = local.tm_hour;
    page->setting[4] = local.tm_min;

    int width = lv_obj_get_width(page->screen);
    int height = lv_obj_get_height(page->screen);
    lv_obj_t *panel = lv_obj_create(page->screen);
    page->setting_panel = panel;
    lv_obj_set_pos(panel, 0, 0);
    lv_obj_set_size(panel, width, height);
    lv_obj_set_style_bg_color(panel, lv_color_hex(CAL_BG), 0);
    lv_obj_set_style_bg_opa(panel, LV_OPA_COVER, 0);
    lv_obj_set_style_border_width(panel, 0, 0);
    lv_obj_set_style_pad_all(panel, 0, 0);
    lv_obj_remove_flag(panel, LV_OBJ_FLAG_SCROLLABLE);

    make_label(panel, 12, 13, 140, 30, "离线校时", true, CAL_TEXT);
    lv_obj_t *close = make_button(panel, width - 70, 9, 60, 32, CAL_CARD);
    make_label(close, 0, 8, 60, 18, "关闭", false, CAL_TEXT);
    lv_obj_add_event_cb(close, setting_close_cb, LV_EVENT_CLICKED, page);

    static const char *const field_names[] = {"年", "月", "日", "时", "分"};
    int column_width = (width - 12) / 5;
    for (int i = 0; i < 5; ++i) {
        int x = 6 + i * column_width;
        int button_width = column_width - 4;
        make_label(panel, x, 55, button_width, 20, field_names[i], false, CAL_MUTED);
        lv_obj_t *plus = make_button(panel, x, 77, button_width, 34, CAL_CARD);
        make_label(plus, 0, 6, button_width, 22, "+", true, CAL_TEXT);
        lv_obj_set_user_data(plus, (void *)(intptr_t)(i * 2));
        lv_obj_add_event_cb(plus, setting_adjust_cb, LV_EVENT_CLICKED, page);
        page->setting_values[i] = make_label(panel, x, 116, button_width, 28,
                                             "--", true, CAL_TEXT);
        lv_obj_t *minus = make_button(panel, x, 149, button_width, 34, CAL_CARD);
        make_label(minus, 0, 6, button_width, 22, "-", true, CAL_TEXT);
        lv_obj_set_user_data(minus, (void *)(intptr_t)(i * 2 + 1));
        lv_obj_add_event_cb(minus, setting_adjust_cb, LV_EVENT_CLICKED, page);
    }
    setting_refresh(page);

    lv_obj_t *save = make_button(panel, width / 2 - 58, height - 40,
                                 116, 34, CAL_ACCENT);
    page->setting_save_label = make_label(save, 0, 8, 116, 20,
                                           "保存到 RTC", false, CAL_TEXT);
    lv_obj_add_event_cb(save, setting_save_cb, LV_EVENT_CLICKED, page);
}

static void timer_cb(lv_timer_t *timer)
{
    calendar_page_t *page = lv_timer_get_user_data(timer);
    int old_day = page->today_day;
    refresh_today(page);
    if (old_day != page->today_day) render(page);
}

static void screen_deleted_cb(lv_event_t *event)
{
    calendar_page_t *page = lv_event_get_user_data(event);
    if (page->timer) lv_timer_delete(page->timer);
    lv_free(page);
}

void calendar_page_init(lv_ui *ui)
{
    if (!ui || !ui->screen_Rli) return;
    calendar_page_t *page = lv_malloc(sizeof(*page));
    if (!page) return;
    *page = (calendar_page_t){ .screen = ui->screen_Rli };

    lv_obj_add_flag(ui->screen_Rli_calendar_1, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(ui->screen_Rli_win_1, LV_OBJ_FLAG_HIDDEN);
    lv_obj_set_style_bg_color(page->screen, lv_color_hex(CAL_BG), 0);
    lv_obj_remove_flag(page->screen, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_t *title = make_label(page->screen, 10, 11, 80, 27,
                                 "日历", true, CAL_TEXT);
    lv_obj_set_style_text_align(title, LV_TEXT_ALIGN_LEFT, 0);
    lv_obj_t *setting = make_button(page->screen, 95, 8, 70, 30, CAL_CARD);
    make_label(setting, 0, 7, 70, 18, "校时", false, CAL_TEXT);
    lv_obj_add_event_cb(setting, setting_open_cb, LV_EVENT_CLICKED, page);
    lv_obj_t *today = make_button(page->screen, 174, 8, 56, 30, CAL_CARD);
    make_label(today, 0, 7, 56, 18, "回今天", false, 0x83d9c8);
    lv_obj_add_event_cb(today, today_clicked_cb, LV_EVENT_CLICKED, page);

    lv_obj_t *previous = make_button(page->screen, 10, 46, 34, 36, CAL_CARD);
    page->previous_button = previous;
    make_label(previous, 0, 7, 34, 22, "<", true, CAL_TEXT);
    lv_obj_add_event_cb(previous, month_clicked_cb, LV_EVENT_CLICKED, page);
    page->month_label = make_label(page->screen, 47, 51, 146, 28,
                                  "--", true, CAL_TEXT);
    lv_obj_t *next = make_button(page->screen, 196, 46, 34, 36, CAL_CARD);
    make_label(next, 0, 7, 34, 22, ">", true, CAL_TEXT);
    lv_obj_add_event_cb(next, month_clicked_cb, LV_EVENT_CLICKED, page);

    for (int column = 0; column < 7; ++column) {
        make_label(page->screen, 8 + column * 32, 88, 32, 18,
                   s_weekdays[column], false,
                   column >= 5 ? 0xd3aa79 : CAL_MUTED);
    }
    for (int i = 0; i < 42; ++i) {
        calendar_cell_t *cell = &page->cells[i];
        cell->page = page;
        cell->button = make_button(page->screen, 9 + (i % 7) * 32,
                                   107 + (i / 7) * 23, 30, 22, CAL_BG);
        cell->label = make_label(cell->button, 0, 1, 30, 20,
                                 "", true, CAL_TEXT);
        lv_obj_add_event_cb(cell->button, day_clicked_cb, LV_EVENT_CLICKED, cell);
    }

    lv_obj_t *detail = lv_obj_create(page->screen);
    lv_obj_set_pos(detail, 8, 250);
    lv_obj_set_size(detail, 224, 31);
    lv_obj_set_style_bg_color(detail, lv_color_hex(CAL_CARD), 0);
    lv_obj_set_style_border_width(detail, 0, 0);
    lv_obj_set_style_radius(detail, 9, 0);
    lv_obj_set_style_pad_all(detail, 0, 0);
    lv_obj_remove_flag(detail, LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_SCROLLABLE);
    page->detail_label = make_label(detail, 0, 7, 224, 19,
                                    "", false, CAL_TEXT);

    refresh_today(page);
    render(page);
    page->timer = lv_timer_create(timer_cb, 30000, page);
    lv_obj_add_event_cb(page->screen, screen_deleted_cb, LV_EVENT_DELETE, page);
}
