#include "ui_landscape.h"

#define UI_PORTRAIT_WIDTH  240
#define UI_PORTRAIT_HEIGHT 284
#define UI_LANDSCAPE_WIDTH 284
#define UI_LANDSCAPE_HEIGHT 240

int32_t ui_landscape_scale_x(int32_t value)
{
    return value * UI_LANDSCAPE_WIDTH / UI_PORTRAIT_WIDTH;
}

int32_t ui_landscape_scale_y(int32_t value)
{
    return value * UI_LANDSCAPE_HEIGHT / UI_PORTRAIT_HEIGHT;
}

static void scale_children(lv_obj_t *parent)
{
    uint32_t count = lv_obj_get_child_count(parent);
    for (uint32_t i = 0; i < count; ++i) {
        lv_obj_t *child = lv_obj_get_child(parent, i);
        int32_t x = lv_obj_get_x(child);
        int32_t y = lv_obj_get_y(child);
        int32_t width = lv_obj_get_width(child);
        int32_t height = lv_obj_get_height(child);
        bool is_label = lv_obj_check_type(child, &lv_label_class);
        bool auto_height = is_label && lv_obj_get_style_height(child, LV_PART_MAIN) == LV_SIZE_CONTENT;

        scale_children(child);
        // 先改为左上角定位，否则原本居中的子控件会把换算后的坐标再叠加一次。
        lv_obj_set_align(child, LV_ALIGN_TOP_LEFT);
        lv_obj_set_pos(child, ui_landscape_scale_x(x), ui_landscape_scale_y(y));
        // 字号没有缩小，标签高度也不能压短，否则汉字和数字的下沿会被裁掉。
        lv_obj_set_size(child, ui_landscape_scale_x(width), is_label ? (auto_height ? LV_SIZE_CONTENT : height) : ui_landscape_scale_y(height));
    }
}

void ui_landscape_apply(lv_ui *ui, lv_obj_t *screen)
{
    if (!screen || lv_obj_has_flag(screen, LV_OBJ_FLAG_USER_1)) {
        return;
    }

    // 页面由原有生成器按竖屏坐标创建；加载前统一换算，避免横屏裁掉底部控件。
    lv_obj_update_layout(screen);
    scale_children(screen);
    lv_obj_set_size(screen, UI_LANDSCAPE_WIDTH, UI_LANDSCAPE_HEIGHT);
    // 状态栏图标的位置保存在手写布局中，避免生成器覆盖校准值。
    if (ui && screen == ui->screen_home && ui->screen_home_img_wifi) {
        lv_obj_set_y(ui->screen_home_img_wifi, ui_landscape_scale_y(3));
    } else if (ui && screen == ui->screen_weather && ui->screen_weather_img_wificlose) {
        lv_obj_set_y(ui->screen_weather_img_wificlose, ui_landscape_scale_y(0));
    }
    lv_obj_add_flag(screen, LV_OBJ_FLAG_USER_1);
}

void ui_landscape_apply_widget(lv_obj_t *widget)
{
    if (!widget || lv_obj_has_flag(widget, LV_OBJ_FLAG_USER_1)) {
        return;
    }

    lv_obj_update_layout(widget);
    int32_t x = lv_obj_get_x(widget);
    int32_t y = lv_obj_get_y(widget);
    int32_t width = lv_obj_get_width(widget);
    int32_t height = lv_obj_get_height(widget);
    scale_children(widget);
    lv_obj_set_align(widget, LV_ALIGN_TOP_LEFT);
    lv_obj_set_pos(widget, ui_landscape_scale_x(x), ui_landscape_scale_y(y));
    lv_obj_set_size(widget, ui_landscape_scale_x(width), ui_landscape_scale_y(height));
    lv_obj_add_flag(widget, LV_OBJ_FLAG_USER_1);
}
