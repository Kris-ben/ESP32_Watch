#ifndef __WETHER_H__
#define __WETHER_H__    

#include <stdbool.h>

#include "gui_guider.h"
#include "esp_err.h"

// 注意：本头文件同时被 C（weather.c）和 C++（voice_dialog.cpp）包含，
// 必须加 extern "C" 保护，否则 C++ 侧会按改名(mangle)规则去链接，找不到 C 符号。
#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
    int high_temp;
    int low_temp;
    char code[4];
    char text_day[16];   // 天气描述，如"多云"、"晴"
    char date[16];       // 日期，如"2026-02-05"
}weather_data_pkt_t;

// 高德天气实时数据结构
typedef struct
{
    int temperature;     // 温度
    int humidity;        // 湿度
    char weather[32];    // 天气现象
    char city[32];       // 城市名
}amap_realtime_weather_t;

void weather_start(void);

/* 在 LVGL 线程或持有 lvgl_mutex 时调用，恢复天气页的缓存数据。 */
void weather_refresh_screen(lv_ui *ui);

// 获取高德实时天气数据
esp_err_t amap_get_realtime_weather(void);

// 更新主界面温湿度显示
void update_home_temp_humidity(lv_ui *ui);

/**
 * @brief 读取最近一次获取到的实时天气数据（高德）
 *
 * 供语音助手等模块直接播报用：问一句天气不必再去调用大模型。
 * @param out 输出参数
 * @return true 数据有效；false 表示尚未成功获取过
 */
bool weather_get_realtime(amap_realtime_weather_t *out);

#ifdef __cplusplus
}
#endif

#endif
