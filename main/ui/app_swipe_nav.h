#pragma once

#include "lvgl.h"

typedef enum {
    APP_SWIPE_TO_LAUNCHER = 1,
    APP_SWIPE_TO_WIFI,
    APP_SWIPE_TO_CLOCK,
    APP_SWIPE_TO_MUSICLIST,
} app_swipe_target_t;

void app_swipe_nav_bind(lv_obj_t *screen, lv_obj_t *old_back_button,
                        app_swipe_target_t target);
void app_swipe_nav_go_back(lv_obj_t *screen, app_swipe_target_t target);
