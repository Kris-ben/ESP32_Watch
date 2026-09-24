#include "ai_status_page.h"

#include <stdint.h>
#include <stdlib.h>

#include "guider_customer_fonts.h"
#include "voice_assistant.h"
#include "wifi_connect.h"

typedef struct {
    lv_timer_t *timer;
    lv_obj_t *ring;
    lv_obj_t *bars[5];
    lv_obj_t *status;
    lv_obj_t *hint;
    lv_obj_t *wifi_icon;
    voice_dialog_state_t state;
    uint8_t phase;
    bool wifi_connected;
    bool wifi_initialized;
} ai_motion_t;

static lv_color_t state_color(voice_dialog_state_t state)
{
    if (state == VOICE_DIALOG_THINKING || state == VOICE_DIALOG_RECOGNIZING)
        return lv_color_hex(0xe9bc75);
    if (state == VOICE_DIALOG_SPEAKING) return lv_color_hex(0x80b8ff);
    if (state == VOICE_DIALOG_ERROR) return lv_color_hex(0xf28585);
    return lv_color_hex(0x61d9cd);
}

static const char *state_text(voice_dialog_state_t state)
{
    switch (state) {
    case VOICE_DIALOG_LISTENING:   return "等你叫我";
    case VOICE_DIALOG_WAKEUP:
    case VOICE_DIALOG_RECORDING:   return "正在听你说";
    case VOICE_DIALOG_RECOGNIZING:
    case VOICE_DIALOG_THINKING:    return "正在思考";
    case VOICE_DIALOG_SPEAKING:    return "正在回答";
    case VOICE_DIALOG_ERROR:       return "暂时不可用";
    default:                       return "准备中";
    }
}

static const char *state_hint(voice_dialog_state_t state)
{
    switch (state) {
    case VOICE_DIALOG_LISTENING:   return "说“小智小智”开始";
    case VOICE_DIALOG_WAKEUP:
    case VOICE_DIALOG_RECORDING:   return "说完稍等，我会自动识别";
    case VOICE_DIALOG_RECOGNIZING:
    case VOICE_DIALOG_THINKING:    return "已听到你的问题";
    case VOICE_DIALOG_SPEAKING:    return "喊“小智”可以打断播报";
    case VOICE_DIALOG_ERROR:       return "请检查网络后重试";
    default:                       return "正在启动语音助手";
    }
}

static bool state_is_active(voice_dialog_state_t state)
{
    return state == VOICE_DIALOG_WAKEUP || state == VOICE_DIALOG_RECORDING ||
           state == VOICE_DIALOG_RECOGNIZING || state == VOICE_DIALOG_THINKING ||
           state == VOICE_DIALOG_SPEAKING;
}

static void motion_draw_bars(ai_motion_t *motion, bool recolor)
{
    static const uint8_t heights[4][5] = {
        {12, 24, 34, 21, 13}, {20, 36, 15, 31, 21},
        {15, 22, 38, 24, 12}, {29, 16, 27, 34, 17},
    };
    voice_dialog_state_t state = motion->state;
    lv_color_t color = state_color(state);

    for (int i = 0; i < 5; ++i) {
        lv_obj_t *bar = motion->bars[i];
        if (state == VOICE_DIALOG_THINKING || state == VOICE_DIALOG_RECOGNIZING) {
            if (i >= 3) {
                lv_obj_add_flag(bar, LV_OBJ_FLAG_HIDDEN);
                continue;
            }
            int diameter = i == (motion->phase % 3) ? 9 : 6;
            lv_obj_remove_flag(bar, LV_OBJ_FLAG_HIDDEN);
            lv_obj_set_size(bar, diameter, diameter);
            lv_obj_set_pos(bar, 17 + i * 13, 32 - diameter / 2);
            lv_obj_set_style_radius(bar, LV_RADIUS_CIRCLE, 0);
        } else if (state == VOICE_DIALOG_RECORDING || state == VOICE_DIALOG_WAKEUP ||
                   state == VOICE_DIALOG_SPEAKING) {
            int index = state == VOICE_DIALOG_SPEAKING ? 4 - i : i;
            int height = heights[motion->phase % 4][index];
            lv_obj_remove_flag(bar, LV_OBJ_FLAG_HIDDEN);
            lv_obj_set_size(bar, 5, height);
            lv_obj_set_pos(bar, 12 + i * 9, (64 - height) / 2);
            lv_obj_set_style_radius(bar, 3, 0);
        } else if (i == 2) {
            lv_obj_remove_flag(bar, LV_OBJ_FLAG_HIDDEN);
            lv_obj_set_size(bar, 13, 13);
            lv_obj_set_pos(bar, 25, 25);
            lv_obj_set_style_radius(bar, LV_RADIUS_CIRCLE, 0);
        } else {
            lv_obj_add_flag(bar, LV_OBJ_FLAG_HIDDEN);
        }
        if (recolor) lv_obj_set_style_bg_color(bar, color, 0);
    }
}

