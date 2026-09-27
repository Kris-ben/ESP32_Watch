#ifndef TIME_TOOLS_H
#define TIME_TOOLS_H

#include <stdbool.h>
#include <stdint.h>

/* 均由 LVGL 任务调用；时间基准是单调时钟，系统校时不影响计时。 */
void time_tools_countdown_set(uint32_t seconds);
void time_tools_countdown_toggle(void);
void time_tools_countdown_reset(void);
uint32_t time_tools_countdown_remaining(void);
bool time_tools_countdown_running(void);
bool time_tools_countdown_poll(void);

void time_tools_stopwatch_toggle(void);
void time_tools_stopwatch_reset(void);
uint32_t time_tools_stopwatch_elapsed_ms(void);
bool time_tools_stopwatch_running(void);
bool time_tools_stopwatch_lap(uint32_t *lap_ms, uint32_t *lap_number);

#endif
