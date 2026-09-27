#include "wifi_connect_page.h"

#include "guider_customer_fonts.h"
#include "wifi_connect.h"

#include <string.h>

#define WIFI_KEY(width) (LV_BUTTONMATRIX_CTRL_POPOVER | (width))

// 首行缩短字母键的权重，把最右侧退格键的触摸区域从约 38 px 扩至约 66 px。
static const lv_buttonmatrix_ctrl_t wifi_text_keyboard_ctrl[] = {
    LV_KEYBOARD_CTRL_BUTTON_FLAGS | 3,
    WIFI_KEY(2), WIFI_KEY(2), WIFI_KEY(2), WIFI_KEY(2), WIFI_KEY(2),
    WIFI_KEY(2), WIFI_KEY(2), WIFI_KEY(2), WIFI_KEY(2), WIFI_KEY(2),
    LV_BUTTONMATRIX_CTRL_CHECKED | 7,
    LV_KEYBOARD_CTRL_BUTTON_FLAGS | 6,
    WIFI_KEY(3), WIFI_KEY(3), WIFI_KEY(3), WIFI_KEY(3), WIFI_KEY(3),
    WIFI_KEY(3), WIFI_KEY(3), WIFI_KEY(3), WIFI_KEY(3),
    LV_BUTTONMATRIX_CTRL_CHECKED | 7,
    LV_BUTTONMATRIX_CTRL_CHECKED | WIFI_KEY(1),
    LV_BUTTONMATRIX_CTRL_CHECKED | WIFI_KEY(1),
    WIFI_KEY(1), WIFI_KEY(1), WIFI_KEY(1), WIFI_KEY(1),
    WIFI_KEY(1), WIFI_KEY(1), WIFI_KEY(1),
    LV_BUTTONMATRIX_CTRL_CHECKED | WIFI_KEY(1),
    LV_BUTTONMATRIX_CTRL_CHECKED | WIFI_KEY(1),
    LV_BUTTONMATRIX_CTRL_CHECKED | WIFI_KEY(1),
    LV_KEYBOARD_CTRL_BUTTON_FLAGS | 2,
    LV_BUTTONMATRIX_CTRL_CHECKED | 2,
    6,
    LV_BUTTONMATRIX_CTRL_CHECKED | 2,
    LV_KEYBOARD_CTRL_BUTTON_FLAGS | 2,
};

static bool wifi_text_map_matches(const char * const map[], const char *letter)
{
    if (!map) return false;
    size_t buttons = 0;
    for (size_t i = 0; map[i] && map[i][0]; ++i) {
        if (strcmp(map[i], "\n") != 0) ++buttons;
    }
    return buttons == sizeof(wifi_text_keyboard_ctrl) / sizeof(wifi_text_keyboard_ctrl[0]) &&
           strcmp(map[10], letter) == 0 &&
           strcmp(map[11], LV_SYMBOL_BACKSPACE) == 0;
}

static void configure_wifi_password_keyboard(lv_obj_t *keyboard)
{
    const char * const *lower = lv_keyboard_get_map_array(keyboard);
    lv_keyboard_set_mode(keyboard, LV_KEYBOARD_MODE_TEXT_UPPER);
    const char * const *upper = lv_keyboard_get_map_array(keyboard);
    lv_keyboard_set_mode(keyboard, LV_KEYBOARD_MODE_TEXT_LOWER);
    if (!wifi_text_map_matches(lower, "p") || !wifi_text_map_matches(upper, "P")) return;

    lv_keyboard_set_map(keyboard, LV_KEYBOARD_MODE_TEXT_LOWER,
                        lower, wifi_text_keyboard_ctrl);
    lv_keyboard_set_map(keyboard, LV_KEYBOARD_MODE_TEXT_UPPER,
                        upper, wifi_text_keyboard_ctrl);
}

#undef WIFI_KEY

typedef struct {
    lv_ui *ui;
    lv_obj_t *status;
    lv_obj_t *eye_label;
    lv_obj_t *eye;
    lv_obj_t *password_label;
    lv_timer_t *timer;
    unsigned ticks;
} wifi_connect_page_t;

