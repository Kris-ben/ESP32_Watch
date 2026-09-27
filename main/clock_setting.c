#include "clock_setting.h"

#include <time.h>
#include <sys/time.h>

#include "sensors.h"

static int days_in_month(int year, int month)
{
    static const int days[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    int count = days[month - 1];
    if (month == 2 && year % 4 == 0 && (year % 100 != 0 || year % 400 == 0)) ++count;
    return count;
}

esp_err_t clock_setting_set_local(int year, int month, int day,
                                  int hour, int minute, bool *rtc_saved)
{
    if (rtc_saved) *rtc_saved = false;
    if (year < 2024 || year > 2099 || month < 1 || month > 12 ||
        day < 1 || hour < 0 || hour > 23 || minute < 0 || minute > 59)
        return ESP_ERR_INVALID_ARG;
    if (day > days_in_month(year, month)) return ESP_ERR_INVALID_ARG;

    struct tm local = {
        .tm_year = year - 1900,
        .tm_mon = month - 1,
        .tm_mday = day,
        .tm_hour = hour,
        .tm_min = minute,
        .tm_sec = 0,
        .tm_isdst = -1,
    };
    time_t epoch = mktime(&local);
    if (epoch == (time_t)-1) return ESP_FAIL;
    struct timeval tv = { .tv_sec = epoch, .tv_usec = 0 };
    if (settimeofday(&tv, NULL) != 0) return ESP_FAIL;

    if (rtc_is_initialized() && rtc_sync_from_system() == ESP_OK && rtc_saved)
        *rtc_saved = true;
    return ESP_OK;
}
