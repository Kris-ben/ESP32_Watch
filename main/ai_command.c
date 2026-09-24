/**
 * @file ai_command.c
 * @brief AI指令解析和控制模块实现
 */

#include "ai_command.h"
#include "esp_log.h"
#include "system_settings.h"
#include "alarm_clock.h"
#include "weather.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>

static const char *TAG = "AICommand";

// UI句柄
static lv_ui *g_ui = NULL;

// LVGL互斥锁
extern SemaphoreHandle_t lvgl_mutex;

// 前向声明
static esp_err_t cmd_set_brightness(uint8_t value);
static esp_err_t cmd_set_volume(uint8_t value);
static esp_err_t cmd_set_alarm(uint8_t index, uint8_t hour, uint8_t minute, bool enabled);
static esp_err_t cmd_enable_alarm(uint8_t index, bool enabled);
static esp_err_t cmd_switch_screen(const char *name);
static esp_err_t cmd_report_weather(void);

esp_err_t ai_command_init(lv_ui *ui)
{
    g_ui = ui;
    ESP_LOGI(TAG, "AI指令模块初始化完成");
    return ESP_OK;
}

/**
 * @brief 解析单个指令字符串
 * @param cmd_str 指令字符串（不含[CMD: 和 ]）
 * @param cmd 输出指令结构
 * @return true=解析成功
 */
static bool parse_command(const char *cmd_str, ai_command_t *cmd)
{
    char buf[128];
    strncpy(buf, cmd_str, sizeof(buf) - 1);
    buf[sizeof(buf) - 1] = '\0';
    
    // 分割参数
    char *parts[8] = {0};
    int part_count = 0;
    char *token = strtok(buf, ":");
    while (token && part_count < 8) {
        parts[part_count++] = token;
        token = strtok(NULL, ":");
    }
    
    if (part_count == 0) return false;
    
    // 转换命令名为大写
    for (char *p = parts[0]; *p; p++) *p = toupper((unsigned char)*p);
    
    // 解析命令类型
    if (strcmp(parts[0], "BRIGHTNESS") == 0 && part_count >= 2) {
        cmd->type = AI_CMD_BRIGHTNESS;
        cmd->params.brightness.value = (uint8_t)atoi(parts[1]);
        if (cmd->params.brightness.value > 100) cmd->params.brightness.value = 100;
        return true;
    }
    else if (strcmp(parts[0], "VOLUME") == 0 && part_count >= 2) {
        cmd->type = AI_CMD_VOLUME;
        cmd->params.volume.value = (uint8_t)atoi(parts[1]);
        if (cmd->params.volume.value > 100) cmd->params.volume.value = 100;
        return true;
    }
    else if (strcmp(parts[0], "ALARM") == 0 && part_count >= 2) {
        int index = atoi(parts[1]);
        if (index < 0 || index >= ALARM_COUNT) return false;
        
        if (part_count >= 5) {
            // [CMD:ALARM:index:hour:minute:ON/OFF]
            cmd->type = AI_CMD_ALARM_SET;
            cmd->params.alarm.index = (uint8_t)index;
            cmd->params.alarm.hour = (uint8_t)atoi(parts[2]);
            cmd->params.alarm.minute = (uint8_t)atoi(parts[3]);
            
            // 转大写比较
            char enabled_str[8] = {0};
            strncpy(enabled_str, parts[4], sizeof(enabled_str) - 1);
            for (char *p = enabled_str; *p; p++) *p = toupper((unsigned char)*p);
            cmd->params.alarm.enabled = (strcmp(enabled_str, "ON") == 0 || strcmp(enabled_str, "1") == 0);
            return true;
        }
        else if (part_count >= 3) {
            // [CMD:ALARM:index:ON/OFF]
            cmd->type = AI_CMD_ALARM_ENABLE;
            cmd->params.alarm.index = (uint8_t)index;
            
            char enabled_str[8] = {0};
            strncpy(enabled_str, parts[2], sizeof(enabled_str) - 1);
            for (char *p = enabled_str; *p; p++) *p = toupper((unsigned char)*p);
            cmd->params.alarm.enabled = (strcmp(enabled_str, "ON") == 0 || strcmp(enabled_str, "1") == 0);
            return true;
        }
    }
    else if (strcmp(parts[0], "SCREEN") == 0 && part_count >= 2) {
        cmd->type = AI_CMD_SCREEN;
        strncpy(cmd->params.screen.name, parts[1], sizeof(cmd->params.screen.name) - 1);
        // 转小写
        for (char *p = cmd->params.screen.name; *p; p++) *p = tolower((unsigned char)*p);
        return true;
    }
    else if (strcmp(parts[0], "WEATHER") == 0) {
        cmd->type = AI_CMD_WEATHER;
        return true;
    }
    else if (strcmp(parts[0], "TIME") == 0) {
        cmd->type = AI_CMD_TIME;
        return true;
    }
    
    return false;
}

