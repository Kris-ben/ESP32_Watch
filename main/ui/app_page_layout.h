#pragma once

#include "gui_guider.h"

typedef enum {
    APP_LAYOUT_WIFI,
    APP_LAYOUT_WIFI_CONNECT,
    APP_LAYOUT_SET_CLOCK,
    APP_LAYOUT_WEATHER,
    APP_LAYOUT_CALENDAR,
    APP_LAYOUT_CLOCK,
    APP_LAYOUT_MUSICLIST,
    APP_LAYOUT_AI,
} app_page_layout_t;

void app_page_layout_apply(lv_ui *ui, app_page_layout_t page);