static void motion_timer_cb(lv_timer_t *timer)
{
    ai_motion_t *motion = lv_timer_get_user_data(timer);
    bool connected = wifi_connect_is_connected();
    if (!motion->wifi_initialized || connected != motion->wifi_connected) {
        motion->wifi_initialized = true;
        motion->wifi_connected = connected;
        lv_obj_set_style_text_color(motion->wifi_icon,
                                    lv_color_hex(connected ? 0x61d9cd : 0x778899), 0);
    }
    voice_dialog_state_t state = voice_assistant_get_state();
    bool changed = state != motion->state;
    if (changed) {
        motion->state = state;
        motion->phase = 0;
        lv_label_set_text(motion->status, state_text(state));
        lv_label_set_text(motion->hint, state_hint(state));
        lv_obj_set_style_border_color(motion->ring, state_color(state), 0);
        lv_timer_set_period(timer, state_is_active(state) ? 220 : 500);
    } else if (state_is_active(state)) {
        motion->phase++;
    } else {
        return;
    }
    motion_draw_bars(motion, changed);
}

static void motion_deleted_cb(lv_event_t *event)
{
    ai_motion_t *motion = lv_event_get_user_data(event);
    if (!motion) return;
    if (motion->timer) lv_timer_delete(motion->timer);
    free(motion);
}

static void create_caption(lv_obj_t *parent, const char *text, lv_color_t color)
{
    lv_obj_t *caption = lv_label_create(parent);
    lv_obj_set_width(caption, LV_PCT(100));
    lv_label_set_text(caption, text);
    lv_obj_set_style_text_font(caption, &lv_customer_font_ZiTiQuanWeiJunHeiW22_12, 0);
    lv_obj_set_style_text_color(caption, color, 0);
}

static void style_dialog_label(lv_obj_t *label, const char *text,
                               lv_color_t background)
{
    lv_obj_set_size(label, LV_PCT(100), LV_SIZE_CONTENT);
    lv_obj_set_style_min_height(label, 34, 0);
    lv_label_set_long_mode(label, LV_LABEL_LONG_WRAP);
    lv_label_set_text(label, text);
    lv_obj_set_style_text_font(label, &lv_customer_font_ZiTiQuanWeiJunHeiW22_12, 0);
    lv_obj_set_style_text_color(label, lv_color_hex(0xf2f6fc), 0);
    lv_obj_set_style_bg_color(label, background, 0);
    lv_obj_set_style_bg_opa(label, LV_OPA_COVER, 0);
    lv_obj_set_style_radius(label, 8, 0);
    lv_obj_set_style_pad_all(label, 7, 0);
    lv_obj_set_style_text_line_space(label, 2, 0);
}

