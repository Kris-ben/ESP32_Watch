/**
 * @file battery_ui.c
 * @brief 电池电量UI显示模块实现
 *
 * - 定期从 battery_monitor 读取电量，更新两个屏幕上的电池控件
 * - 使用 lv_obj_clean + lv_list_add_button 安全重建控件
 */

#include "app_theme.h"
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
extern SemaphoreHandle_t lvgl_mutex;

LV_FONT_DECLARE(lv_font_ZiTiQuanWeiJunHeiW22_12)

/**
 * @brief 清空列表，重建按钮并应用样式
 *
 * @param list       列表对象 (screen_xxx_list_bettery)
 * @param item_ptr   按钮指针存储位置
 * @param icon       电池符号
 * @param text       百分比文本
 */
static void battery_update_list(lv_obj_t *list, lv_obj_t **item_ptr,
                                const char *icon, const char *text)
{
    if (!list || !lv_obj_is_valid(list)) return;

    /* 清除旧按钮 */
    lv_obj_clean(list);

    /* 重新创建按钮 */
    lv_obj_t *btn = lv_list_add_button(list, icon, text);
    if (!btn) return;

    /* 基础样式 */
    lv_obj_set_style_pad_top(btn, 5, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(btn, 5, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(btn, 5, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(btn, 5, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(btn, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(btn, lv_color_hex(APP_THEME_TEXT), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(btn, &lv_font_ZiTiQuanWeiJunHeiW22_12, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(btn, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_radius(btn, 3, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(btn, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(btn, lv_color_hex(APP_THEME_BG), LV_PART_MAIN | LV_STATE_DEFAULT);

    *item_ptr = btn;
}

/* ---- 更新逻辑 ---- */

static void battery_ui_apply_info_nolock(lv_ui *ui, const battery_info_t *info)
{
    if (!ui || !info) {
        return;
    }

    char voltage_text[16];
    if (info->voltage_mv < 0) {
        snprintf(voltage_text, sizeof(voltage_text), "--");
    } else {
        // USB 充电时 BAT+ 电压不能证明电池容量，界面只显示直接测得的电压。
        snprintf(voltage_text, sizeof(voltage_text), "%d.%02dV",
                 info->voltage_mv / 1000, (info->voltage_mv % 1000) / 10);
    }

    battery_update_list(ui->screen_home_list_bettery,
                        &ui->screen_home_list_bettery_item0,
                        NULL, voltage_text);

    battery_update_list(ui->screen_weather_list_bettery,
                        &ui->screen_weather_list_bettery_item0,
                        NULL, voltage_text);

    battery_update_list(ui->screen_wifi_list_battery,
                        &ui->screen_wifi_list_battery_item0,
                        NULL, voltage_text);
    battery_update_list(ui->screen_AI_list_battery,
                        &ui->screen_AI_list_battery_item0,
                        NULL, voltage_text);
    if (ui->screen_AI_list_battery && ui->screen_AI_list_battery_item0 &&
        lv_obj_is_valid(ui->screen_AI_list_battery) &&
        lv_obj_is_valid(ui->screen_AI_list_battery_item0) &&
        lv_obj_get_parent(ui->screen_AI_list_battery_item0) == ui->screen_AI_list_battery) {
        lv_obj_set_style_pad_all(ui->screen_AI_list_battery_item0, 1, 0);
        lv_obj_set_style_bg_opa(ui->screen_AI_list_battery_item0, LV_OPA_TRANSP, 0);
        lv_obj_set_style_border_width(ui->screen_AI_list_battery_item0, 0, 0);
    }
    battery_update_list(ui->screen_musiclist_list_battery,
                        &ui->screen_musiclist_list_battery_item0,
                        NULL, voltage_text);
    battery_update_list(ui->screen_play_list_battery,
                        &ui->screen_play_list_battery_item0,
                        NULL, voltage_text);
    battery_update_list(ui->screen_play_list_3,
                        &ui->screen_play_list_3_item0,
                        NULL, voltage_text);

    if (info->voltage_mv >= 0) {
        ESP_LOGI(TAG, "BAT+ 测量电压: %dmV (充电时不代表剩余电量)", info->voltage_mv);
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

    BaseType_t ret = xTaskCreate(battery_ui_task, "battery_ui",
                                 3072, NULL, 3, NULL);
    if (ret != pdPASS) {
        ESP_LOGE(TAG, "电池UI任务创建失败");
        return false;
    }

    ESP_LOGI(TAG, "电池UI模块初始化成功");
    return true;
}
