#pragma once

#include <stdbool.h>

#include "esp_err.h"

#include "cst816t.h"
#include "display_st7789v.h"
#include "st7789v.h"

#ifdef __cplusplus
extern "C" {
#endif

// 初始化 LCD（包含背光初始化；默认背光 100%）
esp_err_t board_display_init(void);

// 获取底层句柄
st7789v_t *board_display_get_lcd(void);
display_st7789v_t *board_display_get_display(void);

// 初始化触摸（如果板子没接触摸或不需要，可不调用）
esp_err_t board_touch_init(void);

bool board_touch_is_initialized(void);

// 读取一次触摸数据（未初始化会返回 ESP_ERR_INVALID_STATE）
esp_err_t board_touch_read(cst816t_touch_data_t *data);

#ifdef __cplusplus
}
#endif
