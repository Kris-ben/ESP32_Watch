/**
 * @file ai_chat_new.cpp
 * @brief AI聊天模块 - 使用voice_input实现按住说话功能
 */

#include "ai_chat.h"
#include "ai_chat_config.h"
#include "voice_input.h"
#include "spark_chat.h"
#include "baidu_tts.h"
#include "max98357a.h"
#include "ai_command.h"

#include <string.h>
#include <stdlib.h>

#include "esp_log.h"
#include "esp_heap_caps.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"

static const char *TAG = "AIChat";

extern SemaphoreHandle_t lvgl_mutex;

// 声明包含完整中文字符的字体
LV_FONT_DECLARE(lv_customer_font_ZiTiQuanWeiJunHeiW22_18);

namespace {

// 全局上下文
typedef struct {
    lv_ui *ui;
    
    // 语音输入
    voice_input_handle_t voice_input;
    
    // Spark AI
    spark_chat_client_t spark;
    bool spark_inited;
    
    // 百度TTS
    baidu_tts_handle_t *tts;
    
    // 音频输出
    max98357a_handle_t **audio_out;
    
    // 状态
    bool recording;
    bool busy;
    
} ai_chat_ctx_t;

static ai_chat_ctx_t g_ctx = {};

// ==================== UI辅助函数 ====================

static void ui_label_set_text(lv_obj_t *label, const char *text)
{
    if (label == nullptr || !lv_obj_is_valid(label)) {
        return;
    }

    if (lvgl_mutex && xSemaphoreTake(lvgl_mutex, pdMS_TO_TICKS(200)) == pdTRUE) {
        lv_label_set_text(label, text ? text : "");
        xSemaphoreGive(lvgl_mutex);
    }
}

static void ui_btn_label_set_text(lv_obj_t *btn_label, const char *text)
{
    if (btn_label == nullptr || !lv_obj_is_valid(btn_label)) {
        return;
    }

    if (lvgl_mutex && xSemaphoreTake(lvgl_mutex, pdMS_TO_TICKS(200)) == pdTRUE) {
        lv_label_set_text(btn_label, text ? text : "");
        xSemaphoreGive(lvgl_mutex);
    }
}

// ==================== 音频输出管理 ====================

static esp_err_t start_audio_output()
{
    if (g_ctx.audio_out == NULL) {
        return ESP_ERR_INVALID_STATE;
    }
    
    // 如果已经初始化，直接返回
    if (*g_ctx.audio_out != NULL) {
        return ESP_OK;
    }
    
    ESP_LOGI(TAG, "初始化音频输出");
    
    max98357a_config_t audio_config = max98357a_get_default_config();
    audio_config.i2s_port = I2S_NUM_1;
    audio_config.bclk_pin = (gpio_num_t)AUDIO_BCLK_PIN;
    audio_config.lrclk_pin = (gpio_num_t)AUDIO_LRCLK_PIN;
    audio_config.din_pin = (gpio_num_t)AUDIO_DIN_PIN;
    audio_config.sd_mode_pin = (gpio_num_t)AUDIO_SD_MODE_PIN;
    audio_config.sample_rate = 16000;
    audio_config.bits_per_sample = I2S_DATA_BIT_WIDTH_16BIT;
    audio_config.gain = MAX98357A_GAIN_9DB;
    audio_config.channel = MAX98357A_CHANNEL_LEFT;
    
    esp_err_t ret = max98357a_init(&audio_config, g_ctx.audio_out);
    if (ret == ESP_OK) {
        ESP_LOGI(TAG, "音频输出初始化成功");
        
        // 更新TTS句柄的音频输出引用
        if (g_ctx.tts != NULL && *g_ctx.audio_out != NULL) {
            g_ctx.tts->audio_handle = *g_ctx.audio_out;
        }
    } else {
        ESP_LOGE(TAG, "音频输出初始化失败: %s", esp_err_to_name(ret));
    }
    
    return ret;
}

// ==================== Spark AI初始化 ====================

static void ensure_spark_inited()
{
    if (g_ctx.spark_inited) {
        return;
    }
    
    ESP_LOGI(TAG, "初始化Spark AI");
    
    spark_chat_config_t spark_cfg = {
        .api_key = SPARK_API_KEY,
        .url = SPARK_URL,
        .user_id = SPARK_USER_ID,
        .model = SPARK_MODEL,
        .timeout_ms = 30000,
        .stream = true,   // 启用流式响应
        .enable_web_search = false,
        .search_mode = NULL,
    };
    
    spark_chat_init(&g_ctx.spark, &spark_cfg);
    // 添加系统提示 - 强制限制回复长度
    spark_chat_add_message(&g_ctx.spark, "system", 
        "你是语音助手。严格限制：回答不超过30个字，用一句话回答。");
    g_ctx.spark_inited = true;
    ESP_LOGI(TAG, "Spark AI初始化成功");
}

// ==================== 语音识别和AI对话 ====================

static void process_voice_input()
{
    if (g_ctx.busy) {
        ESP_LOGW(TAG, "系统忙碌中");
        return;
    }
    
    ESP_LOGI(TAG, "开始处理语音输入");
    g_ctx.busy = true;
    g_ctx.recording = true;
    
    // 更新UI
    ui_btn_label_set_text(g_ctx.ui->screen_AI_btn_spreak_label, "松开结束");
    ui_label_set_text(g_ctx.ui->screen_AI_label_user, "");
    ui_label_set_text(g_ctx.ui->screen_AI_label_ai, "");
    
    // 录音并识别
    char asr_result[512] = {0};
    esp_err_t ret = voice_input_record_and_recognize(g_ctx.voice_input, 
                                                      asr_result, 
                                                      sizeof(asr_result));
    
    g_ctx.recording = false;
    ui_btn_label_set_text(g_ctx.ui->screen_AI_btn_spreak_label, "按下说话");
    
    // 检查识别结果
    if (ret != ESP_OK || strlen(asr_result) == 0) {
        ESP_LOGW(TAG, "语音识别失败或结果为空");
        ui_label_set_text(g_ctx.ui->screen_AI_label_user, "（未识别到内容）");
        g_ctx.busy = false;
        return;
    }
    
    ESP_LOGI(TAG, "识别结果: %s", asr_result);
    ui_label_set_text(g_ctx.ui->screen_AI_label_user, asr_result);
    
    // 确保Spark已初始化
    ensure_spark_inited();
    
    if (!g_ctx.spark_inited) {
        ui_label_set_text(g_ctx.ui->screen_AI_label_ai, "（AI未初始化）");
        g_ctx.busy = false;
        return;
    }
    
    // 调用Spark AI
    ui_label_set_text(g_ctx.ui->screen_AI_label_ai, "思考中...");
    
    // 清除历史，避免内容膨胀
    spark_chat_clear_history(&g_ctx.spark);
    
    // 构建包含设备控制指令的系统提示
    static char system_prompt[768];
    ai_command_get_system_prompt(system_prompt, sizeof(system_prompt));
    spark_chat_add_message(&g_ctx.spark, "system", system_prompt);
    spark_chat_add_message(&g_ctx.spark, "user", asr_result);
    
    if (spark_chat_request(&g_ctx.spark)) {
        const char *ai_response = spark_chat_get_last_response(&g_ctx.spark);
        ESP_LOGI(TAG, "AI回复: %s", ai_response);
        
        // 限制AI回复长度（避免TTS超时）
        static char truncated_response[200];
        size_t resp_len = ai_response ? strlen(ai_response) : 0;
        if (resp_len > 150) {
            // 找到最后一个完整的句子（中文句号或逗号）
            size_t cut_pos = 150;
            for (size_t i = 150; i > 50; i--) {
                // 检查中文句号 。(0xE3 0x80 0x82) 或逗号 ，(0xEF 0xBC 0x8C)
                if (i >= 3) {
                    unsigned char c1 = (unsigned char)ai_response[i-3];
                    unsigned char c2 = (unsigned char)ai_response[i-2];
                    unsigned char c3 = (unsigned char)ai_response[i-1];
                    if ((c1 == 0xE3 && c2 == 0x80 && c3 == 0x82) ||
                        (c1 == 0xEF && c2 == 0xBC && c3 == 0x8C)) {
                        cut_pos = i;
                        break;
                    }
                }
            }
            strncpy(truncated_response, ai_response, cut_pos);
            truncated_response[cut_pos] = '\0';
            ai_response = truncated_response;
            ESP_LOGW(TAG, "AI回复已截断至 %d 字节", cut_pos);
        }
        
        // 解析并执行AI回复中的控制指令
        static char clean_response[200];
        int cmd_count = ai_command_parse_and_execute(ai_response, clean_response, sizeof(clean_response));
        if (cmd_count > 0) {
            ESP_LOGI(TAG, "执行了 %d 条控制指令", cmd_count);
            ai_response = clean_response;  // 使用去除指令后的纯文本
        }
        
        // 显示AI回复
        ui_label_set_text(g_ctx.ui->screen_AI_label_ai, ai_response);
        
        // 关闭Spark连接释放内存
        spark_chat_close_connection(&g_ctx.spark);
        vTaskDelay(pdMS_TO_TICKS(100));
        
        // 播放TTS
        if (ai_response && strlen(ai_response) > 0) {
            // 确保音频输出可用
            if (start_audio_output() == ESP_OK) {
                ESP_LOGI(TAG, "TTS: %.50s%s", ai_response, strlen(ai_response) > 50 ? "..." : "");
                baidu_tts_speak(g_ctx.tts, ai_response);
            } else {
                ESP_LOGE(TAG, "音频输出初始化失败，跳过TTS");
            }
        }
    } else {
        ESP_LOGE(TAG, "Spark AI请求失败");
        ui_label_set_text(g_ctx.ui->screen_AI_label_ai, "（AI请求失败）");
        spark_chat_close_connection(&g_ctx.spark);
    }
    
    g_ctx.busy = false;
}

// ==================== 按钮事件处理 ====================

static void btn_spreak_event_cb(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    
    if (code == LV_EVENT_PRESSED) {
        // 按下 → 开始录音
        ESP_LOGI(TAG, ">>> 按下按钮：开始录音");
        
        if (g_ctx.busy) {
            ESP_LOGW(TAG, "系统忙碌中，忽略");
            return;
        }
        
        // 在新任务中处理，避免阻塞UI（栈需要足够大以容纳HTTPS请求）
        xTaskCreate([](void *arg) {
            process_voice_input();
            vTaskDelete(NULL);
        }, "voice_proc", 16384, NULL, 5, NULL);
        
    } else if (code == LV_EVENT_RELEASED || code == LV_EVENT_PRESS_LOST) {
        // 释放 → 停止录音
        ESP_LOGI(TAG, ">>> 释放按钮：停止录音 (recording=%d, busy=%d)", g_ctx.recording, g_ctx.busy);
        
        if (g_ctx.recording && g_ctx.voice_input != NULL) {
            ESP_LOGI(TAG, "设置停止标志");
            voice_input_set_stop_flag(g_ctx.voice_input, true);
        } else {
            ESP_LOGW(TAG, "未在录音状态，忽略释放事件");
        }
    }
}

} // namespace

