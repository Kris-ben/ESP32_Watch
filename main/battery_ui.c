/**
 * @file battery_ui.c
 * @brief 电池电量UI显示模块实现
 *
 * - 定期从 battery_monitor 读取电压，更新各页面的估算电量
 * - 原位更新各页面的电池图标和百分比，避免定时刷新时闪动
 */

#include "battery_ui.h"
#include "battery_monitor.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "esp_log.h"
#include "lvgl.h"
#include <stdio.h>

static const char *TAG = "BATTERY_UI";

#define BATTERY_UI_UPDATE_INTERVAL_MS   10000

static lv_ui *s_ui = NULL;
static bool s_cached_percentage_valid = false;
static int s_cached_percentage = -1;
extern SemaphoreHandle_t lvgl_mutex;

const char *battery_ui_symbol_for_percentage(int percentage)
{
    if (percentage >= 80) return LV_SYMBOL_BATTERY_FULL;
    if (percentage >= 60) return LV_SYMBOL_BATTERY_3;
    if (percentage >= 40) return LV_SYMBOL_BATTERY_2;
    if (percentage >= 20) return LV_SYMBOL_BATTERY_1;
    return LV_SYMBOL_BATTERY_EMPTY;
}

/**
 * @brief 更新现有列表按钮并应用样式
 *
 * @param list       列表对象 (screen_xxx_list_bettery)
 * @param item_ptr   按钮指针存储位置
 * @param icon       电池符号
 * @param text       估算百分比文本
 */
