#include "app_theme.h"
#include "quick_settings_page.h"

#include <stdio.h>
#include <time.h>

#include "esp_wifi.h"
#include "guider_customer_fonts.h"
#include "quick_settings_icons.h"
#include "system_settings.h"
#include "wifi_connect.h"

#define COLOR_BG       APP_THEME_BG
#define COLOR_CARD     APP_THEME_CARD
#define COLOR_TEXT     APP_THEME_TEXT
#define COLOR_SUBTEXT  APP_THEME_MUTED



static lv_ui *s_ui;
static lv_obj_t *s_panel;
static lv_obj_t *s_time;
static lv_obj_t *s_wifi_status;
static lv_obj_t *s_brightness_value;
static lv_obj_t *s_volume_value;
static lv_timer_t *s_status_timer;
static int16_t s_press_y = -1;

typedef struct {
    lv_obj_t *slider;
    lv_obj_t *value_label;
    bool brightness;
    bool changed;
    bool card_active;
} slider_touch_t;

static slider_touch_t s_brightness_touch;
static slider_touch_t s_volume_touch;

static void set_rect(lv_obj_t *obj, int x, int y, int w, int h,
                     uint32_t color, int radius)
{
    lv_obj_set_pos(obj, x, y);
    lv_obj_set_size(obj, w, h);
    lv_obj_set_style_bg_color(obj, lv_color_hex(color), 0);
    lv_obj_set_style_bg_opa(obj, LV_OPA_COVER, 0);
    lv_obj_set_style_border_width(obj, 0, 0);
    lv_obj_set_style_radius(obj, radius, 0);
    lv_obj_set_style_shadow_width(obj, 0, 0);
    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
}

static lv_obj_t *add_label(lv_obj_t *parent, int x, int y, int w, int h,
                           const char *text, bool large, uint32_t color)
{
    lv_obj_t *label = lv_label_create(parent);
    lv_obj_set_pos(label, x, y);
    lv_obj_set_size(label, w, h);
    lv_label_set_text(label, text);
    lv_obj_set_style_text_color(label, lv_color_hex(color), 0);
    lv_obj_set_style_text_font(label,
        large ? &lv_customer_font_ZiTiQuanWeiJunHeiW22_18
              : &lv_customer_font_ZiTiQuanWeiJunHeiW22_12, 0);
    lv_label_set_long_mode(label, LV_LABEL_LONG_DOT);
    return label;
}

static bool *current_screen_deleted_flag(lv_ui *ui, lv_obj_t *screen)
{
    if (screen == ui->screen_home) return &ui->screen_home_del;
    if (screen == ui->screen_1) return &ui->screen_1_del;
    if (screen == ui->screen_selete) return &ui->screen_selete_del;
    if (screen == ui->screen_AI) return &ui->screen_AI_del;
    if (screen == ui->screen_Rli) return &ui->screen_Rli_del;
    if (screen == ui->screen_wifi) return &ui->screen_wifi_del;
    if (screen == ui->screen_wifi_connect) return &ui->screen_wifi_connect_del;
    if (screen == ui->screen_weather) return &ui->screen_weather_del;
    if (screen == ui->screen_clock) return &ui->screen_clock_del;
    if (screen == ui->screen_set_clock) return &ui->screen_set_clock_del;
    if (screen == ui->screen_musiclist) return &ui->screen_musiclist_del;
    if (screen == ui->screen_play) return &ui->screen_play_del;
    if (screen == ui->screen_down) return &ui->screen_down_del;
    return NULL;
}

static void refresh_status(lv_timer_t *timer)
{
    (void)timer;
    time_t now = time(NULL);
    struct tm local_time;
    localtime_r(&now, &local_time);
    lv_label_set_text_fmt(s_time, "%02d:%02d", local_time.tm_hour, local_time.tm_min);

    wifi_ap_record_t ap = {0};
    if (wifi_connect_is_connected() && esp_wifi_sta_get_ap_info(&ap) == ESP_OK) {
        lv_label_set_text_fmt(s_wifi_status, "已连接  %s", ap.ssid);
        lv_obj_set_style_text_color(s_wifi_status, lv_color_hex(APP_THEME_ACCENT), 0);
    } else {
        lv_label_set_text(s_wifi_status, "未连接  点击设置");
        lv_obj_set_style_text_color(s_wifi_status, lv_color_hex(COLOR_SUBTEXT), 0);
    }
}

