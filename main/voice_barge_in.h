#ifndef VOICE_BARGE_IN_H
#define VOICE_BARGE_IN_H

#include <stdint.h>
#include "MSM261D4030H1CPM_Driver_V5.hpp"
#include "baidu_asr.h"
#include "baidu_tts.h"

struct voice_barge_in;

// 仅在播报期间调用；此时对话任务不能同时读取同一个麦克风。
voice_barge_in *voice_barge_in_start(MSM261D4030H1CPM::MicDriver *mic,
                                    const baidu_asr_handle_t *asr,
                                    baidu_tts_handle_t *tts,
                                    const char *wakeup_word,
                                    uint16_t vad_threshold);

// 等待监听任务退出并释放资源，返回播报期间是否听到唤醒词。
bool voice_barge_in_stop(voice_barge_in *monitor);

#endif
