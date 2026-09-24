/**
 * @file voice_local_intent.h
 * @brief 本地语音意图处理（不依赖大模型）
 *
 * 作用：把"设备本机就能完成"的语音指令在本地直接处理掉，不走大模型。
 *
 * 覆盖范围：天气播报、设备步数、报时、音量、亮度、闹钟、切换界面。
 *
 * 好处：响应快（不用等大模型 3~5 秒）、不消耗大模型额度、断网也能用。
 * 未命中的说法会返回 false，交给调用方去问大模型，两者互补。
 */

#pragma once

#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief 尝试用本地规则理解用户这句话，命中则执行设备控制并生成回复文本
 *
 * @param user_text  ASR 识别出的用户原话
 * @param reply      输出：要播报的回复文本
 * @param reply_len  reply 缓冲区大小（建议 >= 192）
 * @return true 已本地处理（调用方直接播报 reply，不要再问大模型）；
 *         false 未命中，请交给大模型处理
 */
bool voice_local_intent_try_handle(const char *user_text, char *reply, size_t reply_len);

#ifdef __cplusplus
}
#endif
