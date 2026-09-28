#include "music_page.h"

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <time.h>

#include "app_swipe_nav.h"
#include "battery_monitor.h"
#include "battery_ui.h"
#include "music_player.h"
#include "sd_card_fs.h"
#include "system_settings.h"
#include "guider_customer_fonts.h"

#define MUSIC_MAX_FILES 32
#define BG 0x11161f
#define CARD 0x202e3b
#define MUTED 0xaab9c9
#define ACCENT 0x80dedb

// “枫”来自 LVGL 依赖自带的 NotoSansSC-Regular.ttf，许可见 music_font_OFL.txt。
static const uint8_t s_missing_bitmap[] = {
    0x00, 0x01, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0c, 0x80,
    0x02, 0x32, 0x22, 0x22, 0x31, 0x00, 0x00, 0x0c, 0x80, 0x0c, 0xff, 0xff,
    0xff, 0xf6, 0x00, 0x00, 0x0c, 0x80, 0x0c, 0x81, 0x11, 0x11, 0xc6, 0x00,
    0x0a, 0xae, 0xca, 0x5c, 0x70, 0x00, 0x46, 0xc6, 0x00, 0x09, 0x9f, 0xc9,
    0x5c, 0x8b, 0x10, 0x99, 0xc6, 0x00, 0x00, 0x1f, 0xb0, 0x0c, 0x7b, 0x70,
    0xd5, 0xc6, 0x00, 0x00, 0x5f, 0xf6, 0x0c, 0x73, 0xe4, 0xf1, 0xc6, 0x00,
    0x00, 0xaf, 0xbe, 0x2c, 0x70, 0xae, 0xa0, 0xb6, 0x00, 0x01, 0xed, 0x89,
    0xac, 0x60, 0x3f, 0x50, 0xb6, 0x00, 0x07, 0xbc, 0x82, 0x5d, 0x50, 0x5f,
    0x90, 0xb7, 0x00, 0x1e, 0x5c, 0x80, 0x0f, 0x40, 0xcb, 0xf1, 0xa7, 0x00,
    0x3d, 0x0c, 0x80, 0x2f, 0x25, 0xe1, 0xc8, 0x98, 0x00, 0x02, 0x0c, 0x80,
    0x5e, 0x1d, 0x80, 0x5e, 0x89, 0x00, 0x00, 0x0c, 0x80, 0xaa, 0x9d, 0x10,
    0x0c, 0x7b, 0x62, 0x00, 0x0c, 0x82, 0xf5, 0x33, 0x00, 0x00, 0x3e, 0xc3,
    0x00, 0x0c, 0x88, 0xd0, 0x00, 0x00, 0x00, 0x0c, 0xd0, 0x00, 0x05, 0x30,
    0x30, 0x00, 0x00, 0x00, 0x01, 0x10,
};
static const lv_font_fmt_txt_glyph_dsc_t s_missing_glyphs[] = {
    {0},
    {.bitmap_index = 0, .adv_w = 288, .box_w = 18, .box_h = 18, .ofs_x = 0, .ofs_y = -3},
};
static const lv_font_fmt_txt_cmap_t s_missing_cmaps[] = {
    {.range_start = 0x67ab, .range_length = 1, .glyph_id_start = 1,
     .unicode_list = NULL, .glyph_id_ofs_list = NULL, .list_length = 0,
     .type = LV_FONT_FMT_TXT_CMAP_FORMAT0_TINY},
};
static const lv_font_fmt_txt_dsc_t s_missing_dsc = {
    .glyph_bitmap = s_missing_bitmap,
    .glyph_dsc = s_missing_glyphs,
    .cmaps = s_missing_cmaps,
    .cmap_num = 1,
    .bpp = 4,
    .bitmap_format = 0,
};
static const lv_font_t s_missing_font = {
    .get_glyph_dsc = lv_font_get_glyph_dsc_fmt_txt,
    .get_glyph_bitmap = lv_font_get_bitmap_fmt_txt,
    .line_height = 18,
    .base_line = 2,
    .dsc = &s_missing_dsc,
};
static lv_font_t s_large_music_font;
static bool s_large_music_font_ready;

