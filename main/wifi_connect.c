/**
 * @file wifi_connect.c
 * @brief WiFi扫描和连接模块实现
 */

#include "wifi_connect.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "wifi_manager.h"
#include "lvgl.h"
#include "wifi_scan_page.h"
#include <string.h>

static const char *TAG = "WiFiConnect";

// WiFi图标声明
LV_IMAGE_DECLARE(_wifi_RGB565A8_17x16);
LV_IMAGE_DECLARE(_wifi_close_RGB565A8_23x26);
LV_IMAGE_DECLARE(_wifi_close_RGB565A8_20x24);
LV_IMAGE_DECLARE(_wifi_close_RGB565A8_20x20);

// 扫描到的WiFi列表
static char g_wifi_ssid_list[WIFI_LIST_MAX_COUNT][33] = {0};  // SSID最大32字节
static int g_wifi_count = 0;

// 当前选中的WiFi索引
static int g_selected_index = -1;

// UI句柄缓存 (用于异步更新)
static lv_ui *g_ui = NULL;
static lv_obj_t *g_scan_screen = NULL;

static void wifi_screen_deleted_cb(lv_event_t *e)
{
    if (g_scan_screen == lv_event_get_target(e)) {
        g_scan_screen = NULL;
    }
}

/**
 * @brief WiFi扫描完成回调
 */
static void wifi_scan_done_callback(int numbers, wifi_ap_record_t *ap_records)
{
    ESP_LOGI(TAG, "扫描完成，发现 %d 个WiFi", numbers);
    extern SemaphoreHandle_t lvgl_mutex;
    // 后台扫描任务等待界面锁，避免界面暂时忙时丢掉唯一的完成通知。
    if (!lvgl_mutex || xSemaphoreTake(lvgl_mutex, portMAX_DELAY) != pdTRUE) {
        ESP_LOGE(TAG, "扫描完成但界面锁不可用，无法显示结果");
        return;
    }

    // 扫描结果和列表点击共用这些数据，统一在界面锁内更新。
    memset(g_wifi_ssid_list, 0, sizeof(g_wifi_ssid_list));
    g_wifi_count = 0;
    // 隐藏热点没有可点击的名称，跳过后再填满最多五个列表项。
    for (int i = 0; i < numbers && ap_records && g_wifi_count < WIFI_LIST_MAX_COUNT; ++i) {
        if (ap_records[i].ssid[0] == '\0') continue;
        strncpy(g_wifi_ssid_list[g_wifi_count], (const char *)ap_records[i].ssid, 32);
        ESP_LOGI(TAG, "  [%d] %s (RSSI: %d)", g_wifi_count,
                 g_wifi_ssid_list[g_wifi_count], ap_records[i].rssi);
        ++g_wifi_count;
    }
    
    // 页面删除后不再写旧控件；列表项也必须属于当前列表容器。
    if (g_ui && g_scan_screen && g_ui->screen_wifi == g_scan_screen &&
        lv_obj_is_valid(g_scan_screen)) {
        lv_obj_t *list = g_ui->screen_wifi_list_wifi;
        if (list && lv_obj_is_valid(list) && lv_obj_get_screen(list) == g_scan_screen) {
            lv_obj_t *label = g_ui->screen_wifi_btn_scanf_label;
            if (label && lv_obj_is_valid(label) && lv_obj_get_screen(label) == g_scan_screen) {
                lv_label_set_text(label, numbers < 0 ? "扫描失败" :
                                      (g_wifi_count > 0 ? "扫描WiFi" : "无结果"));
            }
            lv_obj_t *items[] = {
                g_ui->screen_wifi_list_wifi_item0, g_ui->screen_wifi_list_wifi_item1,
                g_ui->screen_wifi_list_wifi_item2, g_ui->screen_wifi_list_wifi_item3,
                g_ui->screen_wifi_list_wifi_item4
            };
            for (int i = 0; i < WIFI_LIST_MAX_COUNT; i++) {
                if (items[i] && lv_obj_is_valid(items[i]) && lv_obj_get_parent(items[i]) == list) {
                    const char *ssid = (i < g_wifi_count && g_wifi_ssid_list[i][0]) ?
                                       g_wifi_ssid_list[i] : "";
                    lv_list_set_button_text(list, items[i], ssid);
                }
            }
            wifi_scan_page_on_finished(g_ui, numbers < 0 ? -1 : g_wifi_count);
            ESP_LOGI(TAG, "WiFi列表UI已更新");
        }
    }
    g_scan_screen = NULL;
    xSemaphoreGive(lvgl_mutex);
}

