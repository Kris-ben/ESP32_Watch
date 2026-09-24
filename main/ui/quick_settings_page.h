#ifndef QUICK_SETTINGS_PAGE_H
#define QUICK_SETTINGS_PAGE_H

#include "gui_guider.h"

/** 注册触摸手势；所有页面从顶部下拉均可打开快捷设置。 */
void quick_settings_page_bind(lv_ui *ui);

/** 在当前页面上打开快捷设置，不改变原页面。 */
void quick_settings_page_open(lv_ui *ui);

#endif
