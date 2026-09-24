#include "app_theme.h"
#include "wifi_scan_page.h"

#include "esp_wifi.h"
#include "guider_customer_fonts.h"
#include "wifi_connect.h"
#include "ota_update_page.h"
#include <string.h>

typedef struct {
    lv_ui *ui;
    lv_obj_t *screen;
    lv_obj_t *status;
    lv_obj_t *empty_box;
    lv_obj_t *empty_text;
    lv_obj_t *spinner;
    lv_timer_t *status_timer;
} wifi_scan_page_t;

static wifi_scan_page_t *s_page;

static void upgrade_clicked_cb(lv_event_t *event)
{
    (void)event;
    (void)ota_update_page_show();
}

static void refresh_connection_status(wifi_scan_page_t *page)
{
    bool connected = wifi_connect_is_connected();
    lv_label_set_text(page->status, connected ? "已连接" : "未连接");
    lv_obj_set_style_text_color(page->status,
                                lv_color_hex(connected ? APP_THEME_ACCENT : APP_THEME_MUTED), 0);
}

static void status_timer_cb(lv_timer_t *timer)
{
    wifi_scan_page_t *page = lv_timer_get_user_data(timer);
    refresh_connection_status(page);
}

static void screen_deleted_cb(lv_event_t *event)
{
    wifi_scan_page_t *page = lv_event_get_user_data(event);
    if (page->status_timer) lv_timer_delete(page->status_timer);
    if (s_page == page) s_page = NULL;
    lv_free(page);
}

void wifi_scan_page_on_started(lv_ui *ui)
{
    wifi_scan_page_t *page = s_page;
    if (!page || page->ui != ui || page->screen != ui->screen_wifi) return;
    lv_obj_add_flag(ui->screen_wifi_list_wifi, LV_OBJ_FLAG_HIDDEN);
    lv_obj_remove_flag(page->empty_box, LV_OBJ_FLAG_HIDDEN);
    lv_obj_remove_flag(page->spinner, LV_OBJ_FLAG_HIDDEN);
    lv_label_set_text(page->empty_text, "正在扫描附近热点");
    lv_label_set_text(ui->screen_wifi_btn_scanf_label, "扫描中");
    lv_obj_add_state(ui->screen_wifi_btn_scanf, LV_STATE_DISABLED);
}

void wifi_scan_page_on_finished(lv_ui *ui, int count)
{
    wifi_scan_page_t *page = s_page;
    if (!page || page->ui != ui || page->screen != ui->screen_wifi) return;
    lv_obj_add_flag(page->spinner, LV_OBJ_FLAG_HIDDEN);
    lv_label_set_text(ui->screen_wifi_btn_scanf_label, "重新扫描");
    lv_obj_remove_state(ui->screen_wifi_btn_scanf, LV_STATE_DISABLED);

    lv_obj_t *items[] = {
        ui->screen_wifi_list_wifi_item0, ui->screen_wifi_list_wifi_item1,
        ui->screen_wifi_list_wifi_item2, ui->screen_wifi_list_wifi_item3,
        ui->screen_wifi_list_wifi_item4,
    };
    wifi_ap_record_t connected_ap = {0};
    bool has_connected_ap = wifi_connect_is_connected() &&
                            esp_wifi_sta_get_ap_info(&connected_ap) == ESP_OK;
    for (int i = 0; i < WIFI_LIST_MAX_COUNT; ++i) {
        if (i < count) lv_obj_remove_flag(items[i], LV_OBJ_FLAG_HIDDEN);
        else lv_obj_add_flag(items[i], LV_OBJ_FLAG_HIDDEN);
        bool current = has_connected_ap && i < count &&
                       strcmp(wifi_connect_get_scanned_ssid(i),
                              (const char *)connected_ap.ssid) == 0;
        lv_obj_set_style_bg_color(items[i],
                                  lv_color_hex(current ? APP_THEME_RAISED : APP_THEME_CARD), 0);
        lv_obj_set_style_border_width(items[i], current ? 1 : 0, 0);
        if (current) lv_obj_set_style_border_color(items[i], lv_color_hex(APP_THEME_BORDER), 0);
    }

    if (count > 0) {
        lv_obj_add_flag(page->empty_box, LV_OBJ_FLAG_HIDDEN);
        lv_obj_remove_flag(ui->screen_wifi_list_wifi, LV_OBJ_FLAG_HIDDEN);
    } else {
        lv_obj_add_flag(ui->screen_wifi_list_wifi, LV_OBJ_FLAG_HIDDEN);
        lv_obj_remove_flag(page->empty_box, LV_OBJ_FLAG_HIDDEN);
        lv_label_set_text(page->empty_text,
                          count < 0 ? "扫描失败，请重试" : "附近没有找到热点");
    }
    refresh_connection_status(page);
}

