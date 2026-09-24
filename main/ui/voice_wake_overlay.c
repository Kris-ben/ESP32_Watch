#include "voice_wake_overlay.h"

#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "lvgl.h"

extern SemaphoreHandle_t lvgl_mutex;

static lv_obj_t *s_outer;
static lv_obj_t *s_inner;
static lv_timer_t *s_timer;
static uint8_t s_phase;

static void apply_mode(voice_wake_overlay_mode_t mode)
{
    static const uint32_t colors[][2] = {
        {0x52d9f5, 0x3982ea}, // 听问题：青蓝
        {0xf5bd69, 0xd88940}, // 识别与思考：暖黄
        {0xa99bff, 0x6e83fa}, // 回复：紫蓝
    };
    if (mode > VOICE_WAKE_OVERLAY_REPLYING) return;
    lv_obj_set_style_border_color(s_outer, lv_color_hex(colors[mode][0]), 0);
    lv_obj_set_style_border_color(s_inner, lv_color_hex(colors[mode][1]), 0);
}

static void pulse_cb(lv_timer_t *timer)
{
    (void)timer;
    // 双层描边交替呼吸，不重绘整页内容，也不拦截触摸。
    static const lv_opa_t outer_opa[] = {110, 145, 185, 225, 255, 225, 185, 145};
    static const lv_opa_t inner_opa[] = {35, 55, 80, 110, 140, 110, 80, 55};
    s_phase = (s_phase + 1) % 8;
    lv_obj_set_style_border_opa(s_outer, outer_opa[s_phase], 0);
    lv_obj_set_style_border_opa(s_inner, inner_opa[s_phase], 0);
}

static lv_obj_t *create_outline(lv_obj_t *parent, int inset, int width, uint32_t color)
{
    lv_obj_t *outline = lv_obj_create(parent);
    lv_obj_set_pos(outline, inset, inset);
    lv_display_t *display = lv_display_get_default();
    lv_obj_set_size(outline, lv_display_get_horizontal_resolution(display) - inset * 2,
                    lv_display_get_vertical_resolution(display) - inset * 2);
    lv_obj_set_style_radius(outline, 25 - inset, 0);
    lv_obj_set_style_bg_opa(outline, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(outline, width, 0);
    lv_obj_set_style_border_color(outline, lv_color_hex(color), 0);
    lv_obj_set_style_pad_all(outline, 0, 0);
    lv_obj_set_style_shadow_width(outline, 0, 0);
    lv_obj_remove_flag(outline, LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_SCROLLABLE);
    return outline;
}

void voice_wake_overlay_show(void)
{
    if (!lvgl_mutex || xSemaphoreTake(lvgl_mutex, pdMS_TO_TICKS(200)) != pdTRUE) return;

    if (!s_outer) {
        lv_obj_t *layer = lv_layer_top();
        s_outer = create_outline(layer, 2, 3, 0x52d9f5);
        s_inner = create_outline(layer, 7, 1, 0x3982ea);
        s_timer = lv_timer_create(pulse_cb, 150, NULL);
    }
    s_phase = 0;
    apply_mode(VOICE_WAKE_OVERLAY_LISTENING);
    lv_obj_set_style_border_opa(s_outer, 110, 0);
    lv_obj_set_style_border_opa(s_inner, 35, 0);
    lv_obj_remove_flag(s_outer, LV_OBJ_FLAG_HIDDEN);
    lv_obj_remove_flag(s_inner, LV_OBJ_FLAG_HIDDEN);
    lv_timer_resume(s_timer);
    xSemaphoreGive(lvgl_mutex);
}

void voice_wake_overlay_set_mode(voice_wake_overlay_mode_t mode)
{
    if (!lvgl_mutex || xSemaphoreTake(lvgl_mutex, pdMS_TO_TICKS(200)) != pdTRUE) return;
    if (s_outer && !lv_obj_has_flag(s_outer, LV_OBJ_FLAG_HIDDEN)) {
        apply_mode(mode);
    }
    xSemaphoreGive(lvgl_mutex);
}

void voice_wake_overlay_hide(void)
{
    if (!lvgl_mutex || xSemaphoreTake(lvgl_mutex, pdMS_TO_TICKS(200)) != pdTRUE) return;
    if (s_outer) {
        lv_obj_add_flag(s_outer, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(s_inner, LV_OBJ_FLAG_HIDDEN);
        lv_timer_pause(s_timer);
    }
    xSemaphoreGive(lvgl_mutex);
}