static char s_files[MUSIC_MAX_FILES][SD_MUSIC_NAME_MAX];
static int s_count;
static int s_selected = -1;
static lv_obj_t *s_title;
static lv_obj_t *s_format;
static lv_obj_t *s_status;
static lv_obj_t *s_elapsed;
static lv_obj_t *s_duration;
static lv_obj_t *s_progress;
static lv_obj_t *s_volume;
static lv_obj_t *s_volume_panel;
static lv_obj_t *s_pause;
static lv_obj_t *s_clock;
static lv_obj_t *s_battery;
static lv_timer_t *s_player_timer;
static lv_timer_t *s_status_timer;
static bool s_dragging;

static lv_obj_t *label(lv_obj_t *parent, int x, int y, int w, int h,
                       const char *text, bool large, uint32_t color)
{
    lv_obj_t *obj = lv_label_create(parent);
    lv_obj_set_pos(obj, x, y);
    lv_obj_set_size(obj, w, h);
    lv_label_set_text(obj, text);
    lv_label_set_long_mode(obj, LV_LABEL_LONG_DOT);
    lv_obj_set_style_text_color(obj, lv_color_hex(color), 0);
    if (!s_large_music_font_ready) {
        s_large_music_font = lv_customer_font_ZiTiQuanWeiJunHeiW22_18;
        s_large_music_font.fallback = &s_missing_font;
        s_large_music_font_ready = true;
    }
    // LVGL 内置图标使用默认字体；歌名中的缺字由小字形补齐。
    lv_obj_set_style_text_font(obj, (unsigned char)text[0] == 0xef ? LV_FONT_DEFAULT
                              : (large ? &s_large_music_font
                                       : &lv_customer_font_ZiTiQuanWeiJunHeiW22_12), 0);
    lv_obj_add_flag(obj, LV_OBJ_FLAG_GESTURE_BUBBLE);
    return obj;
}

static void box(lv_obj_t *obj, int x, int y, int w, int h, uint32_t color, int radius)
{
    lv_obj_set_pos(obj, x, y);
    lv_obj_set_size(obj, w, h);
    lv_obj_set_style_bg_color(obj, lv_color_hex(color), 0);
    lv_obj_set_style_bg_opa(obj, LV_OPA_COVER, 0);
    lv_obj_set_style_border_width(obj, 0, 0);
    lv_obj_set_style_radius(obj, radius, 0);
    lv_obj_set_style_pad_all(obj, 0, 0);
    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_add_flag(obj, LV_OBJ_FLAG_GESTURE_BUBBLE);
}

static const char *extension(const char *name)
{
    const char *dot = strrchr(name, '.');
    return dot ? dot + 1 : "";
}

static void display_name(char *out, size_t size, const char *filename)
{
    snprintf(out, size, "%s", filename);
    char *dot = strrchr(out, '.');
    if (dot) *dot = '\0';
}

static void format_time(char *out, size_t size, uint32_t ms)
{
    uint32_t sec = ms / 1000;
    snprintf(out, size, "%02lu:%02lu", (unsigned long)(sec / 60),
             (unsigned long)(sec % 60));
}

static void status_tick(lv_timer_t *timer)
{
    (void)timer;
    time_t now = time(NULL);
    struct tm local;
    if (localtime_r(&now, &local)) {
        lv_label_set_text_fmt(s_clock, "%02d:%02d", local.tm_hour, local.tm_min);
    }
    battery_info_t info;
    if (battery_get_info(&info) && info.percentage >= 0) {
        lv_label_set_text_fmt(s_battery, "%s%d%%",
                              battery_ui_symbol_for_percentage(info.percentage),
                              info.percentage);
    } else {
        lv_label_set_text(s_battery, "--");
    }
}

static void screen_deleted(lv_event_t *event)
{
    lv_obj_t *screen = lv_event_get_target(event);
    if (s_clock && lv_obj_get_parent(s_clock) == screen && s_status_timer) {
        lv_timer_del(s_status_timer);
        s_status_timer = NULL;
        s_clock = s_battery = NULL;
    }
    if (screen == guider_ui.screen_play && s_player_timer) {
        lv_timer_del(s_player_timer);
        s_player_timer = NULL;
        s_title = s_format = s_status = s_elapsed = s_duration = NULL;
        s_progress = s_volume = s_volume_panel = s_pause = NULL;
    }
}

