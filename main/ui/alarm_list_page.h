#ifndef ALARM_LIST_PAGE_H
#define ALARM_LIST_PAGE_H

#include "gui_guider.h"

void alarm_list_page_init(lv_ui *ui);
// 调用者持有 LVGL 锁；刷新当前闹钟页实际可见的四行。
void alarm_list_page_refresh(lv_ui *ui);

#endif