void wifi_scan_page_init(lv_ui *ui)
{
    if (!ui || !ui->screen_wifi) return;

    lv_obj_t *screen = ui->screen_wifi;
    lv_obj_set_style_bg_color(screen, lv_color_hex(APP_THEME_BG), 0);
    lv_obj_remove_flag(screen, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_add_flag(ui->screen_wifi_digital_clock_time, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(ui->screen_wifi_img_wi_close, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(ui->screen_wifi_list_battery, LV_OBJ_FLAG_HIDDEN);

    lv_obj_t *title = lv_label_create(screen);
    lv_obj_set_pos(title, 14, 11);
    lv_label_set_text(title, "WiFi 网络");
    lv_obj_set_style_text_font(title, &lv_customer_font_ZiTiQuanWeiJunHeiW22_18, 0);
    lv_obj_set_style_text_color(title, lv_color_hex(APP_THEME_TEXT), 0);

    lv_obj_t *status = lv_label_create(screen);
    lv_obj_set_pos(status, 166, 18);
    lv_obj_set_size(status, 63, 20);
    lv_obj_set_style_text_font(status, &lv_customer_font_ZiTiQuanWeiJunHeiW22_12, 0);
    lv_obj_set_style_text_align(status, LV_TEXT_ALIGN_RIGHT, 0);

    lv_obj_t *list = ui->screen_wifi_list_wifi;
    lv_obj_set_pos(list, 10, 47);
    lv_obj_set_size(list, 220, 180);
    lv_obj_set_style_bg_opa(list, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(list, 0, 0);
    lv_obj_set_style_pad_all(list, 0, 0);
    lv_obj_set_style_pad_row(list, 5, 0);
    lv_obj_remove_flag(list, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_t *items[] = {
        ui->screen_wifi_list_wifi_item0, ui->screen_wifi_list_wifi_item1,
        ui->screen_wifi_list_wifi_item2, ui->screen_wifi_list_wifi_item3,
        ui->screen_wifi_list_wifi_item4,
    };
    for (int i = 0; i < WIFI_LIST_MAX_COUNT; ++i) {
        lv_obj_t *item = items[i];
        lv_obj_set_height(item, 32);
        lv_obj_set_style_bg_color(item, lv_color_hex(APP_THEME_CARD), 0);
        lv_obj_set_style_radius(item, 9, 0);
        lv_obj_set_style_border_width(item, 0, 0);
        lv_obj_set_style_pad_left(item, 12, 0);
        lv_obj_set_style_pad_right(item, 6, 0);
        lv_obj_set_style_pad_top(item, 2, 0);
        lv_obj_set_style_pad_bottom(item, 2, 0);
        lv_obj_set_style_text_font(item, &lv_customer_font_ZiTiQuanWeiJunHeiW22_18, 0);
        lv_obj_set_style_text_color(item, lv_color_hex(APP_THEME_TEXT), 0);
        uint32_t children = lv_obj_get_child_count(item);
        for (uint32_t child = 0; child < children; ++child) {
            lv_obj_t *part = lv_obj_get_child(item, child);
            if (lv_obj_check_type(part, &lv_image_class)) {
                // 列表生成器附带的 WiFi 字形不在当前字库中，会显示为方框。
                lv_obj_add_flag(part, LV_OBJ_FLAG_HIDDEN);
            } else if (lv_obj_check_type(part, &lv_label_class)) {
                lv_obj_set_style_text_font(part, &lv_customer_font_ZiTiQuanWeiJunHeiW22_18, 0);
                lv_obj_set_style_text_color(part, lv_color_hex(APP_THEME_TEXT), 0);
                lv_label_set_long_mode(part, LV_LABEL_LONG_DOT);
            }
        }
        lv_obj_add_flag(item, LV_OBJ_FLAG_HIDDEN);
    }
    lv_obj_add_flag(list, LV_OBJ_FLAG_HIDDEN);

    lv_obj_t *empty_box = lv_obj_create(screen);
    lv_obj_set_pos(empty_box, 10, 47);
    lv_obj_set_size(empty_box, 220, 180);
    lv_obj_set_style_bg_color(empty_box, lv_color_hex(APP_THEME_SURFACE), 0);
    lv_obj_set_style_border_width(empty_box, 0, 0);
    lv_obj_set_style_radius(empty_box, 11, 0);
    lv_obj_remove_flag(empty_box, LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_t *spinner = lv_spinner_create(empty_box);
    lv_obj_set_pos(spinner, 87, 43);
    lv_obj_set_size(spinner, 34, 34);
    lv_obj_add_flag(spinner, LV_OBJ_FLAG_HIDDEN);

    lv_obj_t *empty_text = lv_label_create(empty_box);
    lv_obj_set_pos(empty_text, 0, 100);
    lv_obj_set_size(empty_text, 220, 26);
    lv_label_set_text(empty_text, "点击下方按钮扫描");
    lv_obj_set_style_text_align(empty_text, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_set_style_text_font(empty_text, &lv_customer_font_ZiTiQuanWeiJunHeiW22_18, 0);
    lv_obj_set_style_text_color(empty_text, lv_color_hex(APP_THEME_TEXT), 0);

    lv_obj_t *button = ui->screen_wifi_btn_scanf;
    lv_obj_set_pos(button, 12, 235);
    lv_obj_set_size(button, 130, 39);
    lv_obj_set_style_radius(button, 10, 0);
    lv_obj_set_style_bg_color(button, lv_color_hex(APP_THEME_ACCENT), 0);
    lv_label_set_text(ui->screen_wifi_btn_scanf_label, "重新扫描");
    lv_obj_set_style_text_font(ui->screen_wifi_btn_scanf_label,
                               &lv_customer_font_ZiTiQuanWeiJunHeiW22_18, 0);
    lv_obj_set_style_text_color(ui->screen_wifi_btn_scanf_label,
                                lv_color_hex(APP_THEME_ON_ACCENT), 0);

    lv_obj_t *upgrade = lv_button_create(screen);
    lv_obj_set_pos(upgrade, 150, 235);
    lv_obj_set_size(upgrade, 78, 39);
    lv_obj_set_style_radius(upgrade, 10, 0);
    lv_obj_set_style_bg_color(upgrade, lv_color_hex(APP_THEME_CARD), 0);
    lv_obj_t *upgrade_text = lv_label_create(upgrade);
    lv_label_set_text(upgrade_text, "升级");
    lv_obj_set_style_text_font(upgrade_text,
                               &lv_customer_font_ZiTiQuanWeiJunHeiW22_18, 0);
    lv_obj_set_style_text_color(upgrade_text, lv_color_hex(APP_THEME_TEXT), 0);
    lv_obj_center(upgrade_text);
    lv_obj_add_event_cb(upgrade, upgrade_clicked_cb, LV_EVENT_CLICKED, NULL);

    wifi_scan_page_t *page = lv_malloc(sizeof(*page));
    if (!page) return;
    *page = (wifi_scan_page_t){
        .ui = ui, .screen = screen, .status = status,
        .empty_box = empty_box, .empty_text = empty_text, .spinner = spinner,
    };
    s_page = page;
    refresh_connection_status(page);
    page->status_timer = lv_timer_create(status_timer_cb, 1000, page);
    lv_obj_add_event_cb(screen, screen_deleted_cb, LV_EVENT_DELETE, page);

    wifi_connect_scan(ui);
}
