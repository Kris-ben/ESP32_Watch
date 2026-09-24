#include "voice_chat_popup.h"

#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "guider_customer_fonts.h"
#include "lvgl.h"

extern SemaphoreHandle_t lvgl_mutex;

static lv_obj_t *s_panel;
static lv_obj_t *s_bubble;
static lv_obj_t *s_status;
static lv_obj_t *s_question;
static lv_obj_t *s_answer;
static lv_obj_t *s_content;
static lv_obj_t *s_glint;
static lv_timer_t *s_glint_timer;
static int16_t s_glint_x = 12;
static int8_t s_glint_direction = 1;
static bool s_dismissed;

static void glint_timer_cb(lv_timer_t *timer)
{
    (void)timer;
    s_glint_x += s_glint_direction * 5;
    if (s_glint_x >= 166 || s_glint_x <= 12) {
        s_glint_direction = -s_glint_direction;
    }
    lv_obj_set_x(s_glint, s_glint_x);
}

static bool ui_lock(void)
{
    return lvgl_mutex && xSemaphoreTake(lvgl_mutex, pdMS_TO_TICKS(200)) == pdTRUE;
}

static void set_expanded(bool expanded)
{
    if (s_dismissed) return;
    if (expanded) {
        lv_obj_add_flag(s_bubble, LV_OBJ_FLAG_HIDDEN);
        lv_obj_remove_flag(s_panel, LV_OBJ_FLAG_HIDDEN);
        lv_obj_move_foreground(s_panel);
        lv_timer_resume(s_glint_timer);
    } else {
        lv_obj_add_flag(s_panel, LV_OBJ_FLAG_HIDDEN);
        lv_obj_remove_flag(s_bubble, LV_OBJ_FLAG_HIDDEN);
        lv_obj_move_foreground(s_bubble);
        lv_timer_pause(s_glint_timer);
    }
}

static void collapse_cb(lv_event_t *event)
{
    (void)event;
    set_expanded(false);
}

static void expand_cb(lv_event_t *event)
{
    (void)event;
    set_expanded(true);
}