static void battery_update_list(lv_obj_t *list, lv_obj_t **item_ptr,
                                const char *icon, const char *text)
{
    if (!list || !lv_obj_is_valid(list)) return;

    lv_obj_set_style_pad_all(list, 0, 0);
    lv_obj_remove_flag(list, LV_OBJ_FLAG_SCROLLABLE | LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_flag(list, LV_OBJ_FLAG_GESTURE_BUBBLE);

    lv_obj_t *btn = *item_ptr;
    if (!btn || !lv_obj_is_valid(btn) || lv_obj_get_parent(btn) != list) {
        btn = lv_list_add_button(list, NULL, text);
        *item_ptr = btn;
    }
    if (!btn) return;

    lv_obj_t *icon_img = NULL;
    for (uint32_t i = 0; i < lv_obj_get_child_count(btn); ++i) {
        lv_obj_t *child = lv_obj_get_child(btn, i);
        if (lv_obj_check_type(child, &lv_image_class)) {
            icon_img = child;
            break;
        }
    }
    if (!icon_img) {
        icon_img = lv_image_create(btn);
        lv_obj_move_to_index(icon_img, 0);
    }
    lv_image_set_src(icon_img, icon);
    lv_list_set_button_text(list, btn, text);

    lv_obj_update_layout(list);
    lv_obj_set_size(btn, LV_PCT(100), lv_obj_get_content_height(list));
    lv_obj_set_style_pad_all(btn, 0, 0);
    lv_obj_set_style_pad_column(btn, 2, 0);
    lv_obj_set_style_border_width(btn, 0, 0);
    lv_obj_set_style_text_color(btn, lv_color_hex(0xfbfbfb), 0);
    lv_obj_set_style_text_font(btn, &lv_font_montserrat_14, 0);
    lv_obj_set_style_bg_opa(btn, LV_OPA_TRANSP, 0);
    lv_obj_remove_flag(btn, LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_add_flag(btn, LV_OBJ_FLAG_GESTURE_BUBBLE);

}

static void battery_update_ai_list(lv_ui *ui, const char *icon, const char *text)
{
    battery_update_list(ui->screen_AI_list_battery,
                        &ui->screen_AI_list_battery_item0, icon, text);
    if (ui->screen_AI_list_battery && ui->screen_AI_list_battery_item0 &&
        lv_obj_is_valid(ui->screen_AI_list_battery) &&
        lv_obj_is_valid(ui->screen_AI_list_battery_item0) &&
        lv_obj_get_parent(ui->screen_AI_list_battery_item0) == ui->screen_AI_list_battery) {
        lv_obj_set_style_pad_all(ui->screen_AI_list_battery_item0, 1, 0);
        lv_obj_set_style_bg_opa(ui->screen_AI_list_battery_item0, LV_OPA_TRANSP, 0);
        lv_obj_set_style_border_width(ui->screen_AI_list_battery_item0, 0, 0);
    }
}

void battery_ui_apply_cached_to_screen_nolock(lv_ui *ui, lv_obj_t *screen)
{
    if (!ui || !screen || !s_cached_percentage_valid) return;

    char percentage_text[8];
    int percentage = s_cached_percentage;
    if (percentage > 100) percentage = 100;
    if (percentage < 0) snprintf(percentage_text, sizeof(percentage_text), "--");
    else snprintf(percentage_text, sizeof(percentage_text), "%d%%", percentage);
    const char *icon = battery_ui_symbol_for_percentage(percentage);

    if (screen == ui->screen_home) {
        battery_update_list(ui->screen_home_list_bettery,
                            &ui->screen_home_list_bettery_item0, icon, percentage_text);
    } else if (screen == ui->screen_weather) {
        battery_update_list(ui->screen_weather_list_bettery,
                            &ui->screen_weather_list_bettery_item0, icon, percentage_text);
    } else if (screen == ui->screen_wifi) {
        battery_update_list(ui->screen_wifi_list_battery,
                            &ui->screen_wifi_list_battery_item0, icon, percentage_text);
    } else if (screen == ui->screen_AI) {
        battery_update_ai_list(ui, icon, percentage_text);
    } else if (screen == ui->screen_musiclist) {
        battery_update_list(ui->screen_musiclist_list_battery,
                            &ui->screen_musiclist_list_battery_item0, icon, percentage_text);
    } else if (screen == ui->screen_play) {
        battery_update_list(ui->screen_play_list_battery,
                            &ui->screen_play_list_battery_item0, icon, percentage_text);
        battery_update_list(ui->screen_play_list_3,
                            &ui->screen_play_list_3_item0, icon, percentage_text);
    }
}

/* ---- 更新逻辑 ---- */

static void battery_ui_apply_info_nolock(lv_ui *ui, const battery_info_t *info)
{
    if (!ui || !info) {
        return;
    }

    char percentage_text[8];
    int percentage = info->percentage;
    if (percentage > 100) percentage = 100;
    if (percentage < 0) snprintf(percentage_text, sizeof(percentage_text), "--");
    else snprintf(percentage_text, sizeof(percentage_text), "%d%%", percentage);
    const char *icon = battery_ui_symbol_for_percentage(percentage);
    s_cached_percentage = percentage;
    s_cached_percentage_valid = true;

    battery_update_list(ui->screen_home_list_bettery,
                        &ui->screen_home_list_bettery_item0,
                        icon, percentage_text);

    battery_update_list(ui->screen_weather_list_bettery,
                        &ui->screen_weather_list_bettery_item0,
                        icon, percentage_text);

    battery_update_list(ui->screen_wifi_list_battery,
                        &ui->screen_wifi_list_battery_item0,
                        icon, percentage_text);
    battery_update_ai_list(ui, icon, percentage_text);
    battery_update_list(ui->screen_musiclist_list_battery,
                        &ui->screen_musiclist_list_battery_item0,
                        icon, percentage_text);
    battery_update_list(ui->screen_play_list_battery,
                        &ui->screen_play_list_battery_item0,
                        icon, percentage_text);
    battery_update_list(ui->screen_play_list_3,
                        &ui->screen_play_list_3_item0,
                        icon, percentage_text);

    if (info->voltage_mv >= 0) {
        ESP_LOGI(TAG, "BAT+ 测量电压: %dmV, 估算电量: %d%% (充电时仅供参考)",
                 info->voltage_mv, info->percentage);
    }
}

void battery_ui_update_nolock(lv_ui *ui)
{
    if (!ui) return;

    battery_info_t info;
    if (!battery_get_info(&info)) {
        ESP_LOGW(TAG, "读取电池信息失败");
        return;
    }

    battery_ui_apply_info_nolock(ui, &info);
}

void battery_ui_update(lv_ui *ui)
{
    if (!ui) return;

    battery_info_t info;
    if (!battery_get_info(&info)) {
        ESP_LOGW(TAG, "读取电池信息失败");
        return;
    }

    if (xSemaphoreTake(lvgl_mutex, pdMS_TO_TICKS(200)) == pdTRUE) {
        battery_ui_apply_info_nolock(ui, &info);
        xSemaphoreGive(lvgl_mutex);
    } else {
        ESP_LOGW(TAG, "无法获取lvgl_mutex，跳过");
    }
}

/* ---- 后台任务 ---- */

static void battery_ui_task(void *arg)
{
    vTaskDelay(pdMS_TO_TICKS(5000));   // 等待 UI 完全就绪
    ESP_LOGI(TAG, "电池UI刷新任务已启动 (周期: %d ms)", BATTERY_UI_UPDATE_INTERVAL_MS);

    while (1) {
        battery_ui_update(s_ui);
        vTaskDelay(pdMS_TO_TICKS(BATTERY_UI_UPDATE_INTERVAL_MS));
    }
}

bool battery_ui_init(lv_ui *ui)
{
    if (!ui) {
        ESP_LOGE(TAG, "参数 ui 为空");
        return false;
    }
    s_ui = ui;

    if (!battery_monitor_init()) {
        ESP_LOGE(TAG, "电池硬件驱动初始化失败");
        return false;
    }

#if !BATTERY_MONITOR_HAS_BATTERY
    ESP_LOGI(TAG, "未安装电池，界面保持 --，无需启动采样任务");
    return true;
#endif

    battery_ui_update(ui);

    BaseType_t ret = xTaskCreate(battery_ui_task, "battery_ui",
                                 3072, NULL, 3, NULL);
    if (ret != pdPASS) {
        ESP_LOGE(TAG, "电池UI任务创建失败");
        return false;
    }

    ESP_LOGI(TAG, "电池UI模块初始化成功");
    return true;
}
