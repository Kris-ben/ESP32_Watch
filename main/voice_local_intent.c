/**
 * @file voice_local_intent.c
 * @brief 本地语音意图处理实现
 *
 * 设计要点：
 * 1. 只用"关键词 + 数字/时间提取"的简单规则，不做自然语言理解——复杂说法交给大模型。
 * 2. 执行设备动作时复用 ai_command.c 里已经写好的执行逻辑（亮度/音量/闹钟/切界面），
 *    避免同一件事有两份实现。
 * 3. 每个分支命中后都必须写出一句"要播报的话"，让上层直接 TTS。
 */

#include "voice_local_intent.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "esp_log.h"

#include "ai_command.h"
#include "alarm_clock.h"
#include "sensors.h"
#include "system_settings.h"
#include "weather.h"

static const char *TAG = "LocalIntent";

/**
 * @brief 取一个中文字符的数字值；不是中文数字返回 -1
 *
 * 注意：这里刻意用"字符串→数值"的显式配对表，**不能用数组下标当数值**。
 * 曾经为了支持"两"（也是2）往数组里插了一项，导致后面所有数字整体错位一格，
 * 表现为"说七点，设成8点"。这个坑必须靠显式配对来避免。
 */
static int cn_digit_value(const char *p)
{
    static const struct {
        const char *s;
        int         v;
    } map[] = {
        { "零", 0 }, { "一", 1 }, { "二", 2 }, { "两", 2 }, { "三", 3 },
        { "四", 4 }, { "五", 5 }, { "六", 6 }, { "七", 7 }, { "八", 8 },
        { "九", 9 },
    };

    for (size_t i = 0; i < sizeof(map) / sizeof(map[0]); i++) {
        if (strncmp(p, map[i].s, 3) == 0) {
            return map[i].v;
        }
    }
    if (strncmp(p, "十", 3) == 0) {
        return 10;
    }
    return -1;
}

/**
 * @brief 解析中文数字（覆盖 0~99：七 / 十 / 十二 / 二十 / 三十五）
 *
 * 语音识别的结果常常是中文数字（"七点钟叫我"），只认阿拉伯数字会导致
 * "设闹钟"这类指令完全命中不了，所以必须支持。
 *
 * @param s        输入（UTF-8 字符串）
 * @param consumed 输出：消耗了几个中文字符（每个 3 字节）
 * @return 数值；-1 表示开头不是中文数字
 */
static int parse_cn_number(const char *s, int *consumed)
{
    int  total   = -1;   // "十"前面的部分
    int  cur     = 0;    // 当前个位
    bool has_cur = false;
    int  n       = 0;
    const char *p = s;

    for (; n < 4; n++, p += 3) {
        int v = cn_digit_value(p);
        if (v < 0) {
            break;
        }
        if (v == 10) {
            if (!has_cur) {
                cur = 1;    // "十二" 读作 12
            }
            total = (total < 0 ? 0 : total) + cur * 10;
            cur = 0;
            has_cur = false;
        } else {
            cur = v;
            has_cur = true;
        }
    }

    if (consumed) {
        *consumed = n;
    }
    if (n == 0) {
        return -1;
    }
    return (total < 0 ? 0 : total) + (has_cur ? cur : 0);
}

/**
 * @brief 提取文本里第一个 0~100 的整数
 *
 * 先找阿拉伯数字（"音量调到50"），再找中文数字（"音量调到五十"）。
 */
static bool extract_percent(const char *text, int *out)
{
    if (text == NULL || out == NULL) {
        return false;
    }

    // 阿拉伯数字
    for (const char *p = text; *p != '\0'; p++) {
        if (*p >= '0' && *p <= '9') {
            int v = atoi(p);
            if (v < 0) v = 0;
            if (v > 100) v = 100;
            *out = v;
            return true;
        }
    }

    // 中文数字
    for (const char *p = text; *p != '\0'; p++) {
        int consumed = 0;
        int v = parse_cn_number(p, &consumed);
        if (v >= 0 && consumed > 0) {
            if (v > 100) v = 100;
            *out = v;
            return true;
        }
    }

    return false;
}

