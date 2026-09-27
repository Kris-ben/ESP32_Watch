#ifndef TIME_TOOLS_PAGE_H
#define TIME_TOOLS_PAGE_H

/* 仅在 LVGL 任务且已持有 LVGL 锁时调用。 */
void time_tools_page_open(void);
void time_tools_page_update(void);
void time_tools_page_show_finished(void);

#endif
