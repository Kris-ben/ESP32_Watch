#pragma once

// 跨页面问答浮窗；可从语音任务调用，模块内部取得 LVGL 锁。
void voice_chat_popup_wake(void);
void voice_chat_popup_listening(void);
void voice_chat_popup_thinking(void);
void voice_chat_popup_question(const char *text);
void voice_chat_popup_answer(const char *text);
void voice_chat_popup_replying(void);
void voice_chat_popup_session_end(void);
// 在 LVGL 页面加载回调中调用；调用者已处于 LVGL 线程，不再取锁。
void voice_chat_popup_ai_screen_entered(void);
// 在持有 LVGL 锁的语音切页代码中调用，给目标页面让出操作空间。
void voice_chat_popup_collapse_for_navigation(void);