// ==================== 公共API ====================

extern "C" void ai_chat_init(lv_ui *ui)
{
    ESP_LOGI(TAG, "=== ai_chat_init() 开始 ===");
    
    if (ui == nullptr) {
        ESP_LOGE(TAG, "UI指针为空！");
        return;
    }
    
    g_ctx.ui = ui;
    
    // 初始化语音输入
    voice_input_config_t voice_cfg = voice_input_get_default_config();
    voice_cfg.mic_data_pin = MIC_DATA_PIN;
    voice_cfg.mic_clk_pin = MIC_CLK_PIN;
    voice_cfg.sample_rate = AI_AUDIO_SAMPLE_RATE;
    voice_cfg.max_record_ms = AI_MAX_RECORD_SECONDS * 1000;
    voice_cfg.silence_timeout_ms = 1500;
    voice_cfg.silence_threshold = 2500;  // 提高阈值
    voice_cfg.api_key = BAIDU_API_KEY;
    voice_cfg.secret_key = BAIDU_SECRET_KEY;
    
    // 获取音频输出句柄（假设在main.c中定义）
    extern max98357a_handle_t **get_audio_handle();
    g_ctx.audio_out = get_audio_handle();
    
    esp_err_t ret = voice_input_init(&voice_cfg, *g_ctx.audio_out, &g_ctx.voice_input);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "语音输入初始化失败");
        return;
    }
    
    // 初始化TTS
    g_ctx.tts = (baidu_tts_handle_t *)malloc(sizeof(baidu_tts_handle_t));
    if (g_ctx.tts == NULL) {
        ESP_LOGE(TAG, "TTS句柄分配失败");
        return;
    }
    
    baidu_tts_config_t tts_cfg = {
        .api_key = BAIDU_API_KEY,
        .secret_key = BAIDU_SECRET_KEY,
        .voice = BAIDU_TTS_VOICE_FEMALE,  // 度小美
        .speed = 5,
        .pitch = 5,
        .volume = 5,
        .timeout_ms = 30000,
    };
    
    ret = baidu_tts_init(g_ctx.tts, &tts_cfg, NULL);  // 音频输出稍后初始化
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "TTS初始化失败: %s", esp_err_to_name(ret));
    } else {
        ESP_LOGI(TAG, "TTS初始化成功");
    }
    
    // 初始化AI指令控制模块
    ai_command_init(ui);
    
    // 按钮事件和字体设置将在screen_AI加载后通过ai_chat_register_screen_events注册
    ESP_LOGI(TAG, "=== ai_chat_init() 完成（按钮事件将在进入screen_AI时注册）===");
}

