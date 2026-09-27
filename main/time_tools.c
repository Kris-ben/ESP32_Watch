#include "time_tools.h"

#include "esp_timer.h"

static int64_t s_countdown_left_us = 5LL * 60 * 1000000;
static int64_t s_countdown_deadline_us;
static bool s_countdown_running;
static int64_t s_stopwatch_accumulated_us;
static int64_t s_stopwatch_started_us;
static bool s_stopwatch_running;
static uint32_t s_stopwatch_lap_origin_ms;
static uint32_t s_stopwatch_lap_count;

void time_tools_countdown_set(uint32_t seconds)
{
    s_countdown_left_us = (int64_t)seconds * 1000000;
    s_countdown_running = false;
}

void time_tools_countdown_toggle(void)
{
    int64_t now = esp_timer_get_time();
    if (s_countdown_running) {
        s_countdown_left_us = s_countdown_deadline_us > now ?
                              s_countdown_deadline_us - now : 0;
        s_countdown_running = false;
    } else if (s_countdown_left_us > 0) {
        s_countdown_deadline_us = now + s_countdown_left_us;
        s_countdown_running = true;
    }
}

void time_tools_countdown_reset(void)
{
    time_tools_countdown_set(5 * 60);
}

uint32_t time_tools_countdown_remaining(void)
{
    int64_t left = s_countdown_running ?
                   s_countdown_deadline_us - esp_timer_get_time() :
                   s_countdown_left_us;
    if (left <= 0) return 0;
    return (uint32_t)((left + 999999) / 1000000);
}

bool time_tools_countdown_running(void)
{
    return s_countdown_running;
}

bool time_tools_countdown_poll(void)
{
    if (!s_countdown_running || esp_timer_get_time() < s_countdown_deadline_us)
        return false;
    s_countdown_running = false;
    s_countdown_left_us = 0;
    return true;
}

void time_tools_stopwatch_toggle(void)
{
    int64_t now = esp_timer_get_time();
    if (s_stopwatch_running) {
        s_stopwatch_accumulated_us += now - s_stopwatch_started_us;
        s_stopwatch_running = false;
    } else {
        s_stopwatch_started_us = now;
        s_stopwatch_running = true;
    }
}

void time_tools_stopwatch_reset(void)
{
    s_stopwatch_running = false;
    s_stopwatch_accumulated_us = 0;
    s_stopwatch_lap_origin_ms = 0;
    s_stopwatch_lap_count = 0;
}

uint32_t time_tools_stopwatch_elapsed_ms(void)
{
    int64_t elapsed = s_stopwatch_accumulated_us;
    if (s_stopwatch_running) elapsed += esp_timer_get_time() - s_stopwatch_started_us;
    return (uint32_t)(elapsed / 1000);
}

bool time_tools_stopwatch_running(void)
{
    return s_stopwatch_running;
}

bool time_tools_stopwatch_lap(uint32_t *lap_ms, uint32_t *lap_number)
{
    if (!s_stopwatch_running || !lap_ms || !lap_number) return false;
    uint32_t elapsed_ms = time_tools_stopwatch_elapsed_ms();
    *lap_ms = elapsed_ms - s_stopwatch_lap_origin_ms;
    *lap_number = ++s_stopwatch_lap_count;
    s_stopwatch_lap_origin_ms = elapsed_ms;
    return true;
}