static void close_panel(void)
{
    if (!s_panel) return;
    if (s_status_timer) {
        lv_timer_delete(s_status_timer);
        s_status_timer = NULL;
    }
    lv_obj_t *panel = s_panel;
    s_panel = NULL;
    s_time = NULL;
    s_wifi_status = NULL;
    s_brightness_value = NULL;
    s_volume_value = NULL;
    lv_obj_delete(panel);
}

static void navigate_wifi_async(void *user_data)
{
    lv_ui *ui = user_data;
    lv_obj_t *screen = lv_screen_active();
    bool *old_deleted = current_screen_deleted_flag(ui, screen);
    close_panel();
    if (screen == ui->screen_wifi || !old_deleted) return;

    ui_load_scr_animation(ui, &ui->screen_wifi, ui->screen_wifi_del,
                          old_deleted, setup_scr_screen_wifi,
                          LV_SCR_LOAD_ANIM_NONE, 0, 0, true, true);
}

static void wifi_clicked_cb(lv_event_t *event)
{
    (void)event;
    lv_async_call(navigate_wifi_async, s_ui);
}

static void preview_slider(slider_touch_t *control)
{
    uint8_t value = (uint8_t)lv_slider_get_value(control->slider);
    lv_label_set_text_fmt(control->value_label, "%u%%", value);
    if (control->brightness) (void)system_preview_brightness(value);
    else (void)system_preview_volume(value);
    control->changed = true;
}

static void set_slider_from_x(slider_touch_t *control, int32_t x)
{
    lv_area_t area;
    lv_obj_get_coords(control->slider, &area);
    if (x < area.x1) x = area.x1;
    if (x > area.x2) x = area.x2;
    int32_t width = area.x2 - area.x1;
    if (width <= 0) return;
    int32_t minimum = lv_slider_get_min_value(control->slider);
    int32_t maximum = lv_slider_get_max_value(control->slider);
    int32_t value = minimum + (x - area.x1) * (maximum - minimum) / width;
    if (value != lv_slider_get_value(control->slider)) {
        lv_slider_set_value(control->slider, value, LV_ANIM_OFF);
        preview_slider(control);
    }
}

static void save_slider(slider_touch_t *control)
{
    if (!control->changed) return;
    uint8_t value = (uint8_t)lv_slider_get_value(control->slider);
    if (control->brightness) (void)system_set_brightness(value);
    else (void)system_set_volume(value);
    control->changed = false;
}

static void slider_event_cb(lv_event_t *event)
{
    slider_touch_t *control = lv_event_get_user_data(event);
    lv_event_code_t code = lv_event_get_code(event);

    if (code == LV_EVENT_PRESSED) {
        control->changed = false;
        lv_point_t point;
        lv_indev_get_point(lv_indev_active(), &point);
        set_slider_from_x(control, point.x);
    } else if (code == LV_EVENT_VALUE_CHANGED) {
        preview_slider(control);
    } else if (code == LV_EVENT_RELEASED || code == LV_EVENT_PRESS_LOST) {
        save_slider(control);
    }
}

static void volume_card_event_cb(lv_event_t *event)
{
    slider_touch_t *control = lv_event_get_user_data(event);
    lv_event_code_t code = lv_event_get_code(event);
    if (code == LV_EVENT_PRESSED) {
        lv_point_t point;
        lv_indev_get_point(lv_indev_active(), &point);
        control->card_active = point.x >= 50;
        control->changed = false;
    }
    if ((code == LV_EVENT_PRESSED || code == LV_EVENT_PRESSING) &&
        control->card_active) {
        lv_point_t point;
        lv_indev_get_point(lv_indev_active(), &point);
        set_slider_from_x(control, point.x);
    } else if (code == LV_EVENT_RELEASED || code == LV_EVENT_PRESS_LOST) {
        if (control->card_active) save_slider(control);
        control->card_active = false;
    }
}

