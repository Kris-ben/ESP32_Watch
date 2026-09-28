/**
 * @file voice_assistant.c
 * @brief 语音助手接线层实现
 *
 * 只启动 voice_dialog 唤醒词引擎，避免多个语音实现争用音频设备。
 */

#include "voice_assistant.h"
#include "watch_standby.h"

#include <stdlib.h>
#include <string.h>

#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"

#include "baidu_tts.h"
#include "max98357a.h"
#include "spark_chat.h"
#include "voice_dialog.h"
#include "ui/voice_chat_popup.h"
#include "ui/voice_wake_overlay.h"
#include "ai_chat/ai_chat_config.h"
#include "ai_command.h"

static const char *TAG = "VOICE_ASST";

// main.c 提供的两个全局资源（音频输出句柄、LVGL 互斥锁）
extern max98357a_handle_t **get_audio_handle(void);
extern SemaphoreHandle_t lvgl_mutex;

static voice_dialog_handle_t s_dialog = NULL;
static spark_chat_client_t   s_spark;      // 大模型客户端（静态分配，生命周期=程序）
static baidu_tts_handle_t   *s_tts = NULL; // TTS 句柄（堆分配）
static lv_ui                *s_ui = NULL;
static bool                  s_ready = false;
static portMUX_TYPE          s_text_lock = portMUX_INITIALIZER_UNLOCKED;
static char                  s_recent_question[256] = "";
static char                  s_recent_answer[384] = "";

static void copy_utf8_text(char *dst, size_t size, const char *src)
{
    if (!dst || size == 0) return;
    if (!src) src = "";
    size_t length = strlen(src);
    if (length >= size) {
        length = size - 1;
        while (length > 0 && ((unsigned char)src[length] & 0xc0) == 0x80) {
            length--;
        }
    }
    memcpy(dst, src, length);
    dst[length] = '\0';
}

static void remember_dialog_text(bool question, const char *text)
{
    char buffer[sizeof(s_recent_answer)] = {0};
    copy_utf8_text(buffer, question ? sizeof(s_recent_question) : sizeof(s_recent_answer), text);
    portENTER_CRITICAL(&s_text_lock);
    if (question) {
        memcpy(s_recent_question, buffer, sizeof(s_recent_question));
        s_recent_answer[0] = '\0';
    } else {
        memcpy(s_recent_answer, buffer, sizeof(s_recent_answer));
    }
    portEXIT_CRITICAL(&s_text_lock);
}

/**
 * @brief 拿 LVGL 锁后安全地更新一个 label
 */
static void ui_set_label(lv_obj_t **label_ref, const char *text)
{
    if (!label_ref || !s_ui || !lvgl_mutex) return;
    if (xSemaphoreTake(lvgl_mutex, pdMS_TO_TICKS(200)) != pdTRUE) return;

    lv_obj_t *screen = s_ui->screen_AI;
    lv_obj_t *label = *label_ref;
    // 页面可能在等待锁时被删除；检查必须和写入处于同一个锁范围。
    if (screen && label && lv_obj_is_valid(screen) && lv_obj_is_valid(label) &&
        lv_obj_check_type(label, &lv_label_class) && lv_obj_get_screen(label) == screen) {
        lv_label_set_text(label, text ? text : "");
        lv_obj_t *parent = lv_obj_get_parent(label);
        if (parent && lv_obj_has_flag(parent, LV_OBJ_FLAG_SCROLLABLE)) {
            lv_obj_update_layout(parent);
            lv_obj_scroll_to_view(label, LV_ANIM_OFF);
        }
    }
    xSemaphoreGive(lvgl_mutex);
}

/**
 * @brief 语音对话事件回调：把对话内容显示到 screen_AI 界面上
 */
static void on_dialog_event(voice_dialog_event_t event, const char *data, void *ctx)
{
    (void)ctx;
    if (s_ui == NULL) {
        return;
    }

    switch (event) {
    case VOICE_DIALOG_EVENT_WAKEUP:
        watch_standby_set_voice_active(true);
        ESP_LOGI(TAG, "已唤醒，等待说话");
        voice_wake_overlay_show();
        voice_chat_popup_wake();
        ui_set_label(&s_ui->screen_AI_label_ai, "我在，请说…");
        ui_set_label(&s_ui->screen_AI_label_user, "");
        break;
    case VOICE_DIALOG_EVENT_SESSION_END:
        watch_standby_set_voice_active(false);
        voice_wake_overlay_hide();
        voice_chat_popup_session_end();
        break;
    case VOICE_DIALOG_EVENT_RECORD_START:
        voice_wake_overlay_set_mode(VOICE_WAKE_OVERLAY_LISTENING);
        voice_chat_popup_listening();
        ui_set_label(&s_ui->screen_AI_label_user, "（正在听…）");
        break;
    case VOICE_DIALOG_EVENT_RECORD_END:
        voice_wake_overlay_set_mode(VOICE_WAKE_OVERLAY_THINKING);
        voice_chat_popup_thinking();
        break;
    case VOICE_DIALOG_EVENT_ASR_RESULT:
        remember_dialog_text(true, data);
        voice_chat_popup_question(data);
        ui_set_label(&s_ui->screen_AI_label_user, data ? data : "");
        ui_set_label(&s_ui->screen_AI_label_ai, "正在整理回答…");
        break;
    case VOICE_DIALOG_EVENT_AI_RESPONSE:
        voice_wake_overlay_set_mode(VOICE_WAKE_OVERLAY_REPLYING);
        remember_dialog_text(false, data);
        voice_chat_popup_answer(data);
        ui_set_label(&s_ui->screen_AI_label_ai, data ? data : "");
        break;
    case VOICE_DIALOG_EVENT_TTS_START:
        voice_wake_overlay_set_mode(VOICE_WAKE_OVERLAY_REPLYING);
        voice_chat_popup_replying();
        break;
    case VOICE_DIALOG_EVENT_TTS_END:
        voice_wake_overlay_set_mode(VOICE_WAKE_OVERLAY_LISTENING);
        voice_chat_popup_listening();
        break;
    case VOICE_DIALOG_EVENT_ERROR:
        voice_chat_popup_answer("出错了，请再试");
        ui_set_label(&s_ui->screen_AI_label_ai, "（出错了）");
        break;
    default:
        break;
    }
}

