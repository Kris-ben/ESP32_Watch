#include "app_page_layout.h"

void app_page_layout_apply(lv_ui *ui, app_page_layout_t page)
{
    if (!ui) return;

    switch (page) {
    case APP_LAYOUT_WIFI:
        lv_obj_set_size(ui->screen_wifi_list_wifi, 235, 177);
        lv_obj_set_pos(ui->screen_wifi_btn_scanf, 82, 229);
        lv_obj_set_size(ui->screen_wifi_btn_scanf, 76, 38);
        break;

    case APP_LAYOUT_WIFI_CONNECT:
        lv_obj_set_pos(ui->screen_wifi_connect_btn_set, 82, 207);
        lv_obj_set_size(ui->screen_wifi_connect_btn_set, 76, 36);
        break;

    case APP_LAYOUT_SET_CLOCK:
        lv_obj_set_pos(ui->screen_set_clock_btn_set, 12, 234);
        lv_obj_set_size(ui->screen_set_clock_btn_set, 216, 43);
        break;

    case APP_LAYOUT_WEATHER:
        lv_obj_set_y(ui->screen_weather_label_today, 75);
        lv_obj_set_y(ui->screen_weather_label_tomorrow, 75);
        lv_obj_set_y(ui->screen_weather_label_later, 75);
        lv_obj_set_y(ui->screen_weather_img_today, 112);
        lv_obj_set_y(ui->screen_weather_img_tomorrow, 112);
        lv_obj_set_y(ui->screen_weather_img_later, 112);
        lv_obj_set_y(ui->screen_weather_label_today1, 190);
        lv_obj_set_y(ui->screen_weather_label_tomorrow2, 190);
        lv_obj_set_y(ui->screen_weather_label_later2, 190);
        lv_obj_set_y(ui->screen_weather_label_today_temp, 230);
        lv_obj_set_y(ui->screen_weather_label_tomorrow_temp, 230);
        lv_obj_set_y(ui->screen_weather_label_later_temp, 230);
        break;

    case APP_LAYOUT_CALENDAR:
        lv_obj_set_pos(ui->screen_Rli_calendar_1, 0, 0);
        lv_obj_set_size(ui->screen_Rli_calendar_1, 240, 280);
        break;

    case APP_LAYOUT_CLOCK:
    {
        lv_obj_set_pos(ui->screen_clock_list_clock, 8, 54);
        lv_obj_set_size(ui->screen_clock_list_clock, 140, 224);
        lv_obj_t *items[] = {
            ui->screen_clock_list_clock_item0,
            ui->screen_clock_list_clock_item1,
            ui->screen_clock_list_clock_item2,
            ui->screen_clock_list_clock_item3,
        };
        lv_obj_t *switches[] = {
            ui->screen_clock_sw_1,
            ui->screen_clock_sw_2,
            ui->screen_clock_sw_3,
            ui->screen_clock_sw_4,
        };
        for (int i = 0; i < 4; ++i) {
            lv_obj_set_height(items[i], 52);
            lv_obj_set_style_text_font(items[i], &lv_font_ZiTiQuanWeiJunHeiW22_24, 0);
            uint32_t child_count = lv_obj_get_child_count(items[i]);
            for (uint32_t child = 0; child < child_count; ++child) {
                lv_obj_set_style_text_font(lv_obj_get_child(items[i], child),
                                           &lv_font_ZiTiQuanWeiJunHeiW22_24, 0);
            }
            lv_obj_set_pos(switches[i], 158, 60 + i * 52);
            lv_obj_set_size(switches[i], 68, 42);
            lv_obj_set_style_radius(switches[i], 21, LV_PART_MAIN);
            lv_obj_set_style_radius(switches[i], 21, LV_PART_KNOB);
        }
        break;
    }

    case APP_LAYOUT_MUSICLIST:
        lv_obj_set_pos(ui->screen_musiclist_list_music, 0, 34);
        lv_obj_set_size(ui->screen_musiclist_list_music, 240, 245);
        break;

    case APP_LAYOUT_AI:
        lv_obj_set_pos(ui->screen_AI_cont_1, 0, 0);
        lv_obj_set_size(ui->screen_AI_cont_1, 240, 27);
        lv_obj_set_style_border_width(ui->screen_AI_cont_1, 0, 0);
        lv_obj_set_pos(ui->screen_AI_digital_clock_1, 23, 4);
        lv_obj_set_size(ui->screen_AI_digital_clock_1, 66, 22);
        lv_obj_set_style_pad_top(ui->screen_AI_digital_clock_1, 0, 0);
        lv_obj_set_pos(ui->screen_AI_img_wifi, 138, 5);
        lv_obj_set_size(ui->screen_AI_img_wifi, 20, 20);
        lv_obj_set_pos(ui->screen_AI_list_battery, 166, 3);
        lv_obj_set_size(ui->screen_AI_list_battery, 64, 24);
        lv_obj_set_style_pad_all(ui->screen_AI_list_battery, 0, 0);
        lv_obj_set_pos(ui->screen_AI_label_title, 15, 31);
        lv_obj_set_size(ui->screen_AI_label_title, 180, 27);
        break;
    }
}