static void set_keyboard_layout(wifi_connect_page_t *page, bool editing)
{
    lv_ui *ui = page->ui;
    lv_obj_t *ssid = ui->screen_wifi_connect_label_wifiname;
    lv_obj_t *input = ui->screen_wifi_connect_ta_input;

    // 键盘占据屏幕下半部，输入时收起说明和连接按钮，避免密码框被盖住。
    if (editing) {
        lv_obj_add_flag(ui->screen_wifi_connect_label_wifi, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(page->password_label, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(page->status, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(ui->screen_wifi_connect_btn_set, LV_OBJ_FLAG_HIDDEN);
        lv_obj_set_pos(ssid, 12, 47);
        lv_obj_set_size(ssid, 216, 30);
        lv_obj_set_style_pad_all(ssid, 4, 0);
        lv_obj_set_pos(input, 12, 82);
        lv_obj_set_pos(page->eye, 188, 82);
    } else {
        lv_obj_remove_flag(ui->screen_wifi_connect_label_wifi, LV_OBJ_FLAG_HIDDEN);
        lv_obj_remove_flag(page->password_label, LV_OBJ_FLAG_HIDDEN);
        lv_obj_remove_flag(page->status, LV_OBJ_FLAG_HIDDEN);
        lv_obj_remove_flag(ui->screen_wifi_connect_btn_set, LV_OBJ_FLAG_HIDDEN);
        lv_obj_set_pos(ssid, 12, 70);
        lv_obj_set_size(ssid, 216, 35);
        lv_obj_set_style_pad_all(ssid, 7, 0);
        lv_obj_set_pos(input, 12, 132);
        lv_obj_set_pos(page->eye, 188, 132);
    }
}

static void input_focus_cb(lv_event_t *event)
{
    wifi_connect_page_t *page = lv_event_get_user_data(event);
    lv_event_code_t code = lv_event_get_code(event);
    if (code == LV_EVENT_FOCUSED) set_keyboard_layout(page, true);
    else if (code == LV_EVENT_READY || code == LV_EVENT_DEFOCUSED)
        set_keyboard_layout(page, false);
}

static void style_label(lv_obj_t *label, int x, int y, int width, int height,
                        uint32_t color, const lv_font_t *font)
{
    lv_obj_set_pos(label, x, y);
    lv_obj_set_size(label, width, height);
    lv_obj_set_style_bg_opa(label, LV_OPA_TRANSP, 0);
    lv_obj_set_style_text_color(label, lv_color_hex(color), 0);
    lv_obj_set_style_text_font(label, font, 0);
}

static void connect_status_cb(lv_timer_t *timer)
{
    wifi_connect_page_t *page = lv_timer_get_user_data(timer);
    if (wifi_connect_is_connected()) {
        lv_label_set_text(page->status, "连接成功，右滑返回");
        lv_label_set_text(page->ui->screen_wifi_connect_btn_set_label, "已连接");
        lv_timer_delete(timer);
        page->timer = NULL;
        return;
    }

    if (++page->ticks >= 40) {
        lv_label_set_text(page->status, "连接未成功，请检查密码");
        lv_label_set_text(page->ui->screen_wifi_connect_btn_set_label, "重新连接");
        lv_obj_remove_state(page->ui->screen_wifi_connect_btn_set, LV_STATE_DISABLED);
        lv_timer_delete(timer);
        page->timer = NULL;
    }
}

static void connect_clicked_cb(lv_event_t *event)
{
    wifi_connect_page_t *page = lv_event_get_user_data(event);
    if (page->timer) return;

    lv_obj_t *keyboard = page->ui->g_kb_top_layer;
    if (keyboard) {
        lv_keyboard_set_textarea(keyboard, NULL);
        lv_obj_add_flag(keyboard, LV_OBJ_FLAG_HIDDEN);
    }

    esp_err_t err = wifi_connect_with_password(page->ui);
    if (err != ESP_OK) {
        lv_label_set_text(page->status, "连接请求失败，请重试");
        return;
    }

    page->ticks = 0;
    lv_label_set_text(page->status, "正在连接，请稍候…");
    lv_label_set_text(page->ui->screen_wifi_connect_btn_set_label, "连接中");
    lv_obj_add_state(page->ui->screen_wifi_connect_btn_set, LV_STATE_DISABLED);
    page->timer = lv_timer_create(connect_status_cb, 500, page);
}

static void eye_clicked_cb(lv_event_t *event)
{
    wifi_connect_page_t *page = lv_event_get_user_data(event);
    lv_obj_t *input = page->ui->screen_wifi_connect_ta_input;
    bool hidden = lv_textarea_get_password_mode(input);
    lv_textarea_set_password_mode(input, !hidden);
    lv_label_set_text(page->eye_label, hidden ? "隐藏" : "显示");
}

static void screen_deleted_cb(lv_event_t *event)
{
    wifi_connect_page_t *page = lv_event_get_user_data(event);
    if (page->timer) lv_timer_delete(page->timer);
    lv_obj_remove_event_cb_with_user_data(page->ui->screen_wifi_connect_ta_input,
                                          input_focus_cb, page);
    lv_obj_t *keyboard = page->ui->g_kb_top_layer;
    if (keyboard) {
        lv_keyboard_set_textarea(keyboard, NULL);
        lv_obj_add_flag(keyboard, LV_OBJ_FLAG_HIDDEN);
    }
    lv_free(page);
}

void wifi_connect_page_init(lv_ui *ui)
{
    if (!ui || !ui->screen_wifi_connect) return;

    lv_obj_t *screen = ui->screen_wifi_connect;
    lv_obj_set_style_bg_color(screen, lv_color_hex(0x090d15), 0);
    lv_obj_remove_flag(screen, LV_OBJ_FLAG_SCROLLABLE);

    lv_label_set_text(ui->screen_wifi_connect_label_connect, "连接 WiFi");
    style_label(ui->screen_wifi_connect_label_connect, 14, 13, 212, 28,
                0xffffff, &lv_customer_font_ZiTiQuanWeiJunHeiW22_18);

    lv_label_set_text(ui->screen_wifi_connect_label_wifi, "所选网络");
    style_label(ui->screen_wifi_connect_label_wifi, 14, 49, 200, 20,
                0xaac0d7, &lv_customer_font_ZiTiQuanWeiJunHeiW22_12);

    lv_obj_t *ssid = ui->screen_wifi_connect_label_wifiname;
    style_label(ssid, 12, 70, 216, 35, 0xffffff,
                &lv_customer_font_ZiTiQuanWeiJunHeiW22_18);
    lv_obj_set_style_bg_color(ssid, lv_color_hex(0x182a36), 0);
    lv_obj_set_style_bg_opa(ssid, LV_OPA_COVER, 0);
    lv_obj_set_style_radius(ssid, 9, 0);
    lv_obj_set_style_pad_all(ssid, 7, 0);
    lv_label_set_long_mode(ssid, LV_LABEL_LONG_DOT);

    lv_obj_t *password_label = lv_label_create(screen);
    lv_label_set_text(password_label, "密码");
    style_label(password_label, 14, 110, 180, 20, 0xaac0d7,
                &lv_customer_font_ZiTiQuanWeiJunHeiW22_12);

    lv_obj_t *input = ui->screen_wifi_connect_ta_input;
    lv_obj_set_pos(input, 12, 132);
    lv_obj_set_size(input, 170, 40);
    lv_obj_set_style_text_font(input, &lv_customer_font_ZiTiQuanWeiJunHeiW22_18, 0);
    lv_obj_set_style_text_color(input, lv_color_hex(0xffffff), 0);
    lv_obj_set_style_bg_color(input, lv_color_hex(0x18212d), 0);
    lv_obj_set_style_border_color(input, lv_color_hex(0x415773), 0);
    lv_obj_set_style_border_width(input, 1, 0);
    lv_obj_set_style_radius(input, 9, 0);
    lv_textarea_set_placeholder_text(input, "输入密码");
    lv_textarea_set_password_mode(input, true);
    lv_textarea_set_one_line(input, true);
    lv_textarea_set_accepted_chars(input, NULL);
    lv_textarea_set_max_length(input, 63);

    lv_obj_t *eye = lv_button_create(screen);
    lv_obj_set_pos(eye, 188, 132);
    lv_obj_set_size(eye, 40, 40);
    lv_obj_set_style_bg_color(eye, lv_color_hex(0x26364d), 0);
    lv_obj_set_style_radius(eye, 9, 0);
    lv_obj_set_style_shadow_width(eye, 0, 0);
    lv_obj_t *eye_label = lv_label_create(eye);
    lv_label_set_text(eye_label, "显示");
    lv_obj_set_style_text_font(eye_label, &lv_customer_font_ZiTiQuanWeiJunHeiW22_12, 0);
    lv_obj_set_style_text_color(eye_label, lv_color_hex(0xffffff), 0);
    lv_obj_center(eye_label);

    lv_obj_t *status = lv_label_create(screen);
    lv_label_set_text(status, "键盘确认后点击连接");
    style_label(status, 14, 184, 212, 32, 0xb9cde2,
                &lv_customer_font_ZiTiQuanWeiJunHeiW22_12);
    lv_label_set_long_mode(status, LV_LABEL_LONG_WRAP);

    lv_obj_t *button = ui->screen_wifi_connect_btn_set;
    lv_obj_set_pos(button, 12, 226);
    lv_obj_set_size(button, 216, 45);
    lv_obj_set_style_radius(button, 10, 0);
    lv_obj_set_style_bg_color(button, lv_color_hex(0x247fda), 0);
    lv_label_set_text(ui->screen_wifi_connect_btn_set_label, "连接");
    lv_obj_set_style_text_font(ui->screen_wifi_connect_btn_set_label,
                               &lv_customer_font_ZiTiQuanWeiJunHeiW22_18, 0);

    if (ui->g_kb_top_layer) {
        configure_wifi_password_keyboard(ui->g_kb_top_layer);
        lv_obj_set_height(ui->g_kb_top_layer, 156);
        lv_obj_align(ui->g_kb_top_layer, LV_ALIGN_BOTTOM_MID, 0, 0);
    }

    wifi_connect_page_t *page = lv_malloc(sizeof(*page));
    if (!page) return;
    *page = (wifi_connect_page_t){
        .ui = ui, .status = status, .eye_label = eye_label,
        .eye = eye, .password_label = password_label,
    };
    lv_obj_add_event_cb(button, connect_clicked_cb, LV_EVENT_CLICKED, page);
    lv_obj_add_event_cb(eye, eye_clicked_cb, LV_EVENT_CLICKED, page);
    lv_obj_add_event_cb(input, input_focus_cb, LV_EVENT_FOCUSED, page);
    lv_obj_add_event_cb(input, input_focus_cb, LV_EVENT_READY, page);
    lv_obj_add_event_cb(input, input_focus_cb, LV_EVENT_DEFOCUSED, page);
    lv_obj_add_event_cb(screen, screen_deleted_cb, LV_EVENT_DELETE, page);
}