static lv_obj_t *make_screen(lv_obj_t **screen)
{
    if (s_status_timer) {
        lv_timer_del(s_status_timer);
        s_status_timer = NULL;
    }
    *screen = lv_obj_create(NULL);
    box(*screen, 0, 0, 240, 284, BG, 0);
    // 顶层页面必须接住手势；若继续向父级传递，LVGL 会走到空父级而丢弃手势。
    lv_obj_remove_flag(*screen, LV_OBJ_FLAG_GESTURE_BUBBLE);
    lv_obj_add_event_cb(*screen, screen_deleted, LV_EVENT_DELETE, NULL);
    s_clock = label(*screen, 15, 9, 75, 19, "--:--", false, MUTED);
    s_battery = label(*screen, 165, 9, 65, 19, "--", false, MUTED);
    lv_obj_set_style_text_font(s_battery, &lv_font_montserrat_14, 0);
    lv_obj_set_style_text_align(s_battery, LV_TEXT_ALIGN_RIGHT, 0);
    s_status_timer = lv_timer_create(status_tick, 1000, NULL);
    status_tick(NULL);
    return *screen;
}

static void open_player(void)
{
    lv_ui *ui = &guider_ui;
    ui_load_scr_animation(ui, &ui->screen_play, ui->screen_play_del,
                          &ui->screen_musiclist_del, setup_scr_screen_play,
                          LV_SCR_LOAD_ANIM_FADE_ON, 200, 200, false, true);
}

static void item_clicked(lv_event_t *event)
{
    int index = (int)(intptr_t)lv_event_get_user_data(event);
    if (index < 0 || index >= s_count) return;
    if (music_player_set_selected_file(s_files[index]) != ESP_OK) return;
    s_selected = index;
    open_player();
}

