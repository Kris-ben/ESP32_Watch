#ifndef WIFI_SCAN_PAGE_H
#define WIFI_SCAN_PAGE_H

#include "gui_guider.h"

void wifi_scan_page_init(lv_ui *ui);
void wifi_scan_page_on_started(lv_ui *ui);
void wifi_scan_page_on_finished(lv_ui *ui, int count);

#endif
