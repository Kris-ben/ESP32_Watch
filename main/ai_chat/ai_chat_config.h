#pragma once

#include "driver/gpio.h"

// 允许用户在不修改此文件的情况下覆盖配置（建议把密钥放在该文件中并避免提交）
#if defined(__has_include)
#if __has_include("ai_chat_config_user.h")
#include "ai_chat_config_user.h"
#endif
#endif

// 未配置时使用空值，真实凭据只写在本机 ai_chat_config_user.h。

// 星火大模型配置
#ifndef SPARK_API_KEY
#define SPARK_API_KEY           ""
#endif

#ifndef SPARK_URL
#define SPARK_URL               "https://spark-api-open.xf-yun.com/v1/chat/completions"
#endif

#ifndef SPARK_USER_ID
#define SPARK_USER_ID           "user_id"
#endif

#ifndef SPARK_MODEL
#define SPARK_MODEL             "generalv3.5"
#endif

// 百度语音配置（同一对 key 通常可用于 token/ASR/TTS）
#ifndef BAIDU_API_KEY
#define BAIDU_API_KEY           ""
#endif

#ifndef BAIDU_SECRET_KEY
#define BAIDU_SECRET_KEY        ""
#endif

// MAX98357A 音频输出引脚配置
#ifndef AUDIO_BCLK_PIN
#define AUDIO_BCLK_PIN          GPIO_NUM_41
#endif

#ifndef AUDIO_LRCLK_PIN
#define AUDIO_LRCLK_PIN         GPIO_NUM_40
#endif

#ifndef AUDIO_DIN_PIN
#define AUDIO_DIN_PIN           GPIO_NUM_42
#endif

#ifndef AUDIO_SD_MODE_PIN
#define AUDIO_SD_MODE_PIN       GPIO_NUM_NC
#endif

// MSM261D4030H1CPM 麦克风引脚配置（PDM）
#ifndef MIC_CLK_PIN
#define MIC_CLK_PIN             GPIO_NUM_4
#endif

#ifndef MIC_DATA_PIN
#define MIC_DATA_PIN            GPIO_NUM_5
#endif

// 音频参数
#ifndef AI_AUDIO_SAMPLE_RATE
#define AI_AUDIO_SAMPLE_RATE    16000
#endif

#ifndef AI_AUDIO_BITS
#define AI_AUDIO_BITS           16
#endif

#ifndef AI_AUDIO_CHANNELS
#define AI_AUDIO_CHANNELS       1
#endif

// 最长录音时长（秒），避免一直按住导致内存爆掉
#ifndef AI_MAX_RECORD_SECONDS
#define AI_MAX_RECORD_SECONDS   8
#endif