static lv_obj_t *add_slider(lv_obj_t *parent, int y, int value,
                            uint32_t color, lv_obj_t *value_label,
                            slider_touch_t *control, bool brightness)
{
    lv_obj_t *slider = lv_slider_create(parent);
    lv_obj_set_pos(slider, 54, y - 2);
    lv_obj_set_size(slider, 148, 22);
    lv_obj_set_ext_click_area(slider, 14);
    lv_obj_set_style_bg_color(slider, lv_color_hex(APP_THEME_RAISED), LV_PART_MAIN);
    lv_obj_set_style_bg_opa(slider, LV_OPA_COVER, LV_PART_MAIN);
    lv_obj_set_style_border_width(slider, 0, LV_PART_MAIN);
    lv_obj_set_style_radius(slider, 9, LV_PART_MAIN);
    lv_obj_set_style_bg_color(slider, lv_color_hex(color), LV_PART_INDICATOR);
    lv_obj_set_style_bg_opa(slider, LV_OPA_COVER, LV_PART_INDICATOR);
    lv_obj_set_style_radius(slider, 9, LV_PART_INDICATOR);
    lv_obj_set_style_bg_color(slider, lv_color_hex(APP_THEME_TEXT), LV_PART_KNOB);
    lv_obj_set_style_bg_opa(slider, LV_OPA_COVER, LV_PART_KNOB);
    lv_obj_set_style_pad_all(slider, 5, LV_PART_KNOB);
    lv_slider_set_range(slider, 0, 100);
    lv_slider_set_value(slider, value, LV_ANIM_OFF);
    *control = (slider_touch_t){
        .slider = slider, .value_label = value_label, .brightness = brightness,
    };
    // 点轨道时立即定位；拖动仍交给 LVGL 原生滑条处理。
    lv_obj_add_event_cb(slider, slider_event_cb, LV_EVENT_PRESSED, control);
    lv_obj_add_event_cb(slider, slider_event_cb, LV_EVENT_VALUE_CHANGED, control);
    lv_obj_add_event_cb(slider, slider_event_cb, LV_EVENT_RELEASED, control);
    lv_obj_add_event_cb(slider, slider_event_cb, LV_EVENT_PRESS_LOST, control);
    return slider;
}

static lv_obj_t *add_icon(lv_obj_t *parent, int x, int y,
                          const lv_image_dsc_t *source)
{
    lv_obj_t *image = lv_image_create(parent);
    lv_image_set_src(image, source);
    lv_obj_set_pos(image, x, y);
    lv_obj_remove_flag(image, LV_OBJ_FLAG_CLICKABLE);
    return image;
}

