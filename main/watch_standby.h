#ifndef WATCH_STANDBY_H
#define WATCH_STANDBY_H

#include <stdbool.h>

/* 均供任务调用，不可在 ISR 调用。init 在启动 LVGL 任务前调用一次；
 * poll/touch 仅由 LVGL 任务调用，voice 接口可跨任务调用。 */
void watch_standby_init(void);
void watch_standby_poll(void);
/* 返回 true 时吞掉本次触摸，防止亮屏时误按控件。 */
bool watch_standby_touch(bool pressed);
void watch_standby_set_voice_active(bool active);
/* 闹钟开始时亮屏，响铃期间保持亮屏；结束后重新计时。 */
void watch_standby_set_alarm_active(bool active);
/* 倒计时提醒与闹钟分别记录，避免关闭一个提醒时误关另一个。 */
void watch_standby_set_timer_alert_active(bool active);
/* 可由传感器任务读取，用来跳过息屏时非必要的采样。 */
bool watch_standby_is_screen_blank(void);
/* 由现有传感器任务馈入，单位 g，不另行访问 I2C。 */
void watch_standby_feed_motion(float x, float y, float z);

#endif
