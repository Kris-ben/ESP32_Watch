#include "watch_standby.h"
#include "system_settings.h"
#include "esp_timer.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"

#define IDLE_US (30LL * 1000000)
#define SHAKE_WINDOW_US 900000
#define SHAKE_GAP_US 100000

static portMUX_TYPE s_lock = portMUX_INITIALIZER_UNLOCKED;
static int64_t s_last_activity;
static bool s_voice_active;
static bool s_alarm_active;
static bool s_ready;
/* 以下状态仅由 LVGL 任务访问。 */
static bool s_asleep;
static bool s_consume_touch;
static int64_t s_touch_start;
static bool s_touch_confirmed;

void watch_standby_init(void)
{
    portENTER_CRITICAL(&s_lock);
    s_last_activity = esp_timer_get_time();
    s_ready = true;
    portEXIT_CRITICAL(&s_lock);
}

static void activity(void)
{
    portENTER_CRITICAL(&s_lock);
    s_last_activity = esp_timer_get_time();
    portEXIT_CRITICAL(&s_lock);
}

void watch_standby_set_voice_active(bool active)
{
    portENTER_CRITICAL(&s_lock);
    s_voice_active = active;
    s_last_activity = esp_timer_get_time();
    portEXIT_CRITICAL(&s_lock);
}

void watch_standby_set_alarm_active(bool active)
{
    portENTER_CRITICAL(&s_lock);
    s_alarm_active = active;
    s_last_activity = esp_timer_get_time();
    portEXIT_CRITICAL(&s_lock);
}

bool watch_standby_touch(bool pressed)
{
    if (!pressed) {
        s_consume_touch = false;
        s_touch_start = 0;
        s_touch_confirmed = false;
        return false;
    }
    if (s_asleep) {
        s_consume_touch = true;
        /* 驱动已过滤无效触摸；熄屏后的第一下只用于唤醒。 */
        activity();
    }
    int64_t now = esp_timer_get_time();
    if (!s_touch_start) s_touch_start = now;
    /* CST816T 偶发单次按下读数；只有持续约 50ms 才算人的操作。 */
    if (!s_touch_confirmed && now - s_touch_start >= 50000) {
        s_touch_confirmed = true;
    }
    if (s_touch_confirmed) activity();
    return s_consume_touch;
}

void watch_standby_poll(void)
{
    portENTER_CRITICAL(&s_lock);
    bool sleep = s_ready && !s_voice_active && !s_alarm_active &&
                 esp_timer_get_time() - s_last_activity >= IDLE_US;
    portEXIT_CRITICAL(&s_lock);
    if (sleep == s_asleep) return;
    esp_err_t err = system_set_screen_blank(sleep);
    if (err == ESP_OK) {
        s_asleep = sleep;
        ESP_LOGI("Standby", "%s", sleep ? "Screen off after 30s idle" : "Screen awake");
    } else {
        static int64_t last_error_log;
        int64_t now = esp_timer_get_time();
        if (now - last_error_log >= 5000000) {
            ESP_LOGW("Standby", "Backlight change failed: %s", esp_err_to_name(err));
            last_error_log = now;
        }
    }
}

void watch_standby_feed_motion(float x, float y, float z)
{
    /* 两个独立加速度峰才算摇动，回落后重新武装，过滤单次磕碰。 */
    static bool armed = true;
    static int64_t first_peak;
    static int64_t last_sample;
    int64_t now = esp_timer_get_time();
    float magnitude_sq = x * x + y * y + z * z;
    if (now - last_sample > SHAKE_WINDOW_US) {
        first_peak = 0;
        armed = true;
    }
    last_sample = now;
    if (magnitude_sq < 1.2f * 1.2f) armed = true;
    if (magnitude_sq < 1.45f * 1.45f || !armed) return;
    armed = false;
    if (first_peak && now - first_peak >= SHAKE_GAP_US &&
        now - first_peak <= SHAKE_WINDOW_US) {
        activity();
        first_peak = 0;
    } else if (!first_peak || now - first_peak > SHAKE_WINDOW_US) {
        first_peak = now;
    }
}
