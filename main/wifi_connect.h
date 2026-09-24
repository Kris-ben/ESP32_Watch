/**
 * @file wifi_connect.h
 * @brief WiFi扫描和连接模块
 */

#ifndef WIFI_CONNECT_H
#define WIFI_CONNECT_H

#include <stdint.h>
#include <stdbool.h>
#include "esp_err.h"
#include "gui_guider.h"

#ifdef __cplusplus
extern "C" {
#endif

#define WIFI_LIST_MAX_COUNT 5  // 列表显示的WiFi数量

/**
 * @brief 初始化WiFi连接模块
 */
esp_err_t wifi_connect_init(void);

/**
 * @brief 开始WiFi扫描
 * @param ui UI句柄
 */
void wifi_connect_scan(lv_ui *ui);

/**
 * @brief 选中有效的WiFi热点
 * @param index WiFi索引 (0-4)
 * @return true=该项有热点，false=空项或无效索引
 */
bool wifi_connect_set_selected_index(int index);

/**
 * @brief 获取当前选中的WiFi索引
 * @return WiFi索引 (0-4)
 */
int wifi_connect_get_selected_index(void);

/** @brief 获取扫描列表中的热点名；索引无效时返回空字符串。 */
const char *wifi_connect_get_scanned_ssid(int index);

/**
 * @brief 使用输入的密码连接WiFi
 * @param ui UI句柄
 */
esp_err_t wifi_connect_with_password(lv_ui *ui);

/**
 * @brief 检查WiFi是否已连接
 * @return true=已连接, false=未连接
 */
bool wifi_connect_is_connected(void);

/**
 * @brief 更新WiFi状态图标
 * @param ui UI句柄
 * @param connected true=显示已连接图标, false=显示断开图标
 */
void wifi_connect_update_icon(lv_ui *ui, bool connected);

/**
 * @brief 更新WiFi状态图标（不加锁版本，在LVGL任务内调用）
 * @param ui UI句柄
 * @param connected true=显示已连接图标, false=显示断开图标
 */
void wifi_connect_update_icon_nolock(lv_ui *ui, bool connected);

/**
 * @brief 注册screen_wifi界面的WiFi扫描事件
 * @param ui UI句柄
 */
void wifi_connect_register_wifi_events(lv_ui *ui);

/**
 * @brief 初始化连接页选中的热点名称和密码输入
 * @param ui UI句柄
 */
void wifi_connect_register_connect_events(lv_ui *ui);

#ifdef __cplusplus
}
#endif

#endif // WIFI_CONNECT_H
