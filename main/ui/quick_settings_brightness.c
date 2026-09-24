#include "quick_settings_brightness.h"

#include "esp_log.h"
#include "system_settings.h"

static const char *TAG = "QuickBrightness";

static void brightness_changed_cb(lv_event_t *event)
{
    lv_obj_t *slider = lv_event_get_current_target_obj(event);
    uint8_t value = (uint8_t)lv_slider_get_value(slider);
    esp_err_t err = system_preview_brightness(value);
    if (err != ESP_OK) ESP_LOGW(TAG, "预览亮度失败: %s", esp_err_to_name(err));
}

static void brightness_save_cb(lv_event_t *event)
{
    lv_obj_t *slider = lv_event_get_current_target_obj(event);
    uint8_t value = (uint8_t)lv_slider_get_value(slider);
    esp_err_t err = system_set_brightness(value);
    if (err != ESP_OK) ESP_LOGW(TAG, "保存亮度失败: %s", esp_err_to_name(err));
}

static void volume_changed_cb(lv_event_t *event)
{
    lv_obj_t *slider = lv_event_get_current_target_obj(event);
    (void)system_preview_volume((uint8_t)lv_slider_get_value(slider));
}

static void volume_save_cb(lv_event_t *event)
{
    lv_obj_t *slider = lv_event_get_current_target_obj(event);
    esp_err_t err = system_set_volume((uint8_t)lv_slider_get_value(slider));
    if (err != ESP_OK) ESP_LOGW(TAG, "保存音量失败: %s", esp_err_to_name(err));
}

void quick_settings_brightness_init(lv_ui *ui)
{
    if (!ui || !ui->screen_down_slider_brightness) return;

    lv_obj_t *slider = ui->screen_down_slider_brightness;
    lv_obj_set_pos(slider, 76, 68);
    lv_obj_set_size(slider, 146, 31);
    lv_obj_set_style_radius(slider, 16, LV_PART_MAIN);
    lv_obj_set_style_radius(slider, 16, LV_PART_INDICATOR);
    lv_slider_set_range(slider, 5, 100);
    lv_slider_set_value(slider, system_get_brightness(), LV_ANIM_OFF);
    lv_obj_add_event_cb(slider, brightness_changed_cb, LV_EVENT_VALUE_CHANGED, NULL);
    lv_obj_add_event_cb(slider, brightness_save_cb, LV_EVENT_RELEASED, NULL);

    lv_obj_t *volume_slider = ui->screen_down_slider_1;
    if (volume_slider) {
        lv_obj_set_pos(volume_slider, 76, 29);
        lv_obj_set_size(volume_slider, 146, 31);
        lv_obj_set_style_radius(volume_slider, 16, LV_PART_MAIN);
        lv_obj_set_style_radius(volume_slider, 16, LV_PART_INDICATOR);
        lv_slider_set_value(volume_slider, system_get_volume(), LV_ANIM_OFF);
        lv_obj_add_event_cb(volume_slider, volume_changed_cb, LV_EVENT_VALUE_CHANGED, NULL);
        lv_obj_add_event_cb(volume_slider, volume_save_cb, LV_EVENT_RELEASED, NULL);
    }
}