void music_page_create_list(lv_ui *ui)
{
    lv_obj_t *screen = make_screen(&ui->screen_musiclist);
    label(screen, 15, 35, 170, 28, "本地音乐", true, 0xf2f7ff);
    lv_obj_t *tag = lv_obj_create(screen);
    box(tag, 180, 36, 47, 24, CARD, 12);
    label(tag, 8, 4, 36, 15, "SD卡", false, ACCENT);

    s_count = sd_card_fs_get_music_files(s_files, MUSIC_MAX_FILES);
    lv_obj_t *summary = label(screen, 15, 67, 210, 18, "", false, MUTED);
    lv_label_set_text_fmt(summary, "找到 %d 首 · 点一首开始播放", s_count);

    lv_obj_t *list = lv_obj_create(screen);
    lv_obj_set_pos(list, 8, 91);
    lv_obj_set_size(list, 224, 185);
    lv_obj_set_style_bg_opa(list, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(list, 0, 0);
    lv_obj_set_style_pad_all(list, 0, 0);
    lv_obj_set_style_pad_row(list, 8, 0);
    lv_obj_set_scroll_dir(list, LV_DIR_VER);
    lv_obj_set_scrollbar_mode(list, LV_SCROLLBAR_MODE_OFF);

    if (s_count == 0) {
        label(list, 12, 20, 200, 35, "SD卡里没有可播放的歌曲", false, MUTED);
    }
    for (int i = 0; i < s_count; ++i) {
        int y = i * 57;
        lv_obj_t *card = lv_obj_create(list);
        box(card, 0, y, 224, 51, CARD, 12);
        lv_obj_add_flag(card, LV_OBJ_FLAG_CLICKABLE);
        lv_obj_set_style_bg_color(card, lv_color_hex(0x345262), LV_STATE_PRESSED);
        lv_obj_set_style_border_color(card, lv_color_hex(ACCENT), LV_STATE_PRESSED);
        lv_obj_set_style_border_width(card, 1, LV_STATE_PRESSED);
        lv_obj_t *num = lv_obj_create(card);
        box(num, 7, 6, 40, 39, 0x294551, 9);
        lv_obj_remove_flag(num, LV_OBJ_FLAG_CLICKABLE);
        lv_obj_t *num_label = label(num, 6, 10, 30, 20, "", true, ACCENT);
        lv_label_set_text_fmt(num_label, "%02d", i + 1);
        char name[SD_MUSIC_NAME_MAX];
        display_name(name, sizeof(name), s_files[i]);
        label(card, 54, 6, 142, 22, name, true, 0xf2f7ff);
        label(card, 54, 30, 120, 16, extension(s_files[i]), false, MUTED);
        label(card, 201, 16, 18, 19, LV_SYMBOL_RIGHT, false, ACCENT);
        lv_obj_add_event_cb(card, item_clicked, LV_EVENT_CLICKED, (void *)(intptr_t)i);
    }
    app_swipe_nav_bind(screen, NULL, APP_SWIPE_TO_LAUNCHER);
}

static void refresh_player(lv_timer_t *timer)
{
    (void)timer;
    if (!s_progress) return;
    bool playing = music_player_is_playing();
    bool paused = music_player_is_paused();
    lv_label_set_text(s_status, paused ? "已暂停" : (playing ? "正在播放" : "播放结束"));
    lv_label_set_text(s_pause, playing && !paused ? LV_SYMBOL_PAUSE : LV_SYMBOL_PLAY);
    if (!s_dragging) lv_slider_set_value(s_progress, music_player_get_progress_percent(), LV_ANIM_OFF);
    uint32_t duration = music_player_get_duration_ms();
    uint32_t elapsed = music_player_get_played_ms();
    char text[16];
    format_time(text, sizeof(text), elapsed);
    lv_label_set_text(s_elapsed, text);
    if (duration) {
        format_time(text, sizeof(text), duration);
        lv_label_set_text(s_duration, text);
    } else {
        lv_label_set_text(s_duration, "--:--");
    }
}

static void play_clicked(lv_event_t *event)
{
    (void)event;
    (void)music_player_toggle_pause();
    refresh_player(NULL);
}

static void change_song(lv_event_t *event)
{
    int step = (int)(intptr_t)lv_event_get_user_data(event);
    if (s_count == 0 || s_selected < 0) return;
    int next = (s_selected + step + s_count) % s_count;
    if (music_player_set_selected_file(s_files[next]) != ESP_OK) return;
    s_selected = next;
    char name[SD_MUSIC_NAME_MAX];
    display_name(name, sizeof(name), s_files[next]);
    lv_label_set_text(s_title, name);
    lv_label_set_text_fmt(s_format, "本地音频 · %s", extension(s_files[next]));
    lv_label_set_text(s_duration, "--:--");
    if (music_player_request_play_selected() == ESP_OK) refresh_player(NULL);
}

static void seek_event(lv_event_t *event)
{
    lv_event_code_t code = lv_event_get_code(event);
    if (code == LV_EVENT_PRESSED) s_dragging = true;
    if (code == LV_EVENT_RELEASED || code == LV_EVENT_PRESS_LOST) {
        int value = lv_slider_get_value(s_progress);
        s_dragging = false;
        if (music_player_get_duration_ms()) (void)music_player_seek_percent((uint8_t)value);
    }
}

static void volume_event(lv_event_t *event)
{
    int value = lv_slider_get_value(s_volume);
    if (lv_event_get_code(event) == LV_EVENT_VALUE_CHANGED) {
        (void)system_preview_volume((uint8_t)value);
    } else if (lv_event_get_code(event) == LV_EVENT_RELEASED) {
        (void)system_set_volume((uint8_t)value);
    }
}

static void back_clicked(lv_event_t *event)
{
    (void)event;
    app_swipe_nav_go_back(guider_ui.screen_play, APP_SWIPE_TO_MUSICLIST);
}

static void volume_clicked(lv_event_t *event)
{
    (void)event;
    if (lv_obj_has_flag(s_volume_panel, LV_OBJ_FLAG_HIDDEN)) {
        lv_obj_remove_flag(s_volume_panel, LV_OBJ_FLAG_HIDDEN);
    } else {
        lv_obj_add_flag(s_volume_panel, LV_OBJ_FLAG_HIDDEN);
    }
}

static lv_obj_t *round_button(lv_obj_t *parent, int x, int y, int size,
                              uint32_t color, const char *symbol, lv_obj_t **text)
{
    lv_obj_t *btn = lv_button_create(parent);
    box(btn, x, y, size, size, color, size / 2);
    lv_obj_set_style_bg_color(btn, lv_color_hex(color == ACCENT ? 0xa4f1ed : 0x3a5867),
                              LV_STATE_PRESSED);
    *text = label(btn, 0, 0, size, size, symbol, true, 0xf2f7ff);
    lv_obj_set_size(*text, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    lv_obj_center(*text);
    return btn;
}

static lv_obj_t *playback_button(lv_obj_t *parent, int x, int width, int circle_size,
                                 uint32_t color, const char *symbol, lv_obj_t **text)
{
    lv_obj_t *hit = lv_button_create(parent);
    lv_obj_set_pos(hit, x, 226);
    lv_obj_set_size(hit, width, 56);
    lv_obj_set_style_bg_opa(hit, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(hit, 0, 0);
    lv_obj_set_style_shadow_width(hit, 0, 0);
    lv_obj_set_style_pad_all(hit, 0, 0);

    lv_obj_t *circle = lv_obj_create(hit);
    box(circle, (width - circle_size) / 2, (56 - circle_size) / 2,
        circle_size, circle_size, color, circle_size / 2);
    lv_obj_remove_flag(circle, LV_OBJ_FLAG_CLICKABLE);
    *text = label(circle, 0, 0, circle_size, circle_size, symbol, true,
                  color == ACCENT ? BG : 0xf2f7ff);
    lv_obj_set_style_text_font(*text, &lv_font_montserrat_24, 0);
    lv_obj_set_size(*text, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    lv_obj_center(*text);
    lv_obj_remove_flag(*text, LV_OBJ_FLAG_CLICKABLE);
    return hit;
}

void music_page_create_player(lv_ui *ui)
{
    lv_obj_t *screen = make_screen(&ui->screen_play);
    lv_obj_t *txt;
    lv_obj_t *back = round_button(screen, 12, 35, 30, CARD, LV_SYMBOL_LEFT, &txt);
    lv_obj_t *volume = round_button(screen, 198, 35, 30, CARD, LV_SYMBOL_VOLUME_MAX, &txt);
    lv_obj_add_event_cb(back, back_clicked, LV_EVENT_CLICKED, NULL);
    lv_obj_add_event_cb(volume, volume_clicked, LV_EVENT_CLICKED, NULL);
    s_status = label(screen, 54, 32, 132, 23, "正在播放", true, 0xf2f7ff);
    lv_obj_set_style_text_align(s_status, LV_TEXT_ALIGN_CENTER, 0);

    lv_obj_t *disc = lv_obj_create(screen);
    box(disc, 69, 57, 102, 102, 0x1e3a42, 51);
    lv_obj_set_style_border_color(disc, lv_color_hex(ACCENT), 0);
    lv_obj_set_style_border_width(disc, 3, 0);
    lv_obj_t *inner = lv_obj_create(disc);
    box(inner, 14, 14, 68, 68, 0x244a52, 34);
    lv_obj_t *music_icon = label(inner, 0, 0, 68, 68, LV_SYMBOL_AUDIO, true, ACCENT);
    lv_obj_set_style_text_font(music_icon, &lv_font_montserrat_24, 0);
    lv_obj_set_size(music_icon, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    lv_obj_center(music_icon);

    char name[SD_MUSIC_NAME_MAX] = "未选择歌曲";
    const char *path = music_player_get_selected_file();
    if (s_selected >= 0 && s_selected < s_count) display_name(name, sizeof(name), s_files[s_selected]);
    else if (path && path[0]) display_name(name, sizeof(name), strrchr(path, '/') ? strrchr(path, '/') + 1 : path);
    s_title = label(screen, 19, 160, 202, 24, name, true, 0xf2f7ff);
    lv_obj_set_style_text_align(s_title, LV_TEXT_ALIGN_CENTER, 0);
    s_format = label(screen, 40, 183, 160, 17, "本地音频", false, MUTED);
    lv_obj_set_style_text_align(s_format, LV_TEXT_ALIGN_CENTER, 0);
    if (s_selected >= 0 && s_selected < s_count) {
        lv_label_set_text_fmt(s_format, "本地音频 · %s", extension(s_files[s_selected]));
    }

    s_progress = lv_slider_create(screen);
    lv_obj_set_pos(s_progress, 19, 201);
    lv_obj_set_size(s_progress, 202, 7);
    lv_slider_set_range(s_progress, 0, 100);
    lv_obj_set_style_bg_color(s_progress, lv_color_hex(0x526474), LV_PART_MAIN);
    lv_obj_set_style_bg_opa(s_progress, LV_OPA_COVER, LV_PART_MAIN);
    lv_obj_set_style_bg_color(s_progress, lv_color_hex(ACCENT), LV_PART_INDICATOR);
    lv_obj_set_style_bg_opa(s_progress, LV_OPA_COVER, LV_PART_INDICATOR);
    lv_obj_set_style_bg_color(s_progress, lv_color_hex(ACCENT), LV_PART_KNOB);
    lv_obj_set_style_bg_opa(s_progress, LV_OPA_COVER, LV_PART_KNOB);
    lv_obj_add_event_cb(s_progress, seek_event, LV_EVENT_PRESSED, NULL);
    lv_obj_add_event_cb(s_progress, seek_event, LV_EVENT_RELEASED, NULL);
    lv_obj_add_event_cb(s_progress, seek_event, LV_EVENT_PRESS_LOST, NULL);
    s_elapsed = label(screen, 19, 209, 50, 16, "00:00", false, MUTED);
    s_duration = label(screen, 170, 209, 51, 16, "--:--", false, MUTED);
    lv_obj_set_style_text_align(s_duration, LV_TEXT_ALIGN_RIGHT, 0);

    lv_obj_t *prev = playback_button(screen, 2, 76, 54, CARD, LV_SYMBOL_PREV, &txt);
    lv_obj_t *play = playback_button(screen, 80, 80, 56, ACCENT, LV_SYMBOL_PAUSE, &s_pause);
    lv_obj_t *next = playback_button(screen, 162, 76, 54, CARD, LV_SYMBOL_NEXT, &txt);
    lv_obj_add_event_cb(prev, change_song, LV_EVENT_PRESSED, (void *)(intptr_t)-1);
    lv_obj_add_event_cb(play, play_clicked, LV_EVENT_PRESSED, NULL);
    lv_obj_add_event_cb(next, change_song, LV_EVENT_PRESSED, (void *)(intptr_t)1);

    // 竖向音量面板避开右滑方向，面板内的手势由面板自己接住。
    s_volume_panel = lv_obj_create(screen);
    box(s_volume_panel, 185, 70, 43, 139, CARD, 12);
    lv_obj_t *volume_close = round_button(s_volume_panel, 9, 6, 26, 0x294551,
                                          LV_SYMBOL_CLOSE, &txt);
    lv_obj_add_event_cb(volume_close, volume_clicked, LV_EVENT_CLICKED, NULL);
    s_volume = lv_slider_create(s_volume_panel);
    lv_obj_set_pos(s_volume, 15, 43);
    lv_obj_set_size(s_volume, 13, 85);
    lv_slider_set_orientation(s_volume, LV_SLIDER_ORIENTATION_VERTICAL);
    lv_slider_set_range(s_volume, 0, 100);
    lv_slider_set_value(s_volume, music_player_get_volume(), LV_ANIM_OFF);
    lv_obj_set_style_bg_color(s_volume, lv_color_hex(0x526474), LV_PART_MAIN);
    lv_obj_set_style_bg_opa(s_volume, LV_OPA_COVER, LV_PART_MAIN);
    lv_obj_set_style_bg_color(s_volume, lv_color_hex(ACCENT), LV_PART_INDICATOR);
    lv_obj_set_style_bg_opa(s_volume, LV_OPA_COVER, LV_PART_INDICATOR);
    lv_obj_set_style_bg_color(s_volume, lv_color_hex(ACCENT), LV_PART_KNOB);
    lv_obj_set_style_bg_opa(s_volume, LV_OPA_COVER, LV_PART_KNOB);
    lv_obj_add_event_cb(s_volume, volume_event, LV_EVENT_VALUE_CHANGED, NULL);
    lv_obj_add_event_cb(s_volume, volume_event, LV_EVENT_RELEASED, NULL);
    lv_obj_add_flag(s_volume_panel, LV_OBJ_FLAG_HIDDEN);

    app_swipe_nav_bind(screen, NULL, APP_SWIPE_TO_MUSICLIST);
    lv_obj_remove_flag(s_volume_panel, LV_OBJ_FLAG_GESTURE_BUBBLE);
    if (path && path[0]) (void)music_player_request_play_selected();
    else lv_label_set_text(s_status, "未选择歌曲");
    s_player_timer = lv_timer_create(refresh_player, 300, NULL);
    refresh_player(NULL);
}
