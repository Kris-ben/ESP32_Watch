#include "app_launcher.h"

#include <stdint.h>

#include "events_init.h"
#include "guider_customer_fonts.h"

typedef enum {
    APP_AI,
    APP_WEATHER,
    APP_WIFI,
    APP_CALENDAR,
    APP_ALARM,
    APP_MUSIC,
    APP_COUNT
} app_id_t;

typedef struct {
    const char *name;
    const char *detail;
    const lv_image_dsc_t *icon;
    uint32_t accent;
} app_item_t;

static const app_item_t s_apps[APP_COUNT] = {
    {"AI",   "语音助手", &_AI_RGB565A8_77x55,      0xa07bff},
    {"天气", "查看预报", &_weather_RGB565A8_77x59, 0xf7bc63},
    {"WiFi", "连接网络", &_WiFi2_RGB565A8_77x59,   0x63b8ff},
    {"日历", "查看日期", &_rli_RGB565A8_77x55,     0x7dd9b0},
    {"闹钟", "定时提醒", &_clock_RGB565A8_77x64,   0xff8c9c},
    {"音乐", "本地播放", &_music_RGB565A8_77x66,   0x77c9ef},
};

static void open_app(app_id_t id)
{
    lv_ui *ui = &guider_ui;
    switch (id) {
    case APP_AI:
        ui_load_scr_animation(ui, &ui->screen_AI, ui->screen_AI_del,
                              &ui->screen_1_del, setup_scr_screen_AI,
                              LV_SCR_LOAD_ANIM_FADE_ON, 200, 0, false, true);
        break;
    case APP_WEATHER:
        ui_load_scr_animation(ui, &ui->screen_weather, ui->screen_weather_del,
                              &ui->screen_1_del, setup_scr_screen_weather,
                              LV_SCR_LOAD_ANIM_FADE_ON, 200, 0, false, true);
        break;
    case APP_WIFI:
        ui_load_scr_animation(ui, &ui->screen_wifi, ui->screen_wifi_del,
                              &ui->screen_1_del, setup_scr_screen_wifi,
                              LV_SCR_LOAD_ANIM_FADE_ON, 200, 0, false, true);
        break;
    case APP_CALENDAR:
        ui_load_scr_animation(ui, &ui->screen_Rli, ui->screen_Rli_del,
                              &ui->screen_1_del, setup_scr_screen_Rli,
                              LV_SCR_LOAD_ANIM_FADE_ON, 200, 0, false, true);
        break;
    case APP_ALARM:
        ui_load_scr_animation(ui, &ui->screen_clock, ui->screen_clock_del,
                              &ui->screen_1_del, setup_scr_screen_clock,
                              LV_SCR_LOAD_ANIM_FADE_ON, 200, 0, false, true);
        break;
    case APP_MUSIC:
        ui_load_scr_animation(ui, &ui->screen_musiclist, ui->screen_musiclist_del,
                              &ui->screen_1_del, setup_scr_screen_musiclist,
                              LV_SCR_LOAD_ANIM_FADE_ON, 200, 0, false, true);
        break;
    default:
        break;
    }
}

static void app_click_cb(lv_event_t *event)
{
    if (lv_event_get_code(event) != LV_EVENT_CLICKED) return;
    app_id_t id = (app_id_t)((uintptr_t)lv_event_get_user_data(event) - 1U);
    open_app(id);
}

