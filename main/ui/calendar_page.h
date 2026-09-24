#ifndef CALENDAR_PAGE_H
#define CALENDAR_PAGE_H

#include "gui_guider.h"

/** 用系统当前日期创建日历页面；由页面删除事件释放内部状态。 */
void calendar_page_init(lv_ui *ui);

#endif