esp_err_t voice_assistant_start(lv_ui *ui)
{
    if (s_ready) {
        ESP_LOGW(TAG, "语音助手已经启动，跳过");
        return ESP_OK;
    }

    s_ui = ui;

    // 初始化设备控制指令模块（亮度/音量/闹钟/切界面由它统一执行，
    // voice_local_intent.c 里命中本地意图后会调用它）
    if (ui != NULL) {
        ai_command_init(ui);
    }

    // ---------- 1) Spark 大模型客户端 ----------
    spark_chat_config_t spark_cfg = {
        .api_key = SPARK_API_KEY,
        .url = SPARK_URL,
        .user_id = SPARK_USER_ID,
        .model = SPARK_MODEL,
        .timeout_ms = 30000,
        .stream = true,
        .enable_web_search = false,
        .search_mode = NULL,
    };
    spark_chat_init(&s_spark, &spark_cfg);
    // 语音场景限制回复长度，避免TTS念太久
    spark_chat_add_message(&s_spark, "system",
                           "你是语音助手。回答不超过30个字，用一句话回答。");

    // ---------- 2) 百度 TTS ----------
    s_tts = (baidu_tts_handle_t *)calloc(1, sizeof(baidu_tts_handle_t));
    if (s_tts == NULL) {
        ESP_LOGE(TAG, "TTS句柄分配失败");
        return ESP_ERR_NO_MEM;
    }

    baidu_tts_config_t tts_cfg = {
        .api_key = BAIDU_API_KEY,
        .secret_key = BAIDU_SECRET_KEY,
        .voice = BAIDU_TTS_VOICE_FEMALE,
        .speed = 5,
        .pitch = 5,
        .volume = 5,
        .timeout_ms = 30000,
    };
    // 音频输出先传 NULL：voice_dialog 播报前会把当前喇叭句柄补进 TTS。
    if (baidu_tts_init(s_tts, &tts_cfg, NULL) != ESP_OK) {
        ESP_LOGW(TAG, "TTS初始化失败：AI回复将没有声音");
    }

    // ---------- 3) 语音对话（唤醒词监听） ----------
    voice_dialog_config_t cfg = voice_dialog_get_default_config();
    cfg.mic_data_pin = (int)MIC_DATA_PIN;
    cfg.mic_clk_pin = (int)MIC_CLK_PIN;
    cfg.sample_rate = AI_AUDIO_SAMPLE_RATE;
    cfg.baidu_api_key = BAIDU_API_KEY;
    cfg.baidu_secret_key = BAIDU_SECRET_KEY;
    cfg.spark_client = &s_spark;
    cfg.tts_handle = s_tts;
    cfg.audio_handle = get_audio_handle();
    cfg.audio_bclk_pin = (int)AUDIO_BCLK_PIN;
    cfg.audio_lrclk_pin = (int)AUDIO_LRCLK_PIN;
    cfg.audio_din_pin = (int)AUDIO_DIN_PIN;
    cfg.audio_sd_mode_pin = (int)AUDIO_SD_MODE_PIN; // GPIO_NUM_NC 即 -1

    esp_err_t err = voice_dialog_init(&cfg, &s_dialog);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "voice_dialog_init 失败: %s", esp_err_to_name(err));
        return err;
    }

    (void)voice_dialog_set_callback(s_dialog, on_dialog_event, NULL);

    err = voice_dialog_start(s_dialog);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "voice_dialog_start 失败: %s", esp_err_to_name(err));
        return err;
    }

    s_ready = true;
    ESP_LOGI(TAG, "语音助手已启动：喊\"%s\"唤醒", cfg.wakeup_word);
    return ESP_OK;
}

void voice_assistant_trigger(void)
{
    if (s_dialog == NULL) {
        ESP_LOGW(TAG, "语音助手未启动，忽略本次唤醒");
        return;
    }

    esp_err_t err = voice_dialog_trigger(s_dialog);
    if (err != ESP_OK) {
        ESP_LOGW(TAG, "手动唤醒失败: %s", esp_err_to_name(err));
    }
}

bool voice_assistant_is_ready(void)
{
    return s_ready;
}

esp_err_t voice_assistant_set_music_playing(bool playing)
{
    return s_dialog ? voice_dialog_set_music_playing(s_dialog, playing) : ESP_OK;
}

esp_err_t voice_assistant_cancel_current_input(void)
{
    return s_dialog ? voice_dialog_cancel_current_input(s_dialog) : ESP_OK;
}

voice_dialog_state_t voice_assistant_get_state(void)
{
    return voice_dialog_get_state(s_dialog);
}

void voice_assistant_get_recent_dialog(char *question, size_t question_size,
                                       char *answer, size_t answer_size)
{
    portENTER_CRITICAL(&s_text_lock);
    copy_utf8_text(question, question_size, s_recent_question);
    copy_utf8_text(answer, answer_size, s_recent_answer);
    portEXIT_CRITICAL(&s_text_lock);
}