static void create_app_card(lv_obj_t *list, app_id_t id)
{
    const app_item_t *item = &s_apps[id];
    lv_obj_t *card = lv_button_create(list);
    lv_obj_set_pos(card, 12, 8 + (int)id * 72);
    lv_obj_set_size(card, 216, 64);
    lv_obj_set_style_radius(card, 12, 0);
    lv_obj_set_style_bg_color(card, lv_color_hex(0x18212d), 0);
    lv_obj_set_style_bg_color(card, lv_color_hex(0x2b4058), LV_STATE_PRESSED);
    lv_obj_set_style_border_color(card, lv_color_hex(0x2b3b4d), 0);
    lv_obj_set_style_border_width(card, 1, 0);
    lv_obj_set_style_border_color(card, lv_color_hex(0x6aaeff), LV_STATE_PRESSED);
    lv_obj_set_style_border_width(card, 2, LV_STATE_PRESSED);
    lv_obj_set_style_shadow_width(card, 0, 0);
    lv_obj_set_style_pad_all(card, 0, 0);
    lv_obj_remove_flag(card, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_add_flag(card, LV_OBJ_FLAG_GESTURE_BUBBLE);
    lv_obj_add_event_cb(card, app_click_cb, LV_EVENT_CLICKED,
                        (void *)(uintptr_t)((unsigned)id + 1U));

    lv_obj_t *accent = lv_obj_create(card);
    lv_obj_set_pos(accent, 9, 10);
    lv_obj_set_size(accent, 43, 43);
    lv_obj_set_style_radius(accent, 10, 0);
    lv_obj_set_style_bg_color(accent, lv_color_hex(item->accent), 0);
    lv_obj_set_style_bg_opa(accent, LV_OPA_20, 0);
    lv_obj_set_style_border_width(accent, 0, 0);
    lv_obj_set_style_pad_all(accent, 0, 0);
    lv_obj_remove_flag(accent, LV_OBJ_FLAG_SCROLLABLE | LV_OBJ_FLAG_CLICKABLE);

    lv_obj_t *icon = lv_image_create(card);
    lv_image_set_src(icon, item->icon);
    lv_image_set_pivot(icon, 0, 0);
    lv_image_set_scale(icon, 140);
    lv_obj_set_pos(icon, 9, 14);

    lv_obj_t *name = lv_label_create(card);
    lv_label_set_text(name, item->name);
    lv_obj_set_pos(name, 76, 9);
    lv_obj_set_style_text_font(name, &lv_customer_font_ZiTiQuanWeiJunHeiW22_18, 0);
    lv_obj_set_style_text_color(name, lv_color_hex(0xf4f7fc), 0);

    lv_obj_t *detail = lv_label_create(card);
    lv_label_set_text(detail, item->detail);
    lv_obj_set_pos(detail, 76, 37);
    lv_obj_set_style_text_font(detail, &lv_customer_font_ZiTiQuanWeiJunHeiW22_12, 0);
    lv_obj_set_style_text_color(detail, lv_color_hex(0xaab7c8), 0);

    lv_obj_t *arrow = lv_label_create(card);
    lv_label_set_text(arrow, LV_SYMBOL_RIGHT);
    lv_obj_set_pos(arrow, 194, 22);
    lv_obj_set_style_text_color(arrow, lv_color_hex(0x91a4ba), 0);
}

void app_launcher_create(lv_ui *ui)
{
    ui->screen_1 = lv_obj_create(NULL);
    lv_obj_set_size(ui->screen_1, 240, 284);
    lv_obj_set_style_bg_color(ui->screen_1, lv_color_hex(0x090d15), 0);
    lv_obj_remove_flag(ui->screen_1, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_t *title = lv_label_create(ui->screen_1);
    lv_label_set_text(title, "应用");
    lv_obj_set_pos(title, 12, 13);
    lv_obj_set_style_text_font(title, &lv_customer_font_ZiTiQuanWeiJunHeiW22_18, 0);
    lv_obj_set_style_text_color(title, lv_color_hex(0xf4f7fc), 0);

    lv_obj_t *hint = lv_label_create(ui->screen_1);
    lv_label_set_text(hint, "右滑返回");
    lv_obj_set_pos(hint, 162, 17);
    lv_obj_set_style_text_font(hint, &lv_customer_font_ZiTiQuanWeiJunHeiW22_12, 0);
    lv_obj_set_style_text_color(hint, lv_color_hex(0x8190a6), 0);

    lv_obj_t *list = lv_obj_create(ui->screen_1);
    lv_obj_set_pos(list, 0, 54);
    lv_obj_set_size(list, 240, 230);
    lv_obj_set_style_bg_opa(list, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(list, 0, 0);
    lv_obj_set_style_radius(list, 0, 0);
    lv_obj_set_style_pad_all(list, 0, 0);
    lv_obj_set_style_bg_color(list, lv_color_hex(0x6aaeff), LV_PART_SCROLLBAR);
    lv_obj_set_style_width(list, 3, LV_PART_SCROLLBAR);
    lv_obj_set_scroll_dir(list, LV_DIR_VER);
    lv_obj_set_scrollbar_mode(list, LV_SCROLLBAR_MODE_AUTO);
    lv_obj_add_flag(list, LV_OBJ_FLAG_GESTURE_BUBBLE);

    for (int i = 0; i < APP_COUNT; ++i) create_app_card(list, (app_id_t)i);
    events_init_screen_1(ui);
}
