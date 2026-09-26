#include "ota_update_page.h"
#include "ui_landscape.h"

#include <stdio.h>
#include <string.h>

#include "guider_customer_fonts.h"
#include "lvgl.h"
#include "ota_update.h"

static lv_obj_t *s_panel;
static lv_obj_t *s_status_label;
static lv_obj_t *s_progress;
static lv_obj_t *s_close_button;
static lv_timer_t *s_timer;
static int s_last_progress = -1;
static char s_last_message[64];

static lv_obj_t *label(lv_obj_t *parent, int x, int y, int width, int height,
                       const char *text, bool large, uint32_t color)
{
    lv_obj_t *obj = lv_label_create(parent);
    lv_obj_set_pos(obj, x, y);
    lv_obj_set_size(obj, width, height);
    lv_label_set_text(obj, text);
    lv_obj_set_style_text_color(obj, lv_color_hex(color), 0);
    lv_obj_set_style_text_font(obj,
        large ? &lv_customer_font_ZiTiQuanWeiJunHeiW22_18
              : &lv_customer_font_ZiTiQuanWeiJunHeiW22_12, 0);
    lv_label_set_long_mode(obj, LV_LABEL_LONG_WRAP);
    return obj;
}

static void refresh_cb(lv_timer_t *timer)
{
    (void)timer;
    ota_update_status_t state;
    ota_update_get_status(&state);
    if (state.progress_percent != s_last_progress) {
        s_last_progress = state.progress_percent;
        lv_bar_set_value(s_progress, state.progress_percent, LV_ANIM_OFF);
    }
    if (strcmp(state.message, s_last_message) != 0) {
        snprintf(s_last_message, sizeof(s_last_message), "%s", state.message);
        lv_label_set_text(s_status_label, s_last_message);
    }
    if (state.uploading || state.finished) {
        lv_obj_add_state(s_close_button, LV_STATE_DISABLED);
    } else {
        lv_obj_remove_state(s_close_button, LV_STATE_DISABLED);
    }
}

static void close_cb(lv_event_t *event)
{
    (void)event;
    if (ota_update_stop() != ESP_OK) return;
    if (s_timer) {
        lv_timer_delete(s_timer);
        s_timer = NULL;
    }
    lv_obj_t *panel = s_panel;
    s_panel = NULL;
    s_status_label = NULL;
    s_progress = NULL;
    s_close_button = NULL;
    lv_obj_delete(panel);
}

esp_err_t ota_update_page_show(void)
{
    if (s_panel) return ESP_OK;
    char url[64] = {0};
    esp_err_t err = ota_update_start(url, sizeof(url));

    s_panel = lv_obj_create(lv_layer_top());
    lv_obj_set_pos(s_panel, 0, 0);
    lv_obj_set_size(s_panel, 240, 284);
    lv_obj_set_style_bg_color(s_panel, lv_color_hex(0x0b1320), 0);
    lv_obj_set_style_bg_opa(s_panel, LV_OPA_COVER, 0);
    lv_obj_set_style_border_width(s_panel, 0, 0);
    lv_obj_set_style_radius(s_panel, 0, 0);
    lv_obj_set_style_pad_all(s_panel, 0, 0);
    lv_obj_remove_flag(s_panel, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_add_flag(s_panel, LV_OBJ_FLAG_CLICKABLE);

    label(s_panel, 14, 17, 210, 27, "无线更新固件", true, 0xf0f6ff);
    label(s_panel, 14, 54, 210, 45,
          "手机和设备连接同一WiFi，打开下方地址上传固件。", false, 0xb9cce2);
    label(s_panel, 14, 108, 200, 20, "升级地址", false, 0x8fa9c5);

    lv_obj_t *url_box = lv_obj_create(s_panel);
    lv_obj_set_pos(url_box, 11, 130);
    lv_obj_set_size(url_box, 218, 53);
    lv_obj_set_style_bg_color(url_box, lv_color_hex(0x1a2a3d), 0);
    lv_obj_set_style_border_width(url_box, 0, 0);
    lv_obj_set_style_radius(url_box, 10, 0);
    lv_obj_set_style_pad_all(url_box, 0, 0);
    lv_obj_remove_flag(url_box, LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_SCROLLABLE);
    label(url_box, 8, 10, 202, 40,
          err == ESP_OK ? url : "请先连接WiFi", false, 0xffffff);

    s_status_label = label(s_panel, 14, 193, 210, 26,
                           err == ESP_OK ? "等待上传固件" :
                           err == ESP_ERR_INVALID_STATE ? "WiFi未连接，无法启动更新" :
                           "升级服务启动失败",
                           false, 0xb9cce2);
    s_progress = lv_bar_create(s_panel);
    lv_obj_set_pos(s_progress, 14, 218);
    lv_obj_set_size(s_progress, 212, 8);
    lv_bar_set_range(s_progress, 0, 100);
    lv_bar_set_value(s_progress, 0, LV_ANIM_OFF);
    lv_obj_set_style_bg_color(s_progress, lv_color_hex(0x344459), LV_PART_MAIN);
    lv_obj_set_style_bg_color(s_progress, lv_color_hex(0x2685dd), LV_PART_INDICATOR);

    s_close_button = lv_button_create(s_panel);
    lv_obj_set_pos(s_close_button, 12, 239);
    lv_obj_set_size(s_close_button, 216, 38);
    lv_obj_set_style_bg_color(s_close_button, lv_color_hex(0x263d55), 0);
    lv_obj_set_style_radius(s_close_button, 9, 0);
    lv_obj_t *close_text = label(s_close_button, 0, 0, 216, 32,
                                 "关闭", true, 0xffffff);
    lv_obj_set_style_text_align(close_text, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_center(close_text);
    lv_obj_add_event_cb(s_close_button, close_cb, LV_EVENT_CLICKED, NULL);

    ui_landscape_apply(NULL, s_panel);

    s_last_progress = -1;
    s_last_message[0] = '\0';
    if (err == ESP_OK) {
        s_timer = lv_timer_create(refresh_cb, 500, NULL);
        refresh_cb(NULL);
    }
    return err;
}