static void dismiss_cb(lv_event_t *event)
{
    (void)event;
    s_dismissed = true;
    lv_obj_add_flag(s_panel, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(s_bubble, LV_OBJ_FLAG_HIDDEN);
    lv_timer_pause(s_glint_timer);
}

static lv_obj_t *make_text(lv_obj_t *parent, const char *text, uint32_t color)
{
    lv_obj_t *label = lv_label_create(parent);
    lv_obj_set_width(label, LV_PCT(100));
    lv_label_set_long_mode(label, LV_LABEL_LONG_WRAP);
    lv_label_set_text(label, text);
    lv_obj_set_style_text_font(label, &lv_customer_font_ZiTiQuanWeiJunHeiW22_12, 0);
    lv_obj_set_style_text_color(label, lv_color_hex(color), 0);
    lv_obj_set_style_text_opa(label, LV_OPA_COVER, 0);
    lv_obj_set_style_text_outline_stroke_color(label, lv_color_hex(0x07131d), 0);
    lv_obj_set_style_text_outline_stroke_width(label, 1, 0);
    lv_obj_set_style_text_outline_stroke_opa(label, 190, 0);
    return label;
}

static void create_popup(void)
{
    lv_obj_t *layer = lv_layer_top();
    s_panel = lv_obj_create(layer);
    lv_obj_set_pos(s_panel, 10, 42);
    lv_obj_set_size(s_panel, 220, 196);
    lv_obj_set_style_radius(s_panel, 14, 0);
    // 半透明冷色底与浅色边线保留页面轮廓，同时保证小屏幕文字对比度。
    lv_obj_set_style_bg_color(s_panel, lv_color_hex(0x29475c), 0);
    lv_obj_set_style_bg_grad_color(s_panel, lv_color_hex(0x101d2d), 0);
    lv_obj_set_style_bg_grad_dir(s_panel, LV_GRAD_DIR_VER, 0);
    lv_obj_set_style_bg_opa(s_panel, 105, 0);
    lv_obj_set_style_border_width(s_panel, 1, 0);
    lv_obj_set_style_border_color(s_panel, lv_color_hex(0xc6f0ff), 0);
    lv_obj_set_style_border_opa(s_panel, 185, 0);
    lv_obj_set_style_shadow_color(s_panel, lv_color_hex(0x07111c), 0);
    lv_obj_set_style_shadow_width(s_panel, 8, 0);
    lv_obj_set_style_shadow_opa(s_panel, 45, 0);
    lv_obj_set_style_pad_all(s_panel, 0, 0);
    lv_obj_remove_flag(s_panel, LV_OBJ_FLAG_SCROLLABLE);

    // 顶部游动的细高光只改变少量像素，呈现液态玻璃的光线流动。
    s_glint = lv_obj_create(s_panel);
    lv_obj_set_pos(s_glint, s_glint_x, 1);
    lv_obj_set_size(s_glint, 43, 2);
    lv_obj_set_style_radius(s_glint, LV_RADIUS_CIRCLE, 0);
    lv_obj_set_style_bg_color(s_glint, lv_color_hex(0xe5f9ff), 0);
    lv_obj_set_style_bg_opa(s_glint, 175, 0);
    lv_obj_set_style_border_width(s_glint, 0, 0);
    lv_obj_set_style_pad_all(s_glint, 0, 0);
    lv_obj_remove_flag(s_glint, LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_SCROLLABLE);
    s_glint_timer = lv_timer_create(glint_timer_cb, 120, NULL);
    lv_timer_pause(s_glint_timer);

    lv_obj_t *title = lv_label_create(s_panel);
    lv_obj_set_pos(title, 12, 10);
    lv_label_set_text(title, "小智问答");
    lv_obj_set_style_text_font(title, &lv_customer_font_ZiTiQuanWeiJunHeiW22_12, 0);
    lv_obj_set_style_text_color(title, lv_color_hex(0xe8f5ff), 0);
    lv_obj_set_style_text_opa(title, LV_OPA_COVER, 0);

    s_status = lv_label_create(s_panel);
    lv_obj_set_pos(s_status, 78, 10);
    lv_obj_set_width(s_status, 58);
    lv_label_set_text(s_status, "我在，请说");
    lv_obj_set_style_text_font(s_status, &lv_customer_font_ZiTiQuanWeiJunHeiW22_12, 0);
    lv_obj_set_style_text_color(s_status, lv_color_hex(0x65d9e7), 0);
    lv_obj_set_style_text_opa(s_status, LV_OPA_COVER, 0);

    lv_obj_t *collapse = lv_button_create(s_panel);
    lv_obj_set_pos(collapse, 139, 3);
    lv_obj_set_size(collapse, 48, 30);
    lv_obj_set_style_radius(collapse, 8, 0);
    lv_obj_set_style_bg_color(collapse, lv_color_hex(0x4c7287), 0);
    lv_obj_set_style_bg_opa(collapse, 90, 0);
    lv_obj_set_style_border_width(collapse, 1, 0);
    lv_obj_set_style_border_color(collapse, lv_color_hex(0xa9dbe8), 0);
    lv_obj_add_event_cb(collapse, collapse_cb, LV_EVENT_CLICKED, NULL);
    lv_obj_t *collapse_text = lv_label_create(collapse);
    lv_label_set_text(collapse_text, "收起");
    lv_obj_set_style_text_font(collapse_text, &lv_customer_font_ZiTiQuanWeiJunHeiW22_12, 0);
    lv_obj_set_style_text_color(collapse_text, lv_color_hex(0xe8f5ff), 0);
    lv_obj_set_style_text_opa(collapse_text, LV_OPA_COVER, 0);
    lv_obj_center(collapse_text);

    lv_obj_t *close = lv_button_create(s_panel);
    lv_obj_set_pos(close, 189, 3);
    lv_obj_set_size(close, 29, 30);
    lv_obj_set_style_radius(close, 7, 0);
    lv_obj_set_style_bg_color(close, lv_color_hex(0x4c7287), 0);
    lv_obj_set_style_bg_opa(close, 90, 0);
    lv_obj_set_style_border_width(close, 1, 0);
    lv_obj_set_style_border_color(close, lv_color_hex(0xa9dbe8), 0);
    lv_obj_add_event_cb(close, dismiss_cb, LV_EVENT_CLICKED, NULL);
    lv_obj_t *close_text = lv_label_create(close);
    lv_label_set_text(close_text, LV_SYMBOL_CLOSE);
    lv_obj_set_style_text_font(close_text, LV_FONT_DEFAULT, 0);
    lv_obj_set_style_text_color(close_text, lv_color_hex(0xe8f5ff), 0);
    lv_obj_set_style_text_opa(close_text, LV_OPA_COVER, 0);
    lv_obj_center(close_text);

    s_content = lv_obj_create(s_panel);
    lv_obj_set_pos(s_content, 9, 38);
    lv_obj_set_size(s_content, 202, 149);
    lv_obj_set_style_radius(s_content, 8, 0);
    lv_obj_set_style_bg_color(s_content, lv_color_hex(0x12273b), 0);
    lv_obj_set_style_bg_opa(s_content, 30, 0);
    lv_obj_set_style_border_width(s_content, 1, 0);
    lv_obj_set_style_border_color(s_content, lv_color_hex(0x9fd6e8), 0);
    lv_obj_set_style_border_opa(s_content, 65, 0);
    lv_obj_set_style_pad_all(s_content, 8, 0);
    lv_obj_set_style_pad_row(s_content, 5, 0);
    lv_obj_set_flex_flow(s_content, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_scroll_dir(s_content, LV_DIR_VER);
    lv_obj_set_scrollbar_mode(s_content, LV_SCROLLBAR_MODE_AUTO);

    make_text(s_content, "你说", 0x8fa9bd);
    s_question = make_text(s_content, "等待你说话…", 0xf1f6fc);
    make_text(s_content, "小智", 0x65d9e7);
    s_answer = make_text(s_content, "", 0xe0eaf4);

    s_bubble = lv_button_create(layer);
    lv_obj_set_pos(s_bubble, 196, 64);
    lv_obj_set_size(s_bubble, 38, 38);
    lv_obj_set_style_radius(s_bubble, LV_RADIUS_CIRCLE, 0);
    lv_obj_set_style_bg_color(s_bubble, lv_color_hex(0x23678a), 0);
    lv_obj_set_style_bg_opa(s_bubble, 130, 0);
    lv_obj_set_style_border_width(s_bubble, 2, 0);
    lv_obj_set_style_border_color(s_bubble, lv_color_hex(0x72dce9), 0);
    lv_obj_add_event_cb(s_bubble, expand_cb, LV_EVENT_CLICKED, NULL);
    lv_obj_t *bubble_text = lv_label_create(s_bubble);
    lv_label_set_text(bubble_text, "AI");
    lv_obj_set_style_text_color(bubble_text, lv_color_hex(0xffffff), 0);
    lv_obj_set_style_text_opa(bubble_text, LV_OPA_COVER, 0);
    lv_obj_center(bubble_text);
    lv_obj_add_flag(s_bubble, LV_OBJ_FLAG_HIDDEN);
}

void voice_chat_popup_wake(void)
{
    if (!ui_lock()) return;
    if (!s_panel) create_popup();
    s_dismissed = false;
    lv_label_set_text(s_status, "我在，请说");
    lv_label_set_text(s_question, "等待你说话…");
    lv_label_set_text(s_answer, "");
    lv_obj_scroll_to_y(s_content, 0, LV_ANIM_OFF);
    set_expanded(true);
    xSemaphoreGive(lvgl_mutex);
}

void voice_chat_popup_listening(void)
{
    if (!ui_lock()) return;
    if (s_panel && !s_dismissed) lv_label_set_text(s_status, "正在听");
    xSemaphoreGive(lvgl_mutex);
}

void voice_chat_popup_thinking(void)
{
    if (!ui_lock()) return;
    if (s_panel && !s_dismissed) lv_label_set_text(s_status, "正在思考");
    xSemaphoreGive(lvgl_mutex);
}

void voice_chat_popup_question(const char *text)
{
    if (!ui_lock()) return;
    if (s_panel && !s_dismissed) {
        lv_label_set_text(s_question, text && text[0] ? text : "未识别到内容");
        lv_label_set_text(s_answer, "正在整理回答…");
        lv_obj_scroll_to_y(s_content, 0, LV_ANIM_OFF);
    }
    xSemaphoreGive(lvgl_mutex);
}

void voice_chat_popup_answer(const char *text)
{
    if (!ui_lock()) return;
    if (s_panel && !s_dismissed) {
        lv_label_set_text(s_status, "正在回复");
        lv_label_set_text(s_answer, text && text[0] ? text : "暂无回复");
    }
    xSemaphoreGive(lvgl_mutex);
}

void voice_chat_popup_replying(void)
{
    if (!ui_lock()) return;
    if (s_panel && !s_dismissed) lv_label_set_text(s_status, "正在回复");
    xSemaphoreGive(lvgl_mutex);
}

void voice_chat_popup_session_end(void)
{
    if (!ui_lock()) return;
    if (s_panel && !s_dismissed) {
        lv_label_set_text(s_status, "已结束");
        set_expanded(false);
    }
    xSemaphoreGive(lvgl_mutex);
}