esp_err_t wifi_connect_init(void)
{
    ESP_LOGI(TAG, "WiFi连接模块初始化");
    g_selected_index = -1;
    g_wifi_count = 0;
    return ESP_OK;
}

void wifi_connect_scan(lv_ui *ui)
{
    if (!ui) return;
    ESP_LOGI(TAG, "开始WiFi扫描...");
    g_ui = ui;
    lv_obj_t *previous_scan_screen = g_scan_screen;
    g_scan_screen = ui->screen_wifi;

    g_selected_index = -1;
    g_wifi_count = 0;
    memset(g_wifi_ssid_list, 0, sizeof(g_wifi_ssid_list));
    wifi_scan_page_on_started(ui);
    lv_obj_t *items[] = {
        ui->screen_wifi_list_wifi_item0, ui->screen_wifi_list_wifi_item1,
        ui->screen_wifi_list_wifi_item2, ui->screen_wifi_list_wifi_item3,
        ui->screen_wifi_list_wifi_item4
    };
    for (int i = 0; i < WIFI_LIST_MAX_COUNT; i++) {
        if (items[i] && lv_obj_is_valid(items[i])) {
            lv_list_set_button_text(ui->screen_wifi_list_wifi, items[i], "");
        }
    }
    if (ui->screen_wifi_btn_scanf_label && lv_obj_is_valid(ui->screen_wifi_btn_scanf_label)) {
        lv_label_set_text(ui->screen_wifi_btn_scanf_label, "扫描中");
    }

    // 先清空旧列表，再启动异步任务，避免扫描很快完成后又被这里清空。
    esp_err_t ret = wifi_manager_scan(wifi_scan_done_callback);
    if (ret != ESP_OK) {
        g_scan_screen = previous_scan_screen;
        ESP_LOGE(TAG, "启动WiFi扫描失败: %s", esp_err_to_name(ret));
        if (ui->screen_wifi_btn_scanf_label && lv_obj_is_valid(ui->screen_wifi_btn_scanf_label)) {
            lv_label_set_text(ui->screen_wifi_btn_scanf_label,
                              ret == ESP_ERR_INVALID_STATE ? "扫描中" : "扫描失败");
        }
        if (ret != ESP_ERR_INVALID_STATE) wifi_scan_page_on_finished(ui, -1);
    }
}

bool wifi_connect_set_selected_index(int index)
{
    if (index >= 0 && index < g_wifi_count && g_wifi_ssid_list[index][0] != '\0') {
        g_selected_index = index;
        ESP_LOGI(TAG, "选中WiFi索引: %d, SSID: %s", index, 
                 strlen(g_wifi_ssid_list[index]) > 0 ? g_wifi_ssid_list[index] : "(空)");
        return true;
    }
    ESP_LOGW(TAG, "WiFi列表项%d没有有效热点", index);
    return false;
}

int wifi_connect_get_selected_index(void)
{
    return g_selected_index;
}

const char *wifi_connect_get_scanned_ssid(int index)
{
    return (index >= 0 && index < g_wifi_count) ? g_wifi_ssid_list[index] : "";
}

esp_err_t wifi_connect_with_password(lv_ui *ui)
{
    if (!ui) return ESP_ERR_INVALID_ARG;
    
    if (g_selected_index < 0 || g_selected_index >= WIFI_LIST_MAX_COUNT) {
        ESP_LOGE(TAG, "未选择WiFi");
        return ESP_ERR_INVALID_STATE;
    }
    
    const char *ssid = g_wifi_ssid_list[g_selected_index];
    if (strlen(ssid) == 0) {
        ESP_LOGE(TAG, "SSID为空");
        return ESP_ERR_INVALID_STATE;
    }
    
    // 从文本输入框获取密码
    const char *password = lv_textarea_get_text(ui->screen_wifi_connect_ta_input);
    if (!password) {
        password = "";
    }
    
    ESP_LOGI(TAG, "连接WiFi: %s", ssid);
    
    // 连接WiFi
    esp_err_t ret = wifi_manager_connect(ssid, password);
    if (ret == ESP_OK) {
        ESP_LOGI(TAG, "WiFi连接请求已发送");
    } else {
        ESP_LOGE(TAG, "WiFi连接失败: %s", esp_err_to_name(ret));
    }
    return ret;
}

bool wifi_connect_is_connected(void)
{
    return wifi_manager_is_connect();
}