void ai_status_page_configure(lv_ui *ui)
{
    if (!ui || !ui->screen_AI) return;
    lv_obj_t *screen = ui->screen_AI;
    lv_obj_set_style_bg_color(screen, lv_color_hex(0x0b111b), 0);
    lv_obj_remove_flag(screen, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_set_pos(ui->screen_AI_cont_1, 0, 0);
    lv_obj_set_size(ui->screen_AI_cont_1, 240, 27);
    lv_obj_set_style_bg_color(ui->screen_AI_cont_1, lv_color_hex(0x0b111b), 0);
    lv_obj_set_style_border_width(ui->screen_AI_cont_1, 0, 0);
    lv_obj_remove_flag(ui->screen_AI_cont_1, LV_OBJ_FLAG_SCROLLABLE | LV_OBJ_FLAG_CLICKABLE);
    lv_obj_set_pos(ui->screen_AI_digital_clock_1, 23, 4);
    lv_obj_set_size(ui->screen_AI_digital_clock_1, 66, 22);
    lv_obj_set_style_bg_opa(ui->screen_AI_digital_clock_1, LV_OPA_TRANSP, 0);
    lv_obj_set_style_pad_all(ui->screen_AI_digital_clock_1, 0, 0);
    lv_obj_set_style_text_font(ui->screen_AI_digital_clock_1,
                               &lv_customer_font_ZiTiQuanWeiJunHeiW22_18, 0);
    lv_obj_set_style_text_align(ui->screen_AI_digital_clock_1, LV_TEXT_ALIGN_LEFT, 0);
    lv_obj_set_style_text_color(ui->screen_AI_digital_clock_1, lv_color_hex(0x9bacbf), 0);
    lv_obj_set_pos(ui->screen_AI_img_wifi, 138, 5);
    lv_obj_add_flag(ui->screen_AI_img_wifi, LV_OBJ_FLAG_HIDDEN);
    lv_obj_set_pos(ui->screen_AI_list_battery, 166, 3);
    lv_obj_set_style_bg_opa(ui->screen_AI_list_battery, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(ui->screen_AI_list_battery, 0, 0);
    lv_obj_set_style_pad_all(ui->screen_AI_list_battery, 0, 0);

    lv_obj_t *wifi_icon = lv_label_create(screen);
    lv_obj_set_pos(wifi_icon, 138, 5);
    lv_obj_set_size(wifi_icon, 22, 20);
    lv_label_set_text(wifi_icon, LV_SYMBOL_WIFI);
    lv_obj_set_style_text_font(wifi_icon, LV_FONT_DEFAULT, 0);
    lv_obj_set_style_text_align(wifi_icon, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_set_style_bg_opa(wifi_icon, LV_OPA_TRANSP, 0);

    lv_label_set_text(ui->screen_AI_label_title, "小智问答");
    lv_obj_set_pos(ui->screen_AI_label_title, 15, 31);
    lv_obj_set_size(ui->screen_AI_label_title, 180, 27);
    lv_obj_set_style_bg_opa(ui->screen_AI_label_title, LV_OPA_TRANSP, 0);
    lv_obj_set_style_text_color(ui->screen_AI_label_title, lv_color_hex(0xf2f6fc), 0);
    lv_obj_set_style_text_font(ui->screen_AI_label_title,
                               &lv_customer_font_ZiTiQuanWeiJunHeiW22_18, 0);
    lv_obj_set_style_text_align(ui->screen_AI_label_title, LV_TEXT_ALIGN_LEFT, 0);
    lv_obj_add_flag(ui->screen_AI_imgbtn_return, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(ui->screen_AI_img_ai, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(ui->screen_AI_img_user, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(ui->screen_AI_btn_spreak, LV_OBJ_FLAG_HIDDEN);

    lv_obj_t *return_hint = lv_label_create(screen);
    lv_obj_set_pos(return_hint, 182, 38);
    lv_label_set_text(return_hint, "右滑返回");
    lv_obj_set_style_text_font(return_hint, &lv_customer_font_ZiTiQuanWeiJunHeiW22_12, 0);
    lv_obj_set_style_text_color(return_hint, lv_color_hex(0x9bacbf), 0);

    ai_motion_t *motion = calloc(1, sizeof(*motion));
    if (motion) {
        motion->wifi_icon = wifi_icon;
        motion->state = (voice_dialog_state_t)-1; // 首次刷新时同步真实状态。
        motion->ring = lv_obj_create(screen);
        lv_obj_set_pos(motion->ring, 88, 58);
        lv_obj_set_size(motion->ring, 64, 64);
        lv_obj_set_style_radius(motion->ring, LV_RADIUS_CIRCLE, 0);
        lv_obj_set_style_bg_color(motion->ring, lv_color_hex(0x132b34), 0);
        lv_obj_set_style_border_width(motion->ring, 2, 0);
        lv_obj_set_style_pad_all(motion->ring, 0, 0);
        lv_obj_remove_flag(motion->ring, LV_OBJ_FLAG_SCROLLABLE | LV_OBJ_FLAG_CLICKABLE);
        for (int i = 0; i < 5; ++i) {
            motion->bars[i] = lv_obj_create(motion->ring);
            lv_obj_set_style_border_width(motion->bars[i], 0, 0);
            lv_obj_set_style_pad_all(motion->bars[i], 0, 0);
            lv_obj_remove_flag(motion->bars[i], LV_OBJ_FLAG_SCROLLABLE | LV_OBJ_FLAG_CLICKABLE);
        }

        motion->status = lv_label_create(screen);
        lv_obj_set_pos(motion->status, 12, 125);
        lv_obj_set_size(motion->status, 216, 26);
        lv_obj_set_style_text_align(motion->status, LV_TEXT_ALIGN_CENTER, 0);
        lv_obj_set_style_text_font(motion->status,
                                   &lv_customer_font_ZiTiQuanWeiJunHeiW22_18, 0);
        lv_obj_set_style_text_color(motion->status, lv_color_hex(0xf2f6fc), 0);

        motion->hint = lv_label_create(screen);
        lv_obj_set_pos(motion->hint, 12, 152);
        lv_obj_set_size(motion->hint, 216, 18);
        lv_obj_set_style_text_align(motion->hint, LV_TEXT_ALIGN_CENTER, 0);
        lv_obj_set_style_text_font(motion->hint,
                                   &lv_customer_font_ZiTiQuanWeiJunHeiW22_12, 0);
        lv_obj_set_style_text_color(motion->hint, lv_color_hex(0x9bacbf), 0);
        motion->timer = lv_timer_create(motion_timer_cb, 220, motion);
        lv_obj_add_event_cb(screen, motion_deleted_cb, LV_EVENT_DELETE, motion);
        motion_timer_cb(motion->timer);
    }

    char question[256];
    char answer[384];
    voice_assistant_get_recent_dialog(question, sizeof(question), answer, sizeof(answer));

    // 问答文字长度不可预知，使用可滚动内容区避免回复被屏幕底部裁掉。
    lv_obj_t *transcript = lv_obj_create(screen);
    lv_obj_set_pos(transcript, 12, 171);
    lv_obj_set_size(transcript, 216, 112);
    lv_obj_set_style_bg_opa(transcript, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(transcript, 0, 0);
    lv_obj_set_style_pad_all(transcript, 0, 0);
    lv_obj_set_style_pad_row(transcript, 4, 0);
    lv_obj_set_flex_flow(transcript, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_scroll_dir(transcript, LV_DIR_VER);
    lv_obj_set_scrollbar_mode(transcript, LV_SCROLLBAR_MODE_AUTO);
    lv_obj_add_flag(transcript, LV_OBJ_FLAG_GESTURE_BUBBLE);

    create_caption(transcript, "你说", lv_color_hex(0x9bacbf));
    lv_obj_set_parent(ui->screen_AI_label_user, transcript);
    style_dialog_label(ui->screen_AI_label_user,
                       question[0] ? question : "你说的话会显示在这里",
                       lv_color_hex(0x1b2937));
    create_caption(transcript, "小智", lv_color_hex(0x61d9cd));
    lv_obj_set_parent(ui->screen_AI_label_ai, transcript);
    style_dialog_label(ui->screen_AI_label_ai,
                       answer[0] ? answer : "喊小智开始对话",
                       lv_color_hex(0x162230));

    // 页面重建后旧指针清空，后台语音回调只写当前页面。
    lv_obj_null_on_delete(&ui->screen_AI);
    lv_obj_null_on_delete(&ui->screen_AI_btn_spreak);
    lv_obj_null_on_delete(&ui->screen_AI_btn_spreak_label);
    lv_obj_null_on_delete(&ui->screen_AI_label_ai);
    lv_obj_null_on_delete(&ui->screen_AI_label_user);
    lv_obj_null_on_delete(&ui->screen_AI_list_battery);
}
