#pragma once

#include "gui_guider.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief 初始化AI聊天模块（ASR、TTS等）
 * 注意：此函数不注册按钮事件，按钮事件需要在screen_AI加载后调用ai_chat_register_screen_events
 */
void ai_chat_init(lv_ui *ui);

/**
 * @brief 注册screen_AI的按钮事件
 * 必须在screen_AI创建后调用
 */
void ai_chat_register_screen_events(lv_ui *ui);

#ifdef __cplusplus
}
#endif