/**
 * @brief 提取"X点Y分"形式的时间
 *
 * 支持："7点"、"7点半"、"7点30"、"早上7点"、"晚上8点"、"下午3点20"
 */
static bool extract_clock_time(const char *text, int *hour, int *minute)
{
    if (text == NULL || hour == NULL || minute == NULL) {
        return false;
    }

    const char *dot = strstr(text, "点");   // "点" 是 UTF-8 三字节
    if (dot == NULL) {
        return false;
    }

    // ---- 小时 ----
    int h = -1;

    // 先试阿拉伯数字："7点"
    const char *h_start = dot;
    while (h_start > text && h_start[-1] >= '0' && h_start[-1] <= '9') {
        h_start--;
    }
    if (h_start != dot) {
        h = atoi(h_start);
    } else {
        // 再试中文数字："七点"（往"点"前面回看 1~2 个中文字符）
        for (int back = 1; back <= 2; back++) {
            if ((size_t)(dot - text) < (size_t)(back * 3)) {
                break;
            }
            const char *pos = dot - back * 3;
            int consumed = 0;
            int v = parse_cn_number(pos, &consumed);
            if (v >= 0 && consumed == back) {
                h = v;
                break;
            }
        }
    }

    if (h < 0 || h > 23) {
        return false;
    }

    // ---- 分钟 ----
    int m = 0;
    const char *after = dot + 3;            // 跳过"点"这 3 个字节
    if (strncmp(after, "半", 3) == 0) {
        m = 30;
    } else {
        const char *m_start = NULL;
        for (const char *q = after; *q != '\0' && q < after + 12; q++) {
            if (*q >= '0' && *q <= '9') {
                m_start = q;
                break;
            }
        }
        if (m_start != NULL) {
            m = atoi(m_start);
        } else {
            int consumed = 0;
            int v = parse_cn_number(after, &consumed);   // "七点二十"
            if (v > 0 && consumed > 0) {
                m = v;
            }
        }
        if (m < 0 || m > 59) {
            m = 0;
        }
    }

    // 上午/下午/晚上 修正
    if ((strstr(text, "下午") != NULL || strstr(text, "晚上") != NULL) && h < 12) {
        h += 12;
    }
    if ((strstr(text, "早上") != NULL || strstr(text, "上午") != NULL ||
         strstr(text, "凌晨") != NULL) && h == 12) {
        h = 0;
    }

    *hour = h;
    *minute = m;
    return true;
}

/**
 * @brief 统一的成功出口：写好要播报的话并打一条日志
 */
static bool finish(const char *text, char *reply, size_t reply_len)
{
    snprintf(reply, reply_len, "%s", text ? text : "");
    ESP_LOGI(TAG, "本地意图命中: %s", reply);
    return true;
}

/**
 * @brief 执行一条 ai_command 指令（复用工程里已有的执行逻辑）
 */
static esp_err_t exec_command(ai_cmd_type_t type, int value, const char *screen_name,
                              int alarm_index, int alarm_hour, int alarm_minute)
{
    ai_command_t cmd;
    memset(&cmd, 0, sizeof(cmd));
    cmd.type = type;

    switch (type) {
    case AI_CMD_VOLUME:
        cmd.params.volume.value = (uint8_t)value;
        break;
    case AI_CMD_BRIGHTNESS:
        cmd.params.brightness.value = (uint8_t)value;
        break;
    case AI_CMD_SCREEN:
        snprintf(cmd.params.screen.name, sizeof(cmd.params.screen.name), "%s",
                 screen_name ? screen_name : "home");
        break;
    case AI_CMD_ALARM_SET:
        cmd.params.alarm.index = (uint8_t)alarm_index;
        cmd.params.alarm.hour = (uint8_t)alarm_hour;
        cmd.params.alarm.minute = (uint8_t)alarm_minute;
        cmd.params.alarm.enabled = true;
        break;
    default:
        return ESP_ERR_INVALID_ARG;
    }

    esp_err_t err = ai_command_execute(&cmd);
    if (err != ESP_OK) {
        ESP_LOGW(TAG, "指令执行失败(%d): %s", (int)type, esp_err_to_name(err));
    }
    return err;
}

