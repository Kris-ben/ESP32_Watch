#ifndef CLOCK_SETTING_H
#define CLOCK_SETTING_H

#include <stdbool.h>

#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

/* 设置本地时间；RTC 不可用时系统时间仍生效，rtc_saved 返回 false。 */
esp_err_t clock_setting_set_local(int year, int month, int day,
                                  int hour, int minute, bool *rtc_saved);

#ifdef __cplusplus
}
#endif

#endif