void quick_settings_page_open(lv_ui *ui)
{
    if (!ui || s_panel || lv_screen_active() == ui->screen_down) return;
    s_ui = ui;

    // 顶层遮罩保留当前页面；收起后直接回到原处，滚动位置也不丢失。
    s_panel = lv_obj_create(lv_layer_top());
    set_rect(s_panel, 0, 0, 240, 284, COLOR_BG, 0);
    lv_obj_set_style_pad_all(s_panel, 0, 0);
    lv_obj_add_flag(s_panel, LV_OBJ_FLAG_CLICKABLE);

    lv_obj_t *handle = lv_obj_create(s_panel);
    set_rect(handle, 99, 6, 42, 4, APP_THEME_RAISED, 2);
    lv_obj_remove_flag(handle, LV_OBJ_FLAG_CLICKABLE);

    add_label(s_panel, 14, 19, 145, 27, "快捷设置", true, COLOR_TEXT);
    s_time = add_label(s_panel, 180, 23, 49, 20, "--:--", false, COLOR_SUBTEXT);
    lv_obj_set_style_text_align(s_time, LV_TEXT_ALIGN_RIGHT, 0);

    lv_obj_t *wifi = lv_obj_create(s_panel);
    set_rect(wifi, 10, 55, 220, 64, APP_THEME_CARD, 14);
    lv_obj_set_style_pad_all(wifi, 0, 0);
    lv_obj_add_flag(wifi, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_t *wifi_icon = lv_obj_create(wifi);
    set_rect(wifi_icon, 8, 11, 42, 42, APP_THEME_RAISED, 11);
    lv_obj_set_style_pad_all(wifi_icon, 0, 0);
    lv_obj_remove_flag(wifi_icon, LV_OBJ_FLAG_CLICKABLE);
    add_icon(wifi_icon, 3, 3, &quick_icon_wifi);
    add_label(wifi, 60, 10, 136, 25, "WiFi 网络", true, COLOR_TEXT);
    s_wifi_status = add_label(wifi, 60, 37, 147, 20, "正在检查", false, APP_THEME_ACCENT);
    add_icon(wifi, 199, 24, &quick_icon_chevron);
    lv_obj_add_event_cb(wifi, wifi_clicked_cb, LV_EVENT_CLICKED, NULL);

    lv_obj_t *brightness = lv_obj_create(s_panel);
    set_rect(brightness, 10, 127, 220, 64, COLOR_CARD, 14);
    lv_obj_set_style_pad_all(brightness, 0, 0);
    lv_obj_remove_flag(brightness, LV_OBJ_FLAG_CLICKABLE);
    add_label(brightness, 10, 7, 80, 24, "亮度", true, COLOR_TEXT);
    s_brightness_value = add_label(brightness, 177, 9, 32, 20, "", false, COLOR_SUBTEXT);
    lv_obj_set_style_text_align(s_brightness_value, LV_TEXT_ALIGN_RIGHT, 0);
    add_icon(brightness, 13, 33, &quick_icon_sun);
    lv_obj_t *brightness_slider = add_slider(
        brightness, 36, system_get_brightness(), APP_THEME_ACCENT,
        s_brightness_value, &s_brightness_touch, true);
    lv_slider_set_range(brightness_slider, 5, 100);
    lv_label_set_text_fmt(s_brightness_value, "%u%%", system_get_brightness());

    lv_obj_t *volume = lv_obj_create(s_panel);
    set_rect(volume, 10, 199, 220, 64, COLOR_CARD, 14);
    lv_obj_set_style_pad_all(volume, 0, 0);
    // 卡片上沿与滑条共享调节动作，触摸坐标略高时也能调节音量。
    lv_obj_add_flag(volume, LV_OBJ_FLAG_CLICKABLE);
    add_label(volume, 10, 7, 80, 24, "音量", true, COLOR_TEXT);
    s_volume_value = add_label(volume, 177, 9, 32, 20, "", false, COLOR_SUBTEXT);
    lv_obj_set_style_text_align(s_volume_value, LV_TEXT_ALIGN_RIGHT, 0);
    add_icon(volume, 13, 33, &quick_icon_speaker);
    add_slider(volume, 36, system_get_volume(), APP_THEME_ACCENT,
               s_volume_value, &s_volume_touch, false);
    lv_obj_add_event_cb(volume, volume_card_event_cb, LV_EVENT_PRESSED,
                        &s_volume_touch);
    lv_obj_add_event_cb(volume, volume_card_event_cb, LV_EVENT_PRESSING,
                        &s_volume_touch);
    lv_obj_add_event_cb(volume, volume_card_event_cb, LV_EVENT_RELEASED,
                        &s_volume_touch);
    lv_obj_add_event_cb(volume, volume_card_event_cb, LV_EVENT_PRESS_LOST,
                        &s_volume_touch);
    lv_label_set_text_fmt(s_volume_value, "%u%%", system_get_volume());

    lv_obj_t *hint = add_label(s_panel, 80, 266, 80, 15,
                               "上滑收起", false, APP_THEME_MUTED);
    lv_obj_set_style_text_align(hint, LV_TEXT_ALIGN_CENTER, 0);

    refresh_status(NULL);
    s_status_timer = lv_timer_create(refresh_status, 1000, NULL);
}

static void indev_pressed_cb(lv_event_t *event)
{
    lv_indev_t *indev = lv_event_get_current_target(event);
    lv_point_t point;
    lv_indev_get_point(indev, &point);
    s_press_y = point.y;
}

static void indev_gesture_cb(lv_event_t *event)
{
    lv_indev_t *indev = lv_event_get_current_target(event);
    lv_dir_t dir = lv_indev_get_gesture_dir(indev);
    if (s_panel && dir == LV_DIR_TOP) {
        lv_indev_wait_release(indev);
        lv_indev_stop_processing(indev);
        close_panel();
    } else if (!s_panel && dir == LV_DIR_BOTTOM && s_press_y >= 0 && s_press_y <= 28) {
        lv_indev_wait_release(indev);
        lv_indev_stop_processing(indev);
        quick_settings_page_open(s_ui);
    }
}

void quick_settings_page_bind(lv_ui *ui)
{
    s_ui = ui;
    for (lv_indev_t *indev = lv_indev_get_next(NULL); indev;
         indev = lv_indev_get_next(indev)) {
        if (lv_indev_get_type(indev) != LV_INDEV_TYPE_POINTER) continue;
        lv_indev_add_event_cb(indev, indev_pressed_cb, LV_EVENT_PRESSED, NULL);
        lv_indev_add_event_cb(indev, indev_gesture_cb, LV_EVENT_GESTURE, NULL);
    }
}