int ai_command_parse_and_execute(const char *response, char *clean_response, size_t max_len)
{
    if (!response) return 0;
    
    int cmd_count = 0;
    const char *src = response;
    char *dst = clean_response;
    size_t remaining = max_len > 0 ? max_len - 1 : 0;
    
    while (*src) {
        // 查找 [CMD:
        if (strncmp(src, "[CMD:", 5) == 0) {
            const char *cmd_start = src + 5;
            const char *cmd_end = strchr(cmd_start, ']');
            
            if (cmd_end) {
                // 提取指令内容
                size_t cmd_len = cmd_end - cmd_start;
                char cmd_str[128];
                if (cmd_len < sizeof(cmd_str)) {
                    strncpy(cmd_str, cmd_start, cmd_len);
                    cmd_str[cmd_len] = '\0';
                    
                    // 解析并执行
                    ai_command_t cmd = {0};
                    if (parse_command(cmd_str, &cmd)) {
                        ESP_LOGI(TAG, "解析到指令: %s", cmd_str);
                        if (ai_command_execute(&cmd) == ESP_OK) {
                            cmd_count++;
                        }
                    } else {
                        ESP_LOGW(TAG, "无法解析指令: %s", cmd_str);
                    }
                }
                
                // 跳过整个指令
                src = cmd_end + 1;
                continue;
            }
        }
        
        // 复制普通字符到clean_response
        if (dst && remaining > 0) {
            *dst++ = *src;
            remaining--;
        }
        src++;
    }
    
    if (dst) *dst = '\0';
    
    if (cmd_count > 0) {
        ESP_LOGI(TAG, "共执行 %d 条指令", cmd_count);
    }
    
    return cmd_count;
}

esp_err_t ai_command_execute(const ai_command_t *cmd)
{
    if (!cmd) return ESP_ERR_INVALID_ARG;
    
    switch (cmd->type) {
        case AI_CMD_BRIGHTNESS:
            return cmd_set_brightness(cmd->params.brightness.value);
            
        case AI_CMD_VOLUME:
            return cmd_set_volume(cmd->params.volume.value);
            
        case AI_CMD_ALARM_SET:
            return cmd_set_alarm(cmd->params.alarm.index, 
                                 cmd->params.alarm.hour,
                                 cmd->params.alarm.minute,
                                 cmd->params.alarm.enabled);
            
        case AI_CMD_ALARM_ENABLE:
            return cmd_enable_alarm(cmd->params.alarm.index,
                                    cmd->params.alarm.enabled);
            
        case AI_CMD_SCREEN:
            return cmd_switch_screen(cmd->params.screen.name);
            
        case AI_CMD_WEATHER:
            return cmd_report_weather();
            
        case AI_CMD_TIME:
            // 报时功能（可扩展为TTS播报）
            ESP_LOGI(TAG, "执行报时");
            return ESP_OK;
            
        default:
            return ESP_ERR_NOT_SUPPORTED;
    }
}

// ==================== 具体命令实现 ====================

static esp_err_t cmd_set_brightness(uint8_t value)
{
    ESP_LOGI(TAG, "设置亮度: %d%%", value);
    esp_err_t ret = system_set_brightness(value);
    
    // 同步更新UI滑块（如果在screen_down界面）
    if (g_ui && lvgl_mutex) {
        if (xSemaphoreTake(lvgl_mutex, pdMS_TO_TICKS(100)) == pdTRUE) {
            if (g_ui->screen_down_slider_brightness && 
                lv_obj_is_valid(g_ui->screen_down_slider_brightness)) {
                lv_slider_set_value(g_ui->screen_down_slider_brightness, value, LV_ANIM_ON);
            }
            xSemaphoreGive(lvgl_mutex);
        }
    }
    
    return ret;
}