bool voice_local_intent_try_handle(const char *user_text, char *reply, size_t reply_len)
{
    if (user_text == NULL || reply == NULL || reply_len == 0) {
        return false;
    }

    char buf[192];

    // ---------- 1. 切换界面（放最前，避免"打开天气"被当成查天气） ----------
    static const struct {
        const char *kw;
        const char *name;
    } scr_map[] = {
        { "音乐",   "music"    },
        { "天气",   "weather"  },
        { "日历",   "calendar" },
        { "AI助手", "ai"       },
        { "语音助手", "ai"     },
        { "小智",   "ai"       },
        { "计时",   "timer"    },
        { "倒计时", "timer"    },
        { "秒表",   "timer"    },
        { "闹钟",   "clock"    },
        { "时钟",   "clock"    },
        { "设置",   "settings" },
        { "主界面", "home"     },
        { "主页",   "home"     },
        { "wifi",   "wifi"     },
        { "WIFI",   "wifi"     },
    };

    if (strstr(user_text, "打开") != NULL || strstr(user_text, "切到") != NULL ||
        strstr(user_text, "切换") != NULL || strstr(user_text, "回到") != NULL ||
        strstr(user_text, "进入") != NULL) {
        for (size_t i = 0; i < sizeof(scr_map) / sizeof(scr_map[0]); i++) {
            if (strstr(user_text, scr_map[i].kw) != NULL) {
                if (exec_command(AI_CMD_SCREEN, 0, scr_map[i].name, 0, 0, 0) != ESP_OK) {
                    return finish("界面暂时没切过去，请再试一次。", reply, reply_len);
                }
                snprintf(buf, sizeof(buf), "好的，已经切换到%s。", scr_map[i].kw);
                return finish(buf, reply, reply_len);
            }
        }
    }

    // ---------- 2. 天气 ----------
    if (strstr(user_text, "天气") != NULL || strstr(user_text, "气温") != NULL ||
        strstr(user_text, "温度") != NULL || strstr(user_text, "多少度") != NULL) {
        amap_realtime_weather_t w;
        if (weather_get_realtime(&w)) {
            snprintf(buf, sizeof(buf), "%s现在%s，气温%d度，湿度百分之%d。",
                     w.city, w.weather, w.temperature, w.humidity);
            return finish(buf, reply, reply_len);
        }
        return finish("天气数据还没准备好，请稍后再问我。", reply, reply_len);
    }

    // ---------- 3. 设备计步器 ----------
    if (strstr(user_text, "步数") != NULL || strstr(user_text, "多少步") != NULL ||
        strstr(user_text, "几步") != NULL ||
        (strstr(user_text, "走了") != NULL && strstr(user_text, "步") != NULL)) {
        uint32_t steps = 0;
        if (!sensors_get_step_count(&steps)) {
            return finish("计步传感器还没有读到数据，请稍后再问我。", reply, reply_len);
        }
        snprintf(buf, sizeof(buf), "设备计步器目前记录了%lu步。", (unsigned long)steps);
        return finish(buf, reply, reply_len);
    }

    // ---------- 4. 报时 ----------
    if (strstr(user_text, "几点") != NULL || strstr(user_text, "报时") != NULL ||
        strstr(user_text, "现在时间") != NULL) {
        time_t now = time(NULL);
        struct tm t;
        localtime_r(&now, &t);

        const char *period = (t.tm_hour < 6)  ? "凌晨" :
                             (t.tm_hour < 12) ? "上午" :
                             (t.tm_hour < 18) ? "下午" : "晚上";
        int h12 = t.tm_hour % 12;
        if (h12 == 0) h12 = 12;

        snprintf(buf, sizeof(buf), "现在是%s%d点%d分。", period, h12, t.tm_min);
        return finish(buf, reply, reply_len);
    }

    // ---------- 5. 音量 ----------
    if (strstr(user_text, "音量") != NULL || strstr(user_text, "声音") != NULL) {
        int v = 0;
        int target = -1;

        if (extract_percent(user_text, &v)) {
            target = v;
        } else if (strstr(user_text, "静音") != NULL) {
            target = 0;
        } else if (strstr(user_text, "大") != NULL || strstr(user_text, "高") != NULL ||
                   strstr(user_text, "响") != NULL) {
            target = system_get_volume() + 20;
        } else if (strstr(user_text, "小") != NULL || strstr(user_text, "低") != NULL ||
                   strstr(user_text, "轻") != NULL) {
            target = system_get_volume() - 20;
        } else {
            return false;   // 没说清楚，交给大模型追问
        }

        if (target > 100) target = 100;
        if (target < 0) target = 0;

        exec_command(AI_CMD_VOLUME, target, NULL, 0, 0, 0);
        snprintf(buf, sizeof(buf), "音量已经调到百分之%d了。", target);
        return finish(buf, reply, reply_len);
    }

    // ---------- 6. 亮度 ----------
    if (strstr(user_text, "亮度") != NULL || strstr(user_text, "屏幕") != NULL ||
        strstr(user_text, "亮一点") != NULL || strstr(user_text, "暗一点") != NULL ||
        strstr(user_text, "太亮") != NULL || strstr(user_text, "太暗") != NULL) {
        int v = 0;
        int target = -1;

        if (extract_percent(user_text, &v)) {
            target = v;
        } else if (strstr(user_text, "最高") != NULL || strstr(user_text, "最亮") != NULL ||
                   strstr(user_text, "最大") != NULL) {
            target = 100;
        } else if (strstr(user_text, "最低") != NULL || strstr(user_text, "最暗") != NULL ||
                   strstr(user_text, "最小") != NULL) {
            // 屏幕驱动保留 5% 作为可见的最低亮度，避免完全熄屏。
            target = 5;
        } else if (strstr(user_text, "暗") != NULL) {
            target = system_get_brightness() - 20;
        } else if (strstr(user_text, "亮") != NULL) {
            target = system_get_brightness() + 20;
        } else {
            return false;
        }

        if (target > 100) target = 100;
        if (target < 0) target = 0;

        exec_command(AI_CMD_BRIGHTNESS, target, NULL, 0, 0, 0);
        snprintf(buf, sizeof(buf), "亮度已经调到百分之%d了。", target);
        return finish(buf, reply, reply_len);
    }

    // ---------- 7. 设闹钟 ----------
    if (strstr(user_text, "闹钟") != NULL || strstr(user_text, "叫我") != NULL ||
        strstr(user_text, "叫醒") != NULL || strstr(user_text, "提醒我") != NULL) {
        int h = 0;
        int m = 0;
        if (!extract_clock_time(user_text, &h, &m)) {
            return finish("没听清闹钟时间，请说几时几分。", reply, reply_len);
        }

        // 优先占用一个还没启用的闹钟位；都满了就覆盖 0 号
        int slot = -1;
        for (int i = 0; i < ALARM_COUNT; i++) {
            if (!alarm_is_enabled(i)) {
                slot = i;
                break;
            }
        }
        if (slot < 0) {
            slot = 0;
        }

        if (exec_command(AI_CMD_ALARM_SET, 0, NULL, slot, h, m) != ESP_OK) {
            return finish("闹钟设置失败，请再试一次。", reply, reply_len);
        }
        snprintf(buf, sizeof(buf), "好的，闹钟%d已经设在%d点%d分。", slot + 1, h, m);
        return finish(buf, reply, reply_len);
    }

    // ---------- 未命中：交给大模型 ----------
    return false;
}
