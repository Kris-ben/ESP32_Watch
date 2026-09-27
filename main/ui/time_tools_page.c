#include "time_tools_page.h"

#include <stdint.h>

#include "lvgl.h"
#include "time_tools.h"
#include "watch_standby.h"
#include "guider_customer_fonts.h"

static lv_obj_t *s_page;
static lv_obj_t *s_alert;
static lv_obj_t *s_countdown_text;
static lv_obj_t *s_stopwatch_text;
static lv_obj_t *s_countdown_toggle;
static lv_obj_t *s_stopwatch_toggle;
static lv_obj_t *s_lap_text;

static lv_obj_t *label(lv_obj_t *parent, const char *text, int x, int y,
                       const lv_font_t *font)
{
    lv_obj_t *obj = lv_label_create(parent);
    lv_label_set_text(obj, text);
    lv_obj_set_pos(obj, x, y);
    lv_obj_set_style_text_font(obj, font, 0);
    lv_obj_set_style_text_color(obj, lv_color_hex(0xf4f7fc), 0);
    return obj;
}

static lv_obj_t *button(lv_obj_t *parent, const char *text,
                        int x, int y, int width, int height,
                        lv_event_cb_t callback)
{
    lv_obj_t *obj = lv_button_create(parent);
    lv_obj_set_pos(obj, x, y);
    lv_obj_set_size(obj, width, height);
    lv_obj_set_style_radius(obj, 9, 0);
    lv_obj_set_style_bg_color(obj, lv_color_hex(0x29445f), 0);
    lv_obj_set_style_shadow_width(obj, 0, 0);
    lv_obj_t *caption = label(obj, text, 0, 0,
                              &lv_customer_font_ZiTiQuanWeiJunHeiW22_18);
    lv_obj_center(caption);
    lv_obj_add_event_cb(obj, callback, LV_EVENT_CLICKED, NULL);
    return obj;
}

static void page_deleted(lv_event_t *event)
{
    (void)event;
    s_page = NULL;
    s_countdown_text = NULL;
    s_stopwatch_text = NULL;
    s_countdown_toggle = NULL;
    s_stopwatch_toggle = NULL;
    s_lap_text = NULL;
}

static void close_page(lv_event_t *event)
{
    (void)event;
    if (s_page) lv_obj_delete(s_page);
}

static void preset_five(lv_event_t *event)
{
    (void)event;
    time_tools_countdown_set(5 * 60);
    time_tools_page_update();
}

static void preset_one(lv_event_t *event)
{
    (void)event;
    time_tools_countdown_set(60);
    time_tools_page_update();
}

static void preset_twenty_five(lv_event_t *event)
{
    (void)event;
    time_tools_countdown_set(25 * 60);
    time_tools_page_update();
}

static void countdown_toggle(lv_event_t *event)
{
    (void)event;
    time_tools_countdown_toggle();
    time_tools_page_update();
}

static void countdown_reset(lv_event_t *event)
{
    (void)event;
    time_tools_countdown_reset();
    time_tools_page_update();
}

static void stopwatch_toggle(lv_event_t *event)
{
    (void)event;
    time_tools_stopwatch_toggle();
    time_tools_page_update();
}

static void stopwatch_lap(lv_event_t *event)
{
    (void)event;
    uint32_t ms;
    uint32_t lap;
    if (s_lap_text && time_tools_stopwatch_lap(&ms, &lap))
        lv_label_set_text_fmt(s_lap_text, "第%lu圈 %02lu:%02lu.%01lu",
                              (unsigned long)lap,
                              (unsigned long)(ms / 60000),
                              (unsigned long)(ms / 1000 % 60),
                              (unsigned long)(ms / 100 % 10));
}

static void stopwatch_reset(lv_event_t *event)
{
    (void)event;
    time_tools_stopwatch_reset();
    if (s_lap_text) lv_label_set_text(s_lap_text, "");
    time_tools_page_update();
}