static esp_err_t cmd_set_volume(uint8_t value)
{
    ESP_LOGI(TAG, "设置音量: %d%%", value);
    esp_err_t ret = system_set_volume(value);
    
    // 同步更新UI滑块
    if (g_ui && lvgl_mutex) {
        if (xSemaphoreTake(lvgl_mutex, pdMS_TO_TICKS(100)) == pdTRUE) {
            if (g_ui->screen_down_slider_1 && 
                lv_obj_is_valid(g_ui->screen_down_slider_1)) {
                lv_slider_set_value(g_ui->screen_down_slider_1, value, LV_ANIM_ON);
            }
            xSemaphoreGive(lvgl_mutex);
        }
    }
    
    return ret;
}

static esp_err_t cmd_set_alarm(uint8_t index, uint8_t hour, uint8_t minute, bool enabled)
{
    ESP_LOGI(TAG, "设置闹钟%d: %02d:%02d %s", index, hour, minute, enabled ? "启用" : "禁用");
    
    esp_err_t ret = alarm_set_time(index, hour, minute);
    if (ret == ESP_OK) {
        ret = alarm_set_enabled(index, enabled);
    }
    
    // 更新UI显示
    if (g_ui && lvgl_mutex) {
        if (xSemaphoreTake(lvgl_mutex, pdMS_TO_TICKS(100)) == pdTRUE) {
            // 调用闹钟模块的UI更新函数
            alarm_update_ui(g_ui);
            xSemaphoreGive(lvgl_mutex);
        }
    }
    
    return ret;
}

static esp_err_t cmd_enable_alarm(uint8_t index, bool enabled)
{
    ESP_LOGI(TAG, "%s闹钟%d", enabled ? "启用" : "禁用", index);
    
    esp_err_t ret = alarm_set_enabled(index, enabled);
    
    // 更新开关UI
    if (g_ui && lvgl_mutex) {
        if (xSemaphoreTake(lvgl_mutex, pdMS_TO_TICKS(100)) == pdTRUE) {
            lv_obj_t *sw = NULL;
            switch (index) {
                case 0: sw = g_ui->screen_clock_sw_1; break;
                case 1: sw = g_ui->screen_clock_sw_2; break;
                case 2: sw = g_ui->screen_clock_sw_3; break;
                case 3: sw = g_ui->screen_clock_sw_4; break;
            }
            if (sw && lv_obj_is_valid(sw)) {
                if (enabled) {
                    lv_obj_add_state(sw, LV_STATE_CHECKED);
                } else {
                    lv_obj_remove_state(sw, LV_STATE_CHECKED);
                }
            }
            xSemaphoreGive(lvgl_mutex);
        }
    }
    
    return ret;
}

static bool *screen_deleted_flag(lv_ui *ui, lv_obj_t *screen)
{
    if (screen == ui->screen_home) return &ui->screen_home_del;
    if (screen == ui->screen_selete) return &ui->screen_selete_del;
    if (screen == ui->screen_Rli) return &ui->screen_Rli_del;
    if (screen == ui->screen_AI) return &ui->screen_AI_del;
    if (screen == ui->screen_wifi) return &ui->screen_wifi_del;
    if (screen == ui->screen_wifi_connect) return &ui->screen_wifi_connect_del;
    if (screen == ui->screen_weather) return &ui->screen_weather_del;
    if (screen == ui->screen_down) return &ui->screen_down_del;
    if (screen == ui->screen_set_clock) return &ui->screen_set_clock_del;
    if (screen == ui->screen_clock) return &ui->screen_clock_del;
    if (screen == ui->screen_1) return &ui->screen_1_del;
    if (screen == ui->screen_musiclist) return &ui->screen_musiclist_del;
    if (screen == ui->screen_play) return &ui->screen_play_del;
    return NULL;
}