// 内部函数：更新单个屏幕的WiFi图标（不加锁，在LVGL任务中调用）
static void update_wifi_icon_internal(lv_obj_t *screen, lv_obj_t *img, const lv_image_dsc_t *connected_img,
                                       const lv_image_dsc_t *disconnected_img, bool connected)
{
    if (screen && img && lv_obj_is_valid(screen) && lv_obj_is_valid(img) &&
        lv_obj_check_type(img, &lv_image_class) && lv_obj_get_screen(img) == screen) {
        const lv_image_dsc_t *target = connected ? connected_img : disconnected_img;
        if (lv_image_get_src(img) != target) lv_image_set_src(img, target);
    }
}

// 不加锁版本，在LVGL任务内直接调用
void wifi_connect_update_icon_nolock(lv_ui *ui, bool connected)
{
    if (!ui) return;
    
    // 更新screen_wifi界面的图标
    update_wifi_icon_internal(ui->screen_wifi, ui->screen_wifi_img_wi_close,
                              &_wifi_RGB565A8_17x16, &_wifi_close_RGB565A8_23x26, connected);
    
    // 更新screen_home界面的图标
    update_wifi_icon_internal(ui->screen_home, ui->screen_home_img_wifi,
                              &_wifi_RGB565A8_17x16, &_wifi_close_RGB565A8_20x24, connected);
    
    // 更新screen_weather界面的图标
    update_wifi_icon_internal(ui->screen_weather, ui->screen_weather_img_wificlose,
                              &_wifi_RGB565A8_17x16, &_wifi_close_RGB565A8_20x24, connected);

    // AI 状态页沿用同一 WiFi 连接状态，避免固定显示断开图标。
    update_wifi_icon_internal(ui->screen_AI, ui->screen_AI_img_wifi,
                              &_wifi_RGB565A8_17x16, &_wifi_close_RGB565A8_20x20, connected);
    
    ESP_LOGD(TAG, "WiFi图标已更新(nolock): %s", connected ? "已连接" : "断开");
}

void wifi_connect_update_icon(lv_ui *ui, bool connected)
{
    if (!ui) return;
    
    extern SemaphoreHandle_t lvgl_mutex;
    if (xSemaphoreTake(lvgl_mutex, pdMS_TO_TICKS(100)) == pdTRUE) {
        wifi_connect_update_icon_nolock(ui, connected);
        xSemaphoreGive(lvgl_mutex);
    } else {
        ESP_LOGW(TAG, "无法获取lvgl_mutex，图标更新失败");
    }
}

// ==================== 事件处理 ====================

// 扫描按钮事件回调
static void wifi_btn_scan_event_cb(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    if (code != LV_EVENT_CLICKED) return;
    
    lv_ui *ui = lv_event_get_user_data(e);
    ESP_LOGI(TAG, "扫描按钮点击");
    wifi_connect_scan(ui);
}

void wifi_connect_register_wifi_events(lv_ui *ui)
{
    if (!ui) return;
    g_selected_index = -1;
    lv_obj_add_event_cb(ui->screen_wifi, wifi_screen_deleted_cb, LV_EVENT_DELETE, NULL);
    
    // 注册扫描按钮事件
    if (ui->screen_wifi_btn_scanf && lv_obj_is_valid(ui->screen_wifi_btn_scanf)) {
        lv_obj_add_event_cb(ui->screen_wifi_btn_scanf, wifi_btn_scan_event_cb, LV_EVENT_CLICKED, ui);
        ESP_LOGI(TAG, "扫描按钮事件已注册");
    }
    
    // 检查WiFi连接状态并更新图标 (在LVGL任务内，不需要加锁)
    wifi_connect_update_icon_nolock(ui, wifi_connect_is_connected());
    
    ESP_LOGI(TAG, "screen_wifi事件已注册");
}

void wifi_connect_register_connect_events(lv_ui *ui)
{
    if (!ui) return;
    
    // 更新标签显示选中的WiFi名称
    if (g_selected_index >= 0 && g_selected_index < WIFI_LIST_MAX_COUNT) {
        const char *ssid = g_wifi_ssid_list[g_selected_index];
        if (strlen(ssid) > 0 && ui->screen_wifi_connect_label_wifiname &&
            lv_obj_is_valid(ui->screen_wifi_connect_label_wifiname)) {
            lv_label_set_text(ui->screen_wifi_connect_label_wifiname, ssid);
        }
    }
    
    // 清空密码输入框
    if (ui->screen_wifi_connect_ta_input && lv_obj_is_valid(ui->screen_wifi_connect_ta_input)) {
        lv_textarea_set_text(ui->screen_wifi_connect_ta_input, "");
    }
    
    ESP_LOGI(TAG, "screen_wifi_connect事件已注册");
}