void time_tools_page_update(void)
{
    if (!s_page) return;
    uint32_t seconds = time_tools_countdown_remaining();
    lv_label_set_text_fmt(s_countdown_text, "%02lu:%02lu",
                          (unsigned long)(seconds / 60),
                          (unsigned long)(seconds % 60));
    uint32_t ms = time_tools_stopwatch_elapsed_ms();
    lv_label_set_text_fmt(s_stopwatch_text, "%02lu:%02lu.%01lu",
                          (unsigned long)(ms / 60000),
                          (unsigned long)(ms / 1000 % 60),
                          (unsigned long)(ms / 100 % 10));
    lv_obj_t *caption = lv_obj_get_child(s_countdown_toggle, 0);
    lv_label_set_text(caption, time_tools_countdown_running() ? "暂停" : "开始");
    lv_obj_center(caption);
    caption = lv_obj_get_child(s_stopwatch_toggle, 0);
    lv_label_set_text(caption, time_tools_stopwatch_running() ? "暂停" : "开始");
    lv_obj_center(caption);
}

void time_tools_page_open(void)
{
    if (s_page) return;
    s_page = lv_obj_create(lv_screen_active());
    lv_obj_set_pos(s_page, 0, 0);
    lv_obj_set_size(s_page, 284, 240);
    lv_obj_set_style_radius(s_page, 0, 0);
    lv_obj_set_style_border_width(s_page, 0, 0);
    lv_obj_set_style_pad_all(s_page, 0, 0);
    lv_obj_set_style_bg_color(s_page, lv_color_hex(0x0d1826), 0);
    lv_obj_remove_flag(s_page, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_add_event_cb(s_page, page_deleted, LV_EVENT_DELETE, NULL);

    label(s_page, "计时工具", 12, 10, &lv_customer_font_ZiTiQuanWeiJunHeiW22_18);
    button(s_page, "返回", 218, 6, 56, 30, close_page);
    label(s_page, "倒计时", 12, 43, &lv_customer_font_ZiTiQuanWeiJunHeiW22_18);
    s_countdown_text = label(s_page, "05:00", 171, 40, &lv_font_montserrat_24);
    button(s_page, "1分", 8, 76, 49, 34, preset_one);
    button(s_page, "5分", 63, 76, 49, 34, preset_five);
    button(s_page, "25分", 118, 76, 49, 34, preset_twenty_five);
    s_countdown_toggle = button(s_page, "开始", 173, 76, 49, 34, countdown_toggle);
    button(s_page, "重置", 228, 76, 49, 34, countdown_reset);

    lv_obj_t *line = lv_obj_create(s_page);
    lv_obj_set_pos(line, 10, 120);
    lv_obj_set_size(line, 264, 1);
    lv_obj_set_style_border_width(line, 0, 0);
    lv_obj_set_style_bg_color(line, lv_color_hex(0x37506b), 0);
    label(s_page, "秒表", 12, 130, &lv_customer_font_ZiTiQuanWeiJunHeiW22_18);
    s_stopwatch_text = label(s_page, "00:00.0", 156, 128, &lv_font_montserrat_24);
    s_stopwatch_toggle = button(s_page, "开始", 10, 171, 80, 36, stopwatch_toggle);
    button(s_page, "计圈", 102, 171, 80, 36, stopwatch_lap);
    button(s_page, "重置", 194, 171, 80, 36, stopwatch_reset);
    s_lap_text = label(s_page, "", 12, 213,
                       &lv_customer_font_ZiTiQuanWeiJunHeiW22_12);
    time_tools_page_update();
}

static void alert_deleted(lv_event_t *event)
{
    (void)event;
    s_alert = NULL;
    watch_standby_set_timer_alert_active(false);
}

static void alert_dismissed(lv_event_t *event)
{
    (void)event;
    if (s_alert) lv_obj_delete(s_alert);
}

void time_tools_page_show_finished(void)
{
    if (s_alert) return;
    watch_standby_set_timer_alert_active(true);
    s_alert = lv_obj_create(lv_screen_active());
    lv_obj_set_pos(s_alert, 0, 0);
    lv_obj_set_size(s_alert, 284, 240);
    lv_obj_set_style_bg_color(s_alert, lv_color_hex(0x101d2e), 0);
    lv_obj_set_style_border_width(s_alert, 0, 0);
    lv_obj_set_style_radius(s_alert, 0, 0);
    lv_obj_add_event_cb(s_alert, alert_deleted, LV_EVENT_DELETE, NULL);
    label(s_alert, "倒计时结束", 82, 70, &lv_customer_font_ZiTiQuanWeiJunHeiW22_18);
    button(s_alert, "知道了", 92, 138, 100, 42, alert_dismissed);
}
