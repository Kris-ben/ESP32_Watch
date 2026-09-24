#include "app_swipe_nav.h"

#include <stdint.h>

#include "gui_guider.h"

static void enable_gesture_bubble(lv_obj_t *parent)
{
    uint32_t count = lv_obj_get_child_count(parent);
    for (uint32_t i = 0; i < count; ++i) {
        lv_obj_t *child = lv_obj_get_child(parent, i);
        lv_obj_add_flag(child, LV_OBJ_FLAG_GESTURE_BUBBLE);
        enable_gesture_bubble(child);
    }
}

void app_swipe_nav_go_back(lv_obj_t *screen, app_swipe_target_t target)
{
    lv_ui *ui = &guider_ui;
    lv_obj_t **next_screen = NULL;
    bool *old_screen_deleted = NULL;
    bool next_screen_deleted = true;
    ui_setup_scr_t setup = NULL;

    switch (target) {
    case APP_SWIPE_TO_LAUNCHER:
        next_screen = &ui->screen_1;
        next_screen_deleted = ui->screen_1_del;
        setup = setup_scr_screen_1;
        break;
    case APP_SWIPE_TO_WIFI:
        next_screen = &ui->screen_wifi;
        next_screen_deleted = ui->screen_wifi_del;
        setup = setup_scr_screen_wifi;
        break;
    case APP_SWIPE_TO_CLOCK:
        next_screen = &ui->screen_clock;
        next_screen_deleted = ui->screen_clock_del;
        setup = setup_scr_screen_clock;
        break;
    case APP_SWIPE_TO_MUSICLIST:
        next_screen = &ui->screen_musiclist;
        next_screen_deleted = ui->screen_musiclist_del;
        setup = setup_scr_screen_musiclist;
        break;
    default:
        return;
    }

    if (screen == ui->screen_AI) old_screen_deleted = &ui->screen_AI_del;
    else if (screen == ui->screen_weather) old_screen_deleted = &ui->screen_weather_del;
    else if (screen == ui->screen_wifi) old_screen_deleted = &ui->screen_wifi_del;
    else if (screen == ui->screen_Rli) old_screen_deleted = &ui->screen_Rli_del;
    else if (screen == ui->screen_clock) old_screen_deleted = &ui->screen_clock_del;
    else if (screen == ui->screen_musiclist) old_screen_deleted = &ui->screen_musiclist_del;
    else if (screen == ui->screen_play) old_screen_deleted = &ui->screen_play_del;
    else if (screen == ui->screen_wifi_connect) old_screen_deleted = &ui->screen_wifi_connect_del;
    else if (screen == ui->screen_set_clock) old_screen_deleted = &ui->screen_set_clock_del;
    else return;

    ui_load_scr_animation(ui, next_screen, next_screen_deleted, old_screen_deleted,
                          setup, LV_SCR_LOAD_ANIM_MOVE_RIGHT, 200, 0, false, true);
}

static void swipe_back_cb(lv_event_t *event)
{
    lv_indev_t *indev = lv_indev_active();
    if (!indev || lv_indev_get_gesture_dir(indev) != LV_DIR_RIGHT) return;
    lv_indev_wait_release(indev);
    app_swipe_nav_go_back(lv_event_get_current_target_obj(event),
                          (app_swipe_target_t)(uintptr_t)lv_event_get_user_data(event));
}

void app_swipe_nav_bind(lv_obj_t *screen, lv_obj_t *old_back_button,
                        app_swipe_target_t target)
{
    if (!screen) return;
    if (old_back_button) lv_obj_add_flag(old_back_button, LV_OBJ_FLAG_HIDDEN);
    lv_obj_remove_flag(screen, LV_OBJ_FLAG_SCROLLABLE);
    enable_gesture_bubble(screen);
    lv_obj_add_event_cb(screen, swipe_back_cb, LV_EVENT_GESTURE,
                        (void *)(uintptr_t)target);
}
