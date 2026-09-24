/**
 * @file system_settings.c
 * @brief 系统设置模块实现 - 音量和亮度控制
 */

#include "system_settings.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "nvs.h"
#include "board_display.h"
#include "display_st7789v.h"
#include "music_player.h"

static const char *TAG = "SysSettings";

// 设置存储
static uint8_t g_brightness = 80;  // 默认亮度 80%
static uint8_t g_volume = 50;      // 默认音量 50%

// NVS命名空间
#define NVS_NAMESPACE "settings"
#define NVS_KEY_BRIGHTNESS "brightness"
#define NVS_KEY_VOLUME "volume"

/**
 * @brief 从NVS加载设置
 */
static void load_settings_from_nvs(void)
{
    nvs_handle_t nvs_handle;
    esp_err_t err = nvs_open(NVS_NAMESPACE, NVS_READONLY, &nvs_handle);
    if (err == ESP_OK) {
        uint8_t val;
        if (nvs_get_u8(nvs_handle, NVS_KEY_BRIGHTNESS, &val) == ESP_OK) {
            g_brightness = val;
            ESP_LOGI(TAG, "从NVS加载亮度: %d%%", g_brightness);
        }
        if (nvs_get_u8(nvs_handle, NVS_KEY_VOLUME, &val) == ESP_OK) {
            g_volume = val;
            ESP_LOGI(TAG, "从NVS加载音量: %d%%", g_volume);
        }
        nvs_close(nvs_handle);
    }
}

/**
 * @brief 保存设置到NVS
 */
static void save_settings_to_nvs(void)
{
    nvs_handle_t nvs_handle;
    esp_err_t err = nvs_open(NVS_NAMESPACE, NVS_READWRITE, &nvs_handle);
    if (err == ESP_OK) {
        nvs_set_u8(nvs_handle, NVS_KEY_BRIGHTNESS, g_brightness);
        nvs_set_u8(nvs_handle, NVS_KEY_VOLUME, g_volume);
        nvs_commit(nvs_handle);
        nvs_close(nvs_handle);
    }
}

esp_err_t system_settings_init(void)
{
    ESP_LOGI(TAG, "初始化系统设置...");
    
    // 从NVS加载保存的设置
    load_settings_from_nvs();
    
    // 应用亮度设置
    system_set_brightness(g_brightness);
    system_preview_volume(g_volume);
    
    ESP_LOGI(TAG, "系统设置初始化完成: 亮度=%d%%, 音量=%d%%", g_brightness, g_volume);
    return ESP_OK;
}

esp_err_t system_preview_brightness(uint8_t percent)
{
    if (percent > 100) percent = 100;
    
    // 设置最小亮度防止全黑
    if (percent < 5) percent = 5;
    
    // 获取display句柄并设置亮度
    display_st7789v_t *disp = board_display_get_display();
    if (disp) {
        esp_err_t err = display_st7789v_backlight_set_percent(disp, percent);
        if (err != ESP_OK) {
            ESP_LOGE(TAG, "设置亮度失败: %s", esp_err_to_name(err));
            return err;
        }
    }
    g_brightness = percent;
    return ESP_OK;
}

esp_err_t system_set_brightness(uint8_t percent)
{
    esp_err_t err = system_preview_brightness(percent);
    if (err != ESP_OK) return err;

    // 保存到NVS
    save_settings_to_nvs();
    ESP_LOGI(TAG, "亮度设置为: %d%%", g_brightness);
    return ESP_OK;
}

uint8_t system_get_brightness(void)
{
    return g_brightness;
}

esp_err_t system_preview_volume(uint8_t percent)
{
    if (percent > 100) percent = 100;
    g_volume = percent;
    music_player_set_volume(percent);
    return ESP_OK;
}

esp_err_t system_set_volume(uint8_t percent)
{
    esp_err_t err = system_preview_volume(percent);
    if (err != ESP_OK) return err;

    // 保存到NVS
    save_settings_to_nvs();
    ESP_LOGI(TAG, "音量设置为: %d%%", g_volume);
    return ESP_OK;
}

uint8_t system_get_volume(void)
{
    return g_volume;
}
