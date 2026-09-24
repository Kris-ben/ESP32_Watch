#pragma once

// 可从语音任务调用；模块内部取得 LVGL 锁。唤醒后显示，连续问答结束时隐藏。
typedef enum {
    VOICE_WAKE_OVERLAY_LISTENING,
    VOICE_WAKE_OVERLAY_THINKING,
    VOICE_WAKE_OVERLAY_REPLYING,
} voice_wake_overlay_mode_t;

void voice_wake_overlay_show(void);
void voice_wake_overlay_set_mode(voice_wake_overlay_mode_t mode);
void voice_wake_overlay_hide(void);