static esp_err_t cmd_switch_screen(const char *name)
{
    if (!g_ui || !lvgl_mutex) return ESP_ERR_INVALID_STATE;
    ESP_LOGI(TAG, "切换到界面: %s", name);
    if (xSemaphoreTake(lvgl_mutex, pdMS_TO_TICKS(200)) != pdTRUE) return ESP_ERR_TIMEOUT;

    lv_obj_t **target = NULL;
    bool *target_deleted = NULL;
    ui_setup_scr_t setup = NULL;
    if (strcmp(name, "home") == 0) {
        target = &g_ui->screen_home; target_deleted = &g_ui->screen_home_del; setup = setup_scr_screen_home;
    } else if (strcmp(name, "weather") == 0) {
        target = &g_ui->screen_weather; target_deleted = &g_ui->screen_weather_del; setup = setup_scr_screen_weather;
    } else if (strcmp(name, "wifi") == 0) {
        target = &g_ui->screen_wifi; target_deleted = &g_ui->screen_wifi_del; setup = setup_scr_screen_wifi;
    } else if (strcmp(name, "clock") == 0 || strcmp(name, "alarm") == 0) {
        target = &g_ui->screen_clock; target_deleted = &g_ui->screen_clock_del; setup = setup_scr_screen_clock;
    } else if (strcmp(name, "ai") == 0) {
        target = &g_ui->screen_AI; target_deleted = &g_ui->screen_AI_del; setup = setup_scr_screen_AI;
    } else if (strcmp(name, "settings") == 0 || strcmp(name, "down") == 0) {
        target = &g_ui->screen_down; target_deleted = &g_ui->screen_down_del; setup = setup_scr_screen_down;
    } else if (strcmp(name, "music") == 0) {
        target = &g_ui->screen_musiclist; target_deleted = &g_ui->screen_musiclist_del; setup = setup_scr_screen_musiclist;
    } else if (strcmp(name, "calendar") == 0 || strcmp(name, "rli") == 0) {
        target = &g_ui->screen_Rli; target_deleted = &g_ui->screen_Rli_del; setup = setup_scr_screen_Rli;
    }

    if (!target) {
        xSemaphoreGive(lvgl_mutex);
        ESP_LOGW(TAG, "未知界面: %s", name);
        return ESP_ERR_NOT_FOUND;
    }

    lv_obj_t *active = lv_screen_active();
    if (active == *target) {
        xSemaphoreGive(lvgl_mutex);
        return ESP_OK;
    }
    if (!*target_deleted && (!*target || !lv_obj_is_valid(*target))) {
        *target_deleted = true;
    }

    // 语音切页也必须更新真实旧页面标记，否则下次会加载已删除的页面。
    bool unused_deleted = false;
    bool *old_deleted = screen_deleted_flag(g_ui, active);
    ui_load_scr_animation(g_ui, target, *target_deleted, old_deleted ? old_deleted : &unused_deleted,
                          setup, LV_SCR_LOAD_ANIM_FADE_ON, 200, 0, false, true);
    xSemaphoreGive(lvgl_mutex);
    return ESP_OK;
}

static esp_err_t cmd_report_weather(void)
{
    ESP_LOGI(TAG, "查询天气");
    
    // 获取天气数据（如果已有缓存）
    // 这里可以调用weather模块获取数据
    // weather_get_current() 等
    
    return ESP_OK;
}

int ai_command_get_system_prompt(char *buffer, size_t max_len)
{
    if (!buffer || max_len == 0) return 0;
    
    // 获取当前系统状态
    uint8_t brightness = system_get_brightness();
    uint8_t volume = system_get_volume();
    
    // 获取闹钟状态
    char alarm_info[256] = "";
    int offset = 0;
    for (int i = 0; i < ALARM_COUNT; i++) {
        uint8_t hour, minute;
        if (alarm_get_time(i, &hour, &minute) == ESP_OK) {
            offset += snprintf(alarm_info + offset, sizeof(alarm_info) - offset,
                               "闹钟%d: %02d:%02d %s; ", 
                               i + 1, hour, minute,
                               alarm_is_enabled(i) ? "开启" : "关闭");
        }
    }
    
    // 构建系统提示
    int len = snprintf(buffer, max_len,
        "你是一个智能手表助手。你可以通过特殊指令控制设备。\n"
        "可用指令格式(放在回复末尾):\n"
        "- [CMD:BRIGHTNESS:0-100] 设置亮度\n"
        "- [CMD:VOLUME:0-100] 设置音量\n"
        "- [CMD:ALARM:索引:时:分:ON/OFF] 设置闹钟(索引0-3)\n"
        "- [CMD:ALARM:索引:ON/OFF] 开关闹钟\n"
        "- [CMD:SCREEN:界面名] 切换界面(home/weather/wifi/clock/ai/settings/music/calendar)\n"
        "\n当前状态: 亮度%d%%, 音量%d%%, %s\n"
        "用户说中文时请用中文回复，执行控制时先确认再在末尾添加指令。",
        brightness, volume, alarm_info);
    
    return len;
}