// 标记是否已注册按钮事件
static bool g_btn_event_registered = false;

extern "C" void ai_chat_register_screen_events(lv_ui *ui)
{
    if (g_btn_event_registered) {
        ESP_LOGD(TAG, "按钮事件已注册，跳过");
        return;
    }
    
    if (!ui) {
        ESP_LOGE(TAG, "ui为空");
        return;
    }
    
    // 注册按钮事件
    if (ui->screen_AI_btn_spreak && lv_obj_is_valid(ui->screen_AI_btn_spreak)) {
        lv_obj_add_event_cb(ui->screen_AI_btn_spreak, btn_spreak_event_cb, LV_EVENT_ALL, ui);
        ESP_LOGI(TAG, "按钮事件回调已注册（按住说话模式）");
        g_btn_event_registered = true;
    } else {
        ESP_LOGE(TAG, "按钮对象无效或为空！");
    }
    
    // 设置按钮标签字体和文本
    if (ui->screen_AI_btn_spreak_label && lv_obj_is_valid(ui->screen_AI_btn_spreak_label)) {
        lv_obj_set_style_text_font(ui->screen_AI_btn_spreak_label, &lv_customer_font_ZiTiQuanWeiJunHeiW22_18, LV_PART_MAIN | LV_STATE_DEFAULT);
        ui_btn_label_set_text(ui->screen_AI_btn_spreak_label, "按下说话");
    }
    
    // 设置AI对话label使用中文字体
    if (ui->screen_AI_label_ai && lv_obj_is_valid(ui->screen_AI_label_ai)) {
        lv_obj_set_style_text_font(ui->screen_AI_label_ai, &lv_customer_font_ZiTiQuanWeiJunHeiW22_18, LV_PART_MAIN | LV_STATE_DEFAULT);
    }
    if (ui->screen_AI_label_user && lv_obj_is_valid(ui->screen_AI_label_user)) {
        lv_obj_set_style_text_font(ui->screen_AI_label_user, &lv_customer_font_ZiTiQuanWeiJunHeiW22_18, LV_PART_MAIN | LV_STATE_DEFAULT);
    }
    ESP_LOGI(TAG, "screen_AI界面初始化完成");
}
