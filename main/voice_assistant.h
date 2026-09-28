/**
 * @file voice_assistant.h
 * @brief 语音助手接线层
 *
 * 作用：把 voice_dialog 唤醒词模块接进主流程，
 *       并将对话状态同步到 AI 页面。
 */

#pragma once

#include <stdbool.h>
#include <stddef.h>

#include "esp_err.h"
#include "lvgl.h"
#include "gui_guider.h"
#include "voice_dialog.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief 启动语音助手
 *
 * 内部会：初始化 Spark 客户端、百度 TTS、唤醒词监听任务，
 * AI 页面创建后，对话回调会更新页面中的状态和内容。
 *
 * @param ui setup_ui() 生成的界面句柄（可为 NULL，此时只监听唤醒词）
 * @return ESP_OK 成功；其它为 esp_err_t
 */
esp_err_t voice_assistant_start(lv_ui *ui);

/**
 * @brief 手动唤醒一次（等同于喊一次唤醒词）
 */
void voice_assistant_trigger(void);

/**
 * @brief 语音助手是否已就绪
 */
bool voice_assistant_is_ready(void);

/** @brief 暂停或恢复语音监听；暂停时等待唤醒模型释放。 */
esp_err_t voice_assistant_set_music_playing(bool playing);

/** @brief 取消当前录音/对话，返回等待下一次唤醒。 */
esp_err_t voice_assistant_cancel_current_input(void);

/** @brief 当前语音流程状态，供屏幕状态页显示。 */
voice_dialog_state_t voice_assistant_get_state(void);

/** @brief 复制最近一轮识别问题和回复，缓冲区始终以 NUL 结尾。 */
void voice_assistant_get_recent_dialog(char *question, size_t question_size,
                                       char *answer, size_t answer_size);

#ifdef __cplusplus
}
#endif
