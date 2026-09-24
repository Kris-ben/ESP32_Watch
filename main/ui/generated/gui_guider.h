/*
* Copyright 2026 NXP
* NXP Proprietary. This software is owned or controlled by NXP and may only be used strictly in
* accordance with the applicable license terms. By expressly accepting such terms or by downloading, installing,
* activating and/or otherwise using the software, you are agreeing that you have read, and that you agree to
* comply with and are bound by, such license terms.  If you do not agree to be bound by the applicable license
* terms, then you may not retain, install, activate or otherwise use the software.
*/

#ifndef GUI_GUIDER_H
#define GUI_GUIDER_H
#ifdef __cplusplus
extern "C" {
#endif

#include "lvgl.h"


typedef struct
{
  
	lv_obj_t *screen_home;
	bool screen_home_del;
	lv_obj_t *screen_home_label_city;
	lv_obj_t *screen_home_digital_clock;
	lv_obj_t *screen_home_label_data;
	lv_obj_t *screen_home_label_week;
	lv_obj_t *screen_home_arc_temp;
	lv_obj_t *screen_home_arc_heart_rate;
	lv_obj_t *screen_home_arc_humidity;
	lv_obj_t *screen_home_label_temp;
	lv_obj_t *screen_home_label_heart_rate;
	lv_obj_t *screen_home_label_humidity;
	lv_obj_t *screen_home_label_te;
	lv_obj_t *screen_home_label_hr;
	lv_obj_t *screen_home_label_him;
	lv_obj_t *screen_home_img_wifi;
	lv_obj_t *screen_home_img_foot;
	lv_obj_t *screen_home_label_7;
	lv_obj_t *screen_home_label_foot;
	lv_obj_t *screen_home_img_pa;
	lv_obj_t *screen_home_label_9;
	lv_obj_t *screen_home_label_pa;
	lv_obj_t *screen_home_list_bettery;
	lv_obj_t *screen_home_list_bettery_item0;
	lv_obj_t *screen_selete;
	bool screen_selete_del;
	lv_obj_t *screen_selete_imgbtn_wifi;
	lv_obj_t *screen_selete_imgbtn_wifi_label;
	lv_obj_t *screen_selete_imgbtn_weather;
	lv_obj_t *screen_selete_imgbtn_weather_label;
	lv_obj_t *screen_selete_imgbtn_rli;
	lv_obj_t *screen_selete_imgbtn_rli_label;
	lv_obj_t *screen_selete_imgbtn_AI;
	lv_obj_t *screen_selete_imgbtn_AI_label;
	lv_obj_t *screen_selete_imgbtn_clock;
	lv_obj_t *screen_selete_imgbtn_clock_label;
	lv_obj_t *screen_selete_imgbtn_music;
	lv_obj_t *screen_selete_imgbtn_music_label;
	lv_obj_t *screen_Rli;
	bool screen_Rli_del;
	lv_obj_t *screen_Rli_win_1;
	lv_obj_t *screen_Rli_win_1_item0;
	lv_obj_t *screen_Rli_calendar_1;
	lv_obj_t *screen_Rli_btn_return;
	lv_obj_t *screen_Rli_btn_return_label;
	lv_obj_t *screen_AI;
	bool screen_AI_del;
	lv_obj_t *screen_AI_btn_spreak;
	lv_obj_t *screen_AI_btn_spreak_label;
	lv_obj_t *screen_AI_label_ai;
	lv_obj_t *screen_AI_label_user;
	lv_obj_t *screen_AI_img_ai;
	lv_obj_t *screen_AI_img_user;
	lv_obj_t *screen_AI_label_title;
	lv_obj_t *screen_AI_imgbtn_return;
	lv_obj_t *screen_AI_imgbtn_return_label;
	lv_obj_t *screen_AI_cont_1;
	lv_obj_t *screen_AI_list_battery;
	lv_obj_t *screen_AI_list_battery_item0;
	lv_obj_t *screen_AI_img_wifi;
	lv_obj_t *screen_AI_digital_clock_1;
	lv_obj_t *screen_wifi;
	bool screen_wifi_del;
	lv_obj_t *screen_wifi_list_wifi;
	lv_obj_t *screen_wifi_list_wifi_item0;
	lv_obj_t *screen_wifi_list_wifi_item1;
	lv_obj_t *screen_wifi_list_wifi_item2;
	lv_obj_t *screen_wifi_list_wifi_item3;
	lv_obj_t *screen_wifi_list_wifi_item4;
	lv_obj_t *screen_wifi_btn_scanf;
	lv_obj_t *screen_wifi_btn_scanf_label;
	lv_obj_t *screen_wifi_digital_clock_time;
	lv_obj_t *screen_wifi_img_wi_close;
	lv_obj_t *screen_wifi_btn_return;
	lv_obj_t *screen_wifi_btn_return_label;
	lv_obj_t *screen_wifi_list_battery;
	lv_obj_t *screen_wifi_list_battery_item0;
	lv_obj_t *screen_wifi_connect;
	bool screen_wifi_connect_del;
	lv_obj_t *screen_wifi_connect_ta_input;
	lv_obj_t *screen_wifi_connect_label_connect;
	lv_obj_t *screen_wifi_connect_btn_set;
	lv_obj_t *screen_wifi_connect_btn_set_label;
	lv_obj_t *screen_wifi_connect_label_wifi;
	lv_obj_t *screen_wifi_connect_label_wifiname;
	lv_obj_t *screen_weather;
	bool screen_weather_del;
	lv_obj_t *screen_weather_label_later;
	lv_obj_t *screen_weather_label_tomorrow;
	lv_obj_t *screen_weather_label_today;
	lv_obj_t *screen_weather_label_later_temp;
	lv_obj_t *screen_weather_label_tomorrow_temp;
	lv_obj_t *screen_weather_label_today_temp;
	lv_obj_t *screen_weather_img_tomorrow;
	lv_obj_t *screen_weather_img_today;
	lv_obj_t *screen_weather_img_later;
	lv_obj_t *screen_weather_label_today1;
	lv_obj_t *screen_weather_label_tomorrow2;
	lv_obj_t *screen_weather_label_later2;
	lv_obj_t *screen_weather_btn_return;
	lv_obj_t *screen_weather_btn_return_label;
	lv_obj_t *screen_weather_label_city;
	lv_obj_t *screen_weather_img_1;
	lv_obj_t *screen_weather_list_bettery;
	lv_obj_t *screen_weather_list_bettery_item0;
	lv_obj_t *screen_weather_img_wificlose;
	lv_obj_t *screen_weather_datetext_1;
	lv_obj_t *screen_down;
	bool screen_down_del;
	lv_obj_t *screen_down_list_voice;
	lv_obj_t *screen_down_list_voice_item0;
	lv_obj_t *screen_down_list_brightness;
	lv_obj_t *screen_down_list_brightness_item0;
	lv_obj_t *screen_down_img_wifi;
	lv_obj_t *screen_down_slider_brightness;
	lv_obj_t *screen_down_slider_1;
	lv_obj_t *screen_down_img_musci;
	lv_obj_t *screen_set_clock;
	bool screen_set_clock_del;
	lv_obj_t *screen_set_clock_roller_hour;
	lv_obj_t *screen_set_clock_roller_minute;
	lv_obj_t *screen_set_clock_btn_set;
	lv_obj_t *screen_set_clock_btn_set_label;
	lv_obj_t *screen_set_clock_btn_return;
	lv_obj_t *screen_set_clock_btn_return_label;
	lv_obj_t *screen_clock;
	bool screen_clock_del;
	lv_obj_t *screen_clock_list_clock;
	lv_obj_t *screen_clock_list_clock_item0;
	lv_obj_t *screen_clock_list_clock_item1;
	lv_obj_t *screen_clock_list_clock_item2;
	lv_obj_t *screen_clock_list_clock_item3;
	lv_obj_t *screen_clock_sw_4;
	lv_obj_t *screen_clock_label_1;
	lv_obj_t *screen_clock_btn_return;
	lv_obj_t *screen_clock_btn_return_label;
	lv_obj_t *screen_clock_sw_3;
	lv_obj_t *screen_clock_sw_2;
	lv_obj_t *screen_clock_sw_1;
	lv_obj_t *screen_1;
	bool screen_1_del;
	lv_obj_t *screen_1_roller_1;
	lv_obj_t *screen_1_btn_set;
	lv_obj_t *screen_1_btn_set_label;
	lv_obj_t *screen_music;
	bool screen_music_del;
	lv_obj_t *screen_musiclist;
	bool screen_musiclist_del;
	lv_obj_t *screen_musiclist_list_music;
	lv_obj_t *screen_musiclist_list_music_item0;
	lv_obj_t *screen_musiclist_list_music_item1;
	lv_obj_t *screen_musiclist_list_music_item2;
	lv_obj_t *screen_musiclist_list_music_item3;
	lv_obj_t *screen_musiclist_list_music_item4;
	lv_obj_t *screen_musiclist_list_music_item5;
	lv_obj_t *screen_musiclist_list_music_item6;
	lv_obj_t *screen_musiclist_list_music_item7;
	lv_obj_t *screen_musiclist_list_music_item8;
	lv_obj_t *screen_musiclist_list_music_item9;
	lv_obj_t *screen_musiclist_list_music_item10;
	lv_obj_t *screen_musiclist_img_wifi;
	lv_obj_t *screen_musiclist_digital_clock_1;
	lv_obj_t *screen_musiclist_list_battery;
	lv_obj_t *screen_musiclist_list_battery_item0;
	lv_obj_t *screen_musiclist_btn_return;
	lv_obj_t *screen_musiclist_btn_return_label;
	lv_obj_t *screen_play;
	bool screen_play_del;
	lv_obj_t *screen_play_img_background;
	lv_obj_t *screen_play_slider_musictime;
	lv_obj_t *screen_play_imgbtn_up;
	lv_obj_t *screen_play_imgbtn_up_label;
	lv_obj_t *screen_play_imgbtn_down;
	lv_obj_t *screen_play_imgbtn_down_label;
	lv_obj_t *screen_play_list_play;
	lv_obj_t *screen_play_list_play_item0;
	lv_obj_t *screen_play_list_aduio;
	lv_obj_t *screen_play_list_aduio_item0;
	lv_obj_t *screen_play_list_3;
	lv_obj_t *screen_play_list_3_item0;
	lv_obj_t *screen_play_img_wifi;
	lv_obj_t *screen_play_digital_clock_1;
	lv_obj_t *screen_play_list_battery;
	lv_obj_t *screen_play_list_battery_item0;
	lv_obj_t *screen_play_slider_aduio;
	lv_obj_t *g_kb_top_layer;
}lv_ui;

typedef void (*ui_setup_scr_t)(lv_ui * ui);

void ui_init_style(lv_style_t * style);

void ui_load_scr_animation(lv_ui *ui, lv_obj_t ** new_scr, bool new_scr_del, bool * old_scr_del, ui_setup_scr_t setup_scr,
                           lv_screen_load_anim_t anim_type, uint32_t time, uint32_t delay, bool is_clean, bool auto_del);

void ui_animation(void * var, uint32_t duration, int32_t delay, int32_t start_value, int32_t end_value, lv_anim_path_cb_t path_cb,
                  uint32_t repeat_cnt, uint32_t repeat_delay, uint32_t playback_time, uint32_t playback_delay,
                  lv_anim_exec_xcb_t exec_cb, lv_anim_start_cb_t start_cb, lv_anim_completed_cb_t ready_cb, lv_anim_deleted_cb_t deleted_cb);


void init_scr_del_flag(lv_ui *ui);

void setup_bottom_layer(void);

void setup_ui(lv_ui *ui);

void video_play(lv_ui *ui);

void init_keyboard(lv_ui *ui);

extern lv_ui guider_ui;


void setup_scr_screen_home(lv_ui *ui);
void setup_scr_screen_selete(lv_ui *ui);
void setup_scr_screen_Rli(lv_ui *ui);
void setup_scr_screen_AI(lv_ui *ui);
void setup_scr_screen_wifi(lv_ui *ui);
void setup_scr_screen_wifi_connect(lv_ui *ui);
void setup_scr_screen_weather(lv_ui *ui);
void setup_scr_screen_down(lv_ui *ui);
void setup_scr_screen_set_clock(lv_ui *ui);
void setup_scr_screen_clock(lv_ui *ui);
void setup_scr_screen_1(lv_ui *ui);
void setup_scr_screen_music(lv_ui *ui);
void setup_scr_screen_musiclist(lv_ui *ui);
void setup_scr_screen_play(lv_ui *ui);
LV_IMAGE_DECLARE(_wifi_close_RGB565A8_20x24);
LV_IMAGE_DECLARE(_foot_RGB565A8_51x34);
LV_IMAGE_DECLARE(_Air_pressure_RGB565A8_45x39);
LV_IMAGE_DECLARE(_WiFi2_RGB565A8_77x59);
LV_IMAGE_DECLARE(_wifi_press_RGB565A8_77x59);
LV_IMAGE_DECLARE(_weather_RGB565A8_77x59);
LV_IMAGE_DECLARE(_wether_press_RGB565A8_77x59);
LV_IMAGE_DECLARE(_rli_RGB565A8_77x55);
LV_IMAGE_DECLARE(_rli_press_RGB565A8_77x55);
LV_IMAGE_DECLARE(_AI_RGB565A8_77x55);
LV_IMAGE_DECLARE(_AI_Press_RGB565A8_77x55);
LV_IMAGE_DECLARE(_clock_RGB565A8_77x64);
LV_IMAGE_DECLARE(_clopress_RGB565A8_77x64);
LV_IMAGE_DECLARE(_music_RGB565A8_77x66);
LV_IMAGE_DECLARE(_musicpress_RGB565A8_77x66);
LV_IMAGE_DECLARE(_ai_label_RGB565A8_54x46);
LV_IMAGE_DECLARE(_User_RGB565A8_54x46);
LV_IMAGE_DECLARE(_return_RGB565A8_27x30);
LV_IMAGE_DECLARE(_return_press_RGB565A8_27x30);
LV_IMAGE_DECLARE(_wifi_close_RGB565A8_20x20);
LV_IMAGE_DECLARE(_wifi_close_RGB565A8_23x26);
LV_IMAGE_DECLARE(_yu_RGB565A8_60x59);
LV_IMAGE_DECLARE(_qing_RGB565A8_58x57);
LV_IMAGE_DECLARE(_yun_RGB565A8_57x50);
LV_IMAGE_DECLARE(_line_RGB565A8_218x37);
LV_IMAGE_DECLARE(_WiFi2_RGB565A8_100x100);
LV_IMAGE_DECLARE(_musci2_RGB565A8_100x100);
LV_IMAGE_DECLARE(_music_RGB565A8_100x100);
LV_IMAGE_DECLARE(_up_RGB565A8_41x43);
LV_IMAGE_DECLARE(_up_press_RGB565A8_41x43);
LV_IMAGE_DECLARE(_last_RGB565A8_41x43);
LV_IMAGE_DECLARE(_down_press_RGB565A8_41x43);

LV_FONT_DECLARE(lv_font_ZiTiQuanWeiJunHeiW22_18)
LV_FONT_DECLARE(lv_font_ZiTiQuanWeiJunHeiW22_16)
LV_FONT_DECLARE(lv_font_ZiTiQuanWeiJunHeiW22_48)
LV_FONT_DECLARE(lv_font_ZiTiQuanWeiJunHeiW22_12)
LV_FONT_DECLARE(lv_font_ZiTiQuanWeiJunHeiW22_24)
LV_FONT_DECLARE(lv_font_ZiTiQuanWeiJunHeiW22_22)
LV_FONT_DECLARE(lv_font_ZiTiQuanWeiJunHeiW22_45)
LV_FONT_DECLARE(lv_font_arial_12)
LV_FONT_DECLARE(lv_font_ZiTiQuanWeiJunHeiW22_64)
LV_FONT_DECLARE(lv_font_ZiTiQuanWeiJunHeiW22_32)
LV_FONT_DECLARE(lv_font_ZiTiQuanWeiJunHeiW22_30)
LV_FONT_DECLARE(lv_font_SourceHanSerifSC_Regular_18)


#ifdef __cplusplus
}
#endif
#endif
