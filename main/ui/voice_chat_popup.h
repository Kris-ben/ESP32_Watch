#pragma once

// 跨页面问答浮窗；可从语音任务调用，模块内部取得 LVGL 锁。
void voice_chat_popup_wake(void);
void voice_chat_popup_listening(void);
void voice_chat_popup_thinking(void);
void voice_chat_popup_question(const char *text);
void voice_chat_popup_answer(const char *text);
void voice_chat_popup_replying(void);
void voice_chat_popup_session_end(void);
