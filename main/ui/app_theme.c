#include "app_theme.h"
#include "src/themes/lv_theme_private.h"

static lv_theme_t s_theme;
static lv_style_t s_screen, s_card, s_accent, s_knob, s_muted;
static bool s_initialized;

static void apply_theme(lv_theme_t *theme, lv_obj_t *obj)
{
    (void)theme;
    // 图片使用原资源和原样式；标签继承父控件文字色，保证金色按钮上的深色文字。
    if (lv_obj_check_type(obj, &lv_image_class) ||
        lv_obj_check_type(obj, &lv_imagebutton_class) ||
        lv_obj_check_type(obj, &lv_label_class)) return;
    lv_obj_add_style(obj, lv_obj_get_parent(obj) ? &s_card : &s_screen, LV_PART_MAIN);
    lv_obj_add_style(obj, &s_muted, LV_PART_MAIN | LV_STATE_DISABLED);
    if (lv_obj_check_type(obj, &lv_slider_class) ||
        lv_obj_check_type(obj, &lv_bar_class) ||
        lv_obj_check_type(obj, &lv_switch_class)) {
        lv_obj_add_style(obj, &s_accent, LV_PART_INDICATOR);
        lv_obj_add_style(obj, &s_knob, LV_PART_KNOB);
    }
    if (lv_obj_check_type(obj, &lv_keyboard_class) ||
        lv_obj_check_type(obj, &lv_buttonmatrix_class)) {
        lv_obj_add_style(obj, &s_card, LV_PART_ITEMS);
        lv_obj_add_style(obj, &s_accent, LV_PART_ITEMS | LV_STATE_PRESSED);
        lv_obj_add_style(obj, &s_accent, LV_PART_ITEMS | LV_STATE_CHECKED);
    }
    if (lv_obj_check_type(obj, &lv_button_class)) {
        lv_obj_add_style(obj, &s_accent, LV_PART_MAIN | LV_STATE_CHECKED);
    }
}

void app_theme_init(void)
{
    lv_display_t *display = lv_display_get_default();
    if (!display || s_initialized) return;
    lv_theme_t *theme = lv_theme_default_init(display,
        lv_color_hex(APP_THEME_ACCENT), lv_color_hex(APP_THEME_BORDER),
        true, LV_FONT_DEFAULT);
    lv_style_init(&s_screen);
    lv_style_set_bg_color(&s_screen, lv_color_hex(APP_THEME_BG));
    lv_style_set_text_color(&s_screen, lv_color_hex(APP_THEME_TEXT));
    lv_style_init(&s_card);
    lv_style_set_bg_color(&s_card, lv_color_hex(APP_THEME_CARD));
    lv_style_set_text_color(&s_card, lv_color_hex(APP_THEME_TEXT));
    lv_style_set_border_color(&s_card, lv_color_hex(APP_THEME_BORDER));
    lv_style_init(&s_accent);
    lv_style_set_bg_color(&s_accent, lv_color_hex(APP_THEME_ACCENT));
    lv_style_set_text_color(&s_accent, lv_color_hex(APP_THEME_ON_ACCENT));
    lv_style_init(&s_knob);
    lv_style_set_bg_color(&s_knob, lv_color_hex(APP_THEME_TEXT));
    lv_style_init(&s_muted);
    lv_style_set_text_color(&s_muted, lv_color_hex(APP_THEME_MUTED));
    s_theme = *theme;
    lv_theme_set_parent(&s_theme, theme);
    lv_theme_set_apply_cb(&s_theme, apply_theme);
    lv_display_set_theme(display, &s_theme);
    lv_obj_set_style_bg_color(lv_layer_bottom(), lv_color_hex(APP_THEME_BG), 0);
    s_initialized = true;
}
