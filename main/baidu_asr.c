/**
 * @file baidu_asr.c
 * @brief 百度语音识别（ASR）驱动实现
 */

#include "baidu_asr.h"
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include "esp_log.h"
#include "esp_http_client.h"
#include "esp_timer.h"
#include "esp_crt_bundle.h"
#include "esp_heap_caps.h"
#include "esp_netif.h"
#include "cJSON.h"
#include "mbedtls/base64.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static const char *TAG = "BaiduASR";

#ifndef BAIDU_ASR_MAX_AUDIO_BYTES
// 与对话录音上限一致：16kHz、16bit、单声道、10 秒。
#define BAIDU_ASR_MAX_AUDIO_BYTES  (320000)
#endif

static esp_http_client_handle_t baidu_http_client_init(const char *url,
                                                       esp_http_client_method_t method,
                                                       uint32_t timeout_ms)
{
    ESP_LOGI(TAG, "初始化HTTP客户端");
    
    // 限制超时时间，避免长时间等待
    if (timeout_ms > 10000) {
        timeout_ms = 10000;  // 减少到10秒
    }
    
    esp_http_client_config_t http_config = {
        .url = url,
        .method = method,
        .timeout_ms = timeout_ms,
        // 跳过证书验证
        .crt_bundle_attach = NULL,
        .cert_pem = NULL,
        .skip_cert_common_name_check = true,
        .use_global_ca_store = false,
        .keep_alive_enable = false,
        .buffer_size = 2048,        // 增加接收缓冲区
        .buffer_size_tx = 1024,     // 增加发送缓冲区
        .transport_type = HTTP_TRANSPORT_OVER_SSL,  // 明确指定SSL传输
        .is_async = false,
    };

    ESP_LOGI(TAG, "HTTP配置: timeout=%lums, buffer_rx=%d, buffer_tx=%d", 
             (unsigned long)timeout_ms, http_config.buffer_size, http_config.buffer_size_tx);
    return esp_http_client_init(&http_config);
}

// 百度语音识别API URL
#define BAIDU_ASR_URL "https://vop.baidu.com/server_api"
#define BAIDU_TOKEN_URL "https://aip.baidubce.com/oauth/2.0/token"

esp_err_t baidu_asr_init(baidu_asr_handle_t *handle, const baidu_asr_config_t *config)
{
    if (handle == NULL || config == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    if (config->api_key == NULL || config->secret_key == NULL ||
        config->api_key[0] == '\0' || config->secret_key[0] == '\0') {
        ESP_LOGE(TAG, "API Key或Secret Key为空");
        return ESP_ERR_INVALID_ARG;
    }

    memset(handle, 0, sizeof(baidu_asr_handle_t));
    handle->config = *config;
    
    // 设置默认值
    if (handle->config.sample_rate == 0) {
        handle->config.sample_rate = 16000;
    }
    if (handle->config.timeout_ms == 0) {
        handle->config.timeout_ms = 60000;
    }

    ESP_LOGI(TAG, "百度ASR初始化完成 (采样率: %lu Hz)", handle->config.sample_rate);
    return ESP_OK;
}

esp_err_t baidu_asr_get_token(baidu_asr_handle_t *handle)
{
    if (handle == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    if (handle->config.api_key == NULL || handle->config.secret_key == NULL ||
        handle->config.api_key[0] == '\0' || handle->config.secret_key[0] == '\0') {
        ESP_LOGE(TAG, "API Key或Secret Key为空");
        return ESP_ERR_INVALID_ARG;
    }

    // 检查令牌是否仍有效（提前1小时过期）
    int64_t now = esp_timer_get_time();
    if (handle->token_expire_time > 0 && now < handle->token_expire_time - 3600LL * 1000000LL) {
        ESP_LOGD(TAG, "使用缓存的访问令牌");
        return ESP_OK;
    }

    // 检查系统时间（仅作为警告，不阻塞）
    time_t current_time;
    time(&current_time);
    ESP_LOGI(TAG, "当前系统时间: %ld", (long)current_time);
    
    // 由于已跳过SSL验证，时间不正确也可以继续
    if (current_time < 1704067200) {  // 2024-01-01 00:00:00 UTC
        ESP_LOGW(TAG, "系统时间可能不正确 (%ld)，但已跳过SSL验证", (long)current_time);
    } else {
        ESP_LOGI(TAG, "✓ 系统时间正常: %ld", (long)current_time);
    }

    ESP_LOGI(TAG, "正在获取百度访问令牌...");
    
    // 打印内存信息用于诊断
    size_t free_heap = esp_get_free_heap_size();
    size_t free_internal = heap_caps_get_free_size(MALLOC_CAP_INTERNAL);
    size_t free_spiram = heap_caps_get_free_size(MALLOC_CAP_SPIRAM);
    ESP_LOGI(TAG, "内存状态: 总堆=%u, 内部RAM=%u, PSRAM=%u", 
             (unsigned)free_heap, (unsigned)free_internal, (unsigned)free_spiram);
    
    // 简化的网络检查 - 只警告不阻止
    esp_netif_t *netif = esp_netif_get_default_netif();
    if (netif != NULL) {
        esp_netif_ip_info_t ip_info;
        if (esp_netif_get_ip_info(netif, &ip_info) == ESP_OK && ip_info.ip.addr != 0) {
            ESP_LOGI(TAG, "网络已连接, IP: " IPSTR, IP2STR(&ip_info.ip));
        } else {
            ESP_LOGW(TAG, "网络接口存在但无IP");
        }
    } else {
        ESP_LOGW(TAG, "未找到默认网络接口");
    }

    // 构建URL
    char url[512];
    snprintf(url, sizeof(url),
             "%s?grant_type=client_credentials&client_id=%s&client_secret=%s",
             BAIDU_TOKEN_URL, handle->config.api_key, handle->config.secret_key);

    // 配置HTTP客户端（token接口使用GET）
    esp_http_client_handle_t client = baidu_http_client_init(url, HTTP_METHOD_GET,
                                                             handle->config.timeout_ms);
    if (client == NULL) {
        ESP_LOGE(TAG, "HTTP客户端初始化失败");
        return ESP_FAIL;
    }

    // 给WiFi任务足够的CPU时间处理管理帧
    vTaskDelay(pdMS_TO_TICKS(50));
    
    // 降低当前任务优先级让WiFi有更多时间
    UBaseType_t orig_priority = uxTaskPriorityGet(NULL);
    vTaskPrioritySet(NULL, tskIDLE_PRIORITY + 1);
    
    ESP_LOGI(TAG, "正在连接服务器...");
    esp_err_t err = esp_http_client_open(client, 0);
    
    // 恢复任务优先级
    vTaskPrioritySet(NULL, orig_priority);
    
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "HTTP连接失败: %s", esp_err_to_name(err));
        esp_http_client_cleanup(client);
        return ESP_FAIL;
    }
    
    // 连接成功后让出 CPU
    vTaskDelay(pdMS_TO_TICKS(10));

    // 动态缓冲区用于接收响应（放到连接成功后，降低握手内存峰值）
    size_t buf_cap = 2048;
    char *response_buf = malloc(buf_cap);
    if (response_buf == NULL) {
        ESP_LOGE(TAG, "内存分配失败");
        esp_http_client_close(client);
        esp_http_client_cleanup(client);
        return ESP_ERR_NO_MEM;
    }

    // 读取响应头
    int content_length = esp_http_client_fetch_headers(client);
    int status_code = esp_http_client_get_status_code(client);
    ESP_LOGI(TAG, "HTTP状态码: %d, 内容长度: %d", status_code, content_length);
    
    if (status_code != 200) {
        ESP_LOGE(TAG, "HTTP请求失败，状态码: %d", status_code);
        free(response_buf);
        esp_http_client_close(client);
        esp_http_client_cleanup(client);
        return ESP_FAIL;
    }

    // 读取响应体
    int total_read = 0;
    while (1) {
        // 确保缓冲区足够大
        if (total_read + 512 > (int)buf_cap) {
            size_t new_cap = buf_cap * 2;
            char *p = realloc(response_buf, new_cap);
            if (p == NULL) {
                ESP_LOGE(TAG, "内存重新分配失败");
                break;
            }
            response_buf = p;
            buf_cap = new_cap;
        }
        
        int r = esp_http_client_read(client, response_buf + total_read, 512);
        if (r > 0) {
            total_read += r;
        } else if (r == 0) {
            // 读完
            break;
        } else {
            // 错误
            ESP_LOGE(TAG, "读取响应数据失败，返回值: %d", r);
            break;
        }
    }

    esp_http_client_close(client);
    esp_http_client_cleanup(client);

    if (total_read <= 0) {
        ESP_LOGE(TAG, "读取响应失败，未读取到任何数据");
        free(response_buf);
        return ESP_FAIL;
    }
    
    response_buf[total_read] = '\0';
    ESP_LOGI(TAG, "成功读取响应: %d 字节", total_read);

    // 解析JSON响应
    cJSON *root = cJSON_Parse(response_buf);
    free(response_buf);

    if (root == NULL) {
        ESP_LOGE(TAG, "JSON解析失败");
        return ESP_FAIL;
    }

    cJSON *access_token = cJSON_GetObjectItem(root, "access_token");
    cJSON *expires_in = cJSON_GetObjectItem(root, "expires_in");

    if (access_token == NULL || !cJSON_IsString(access_token)) {
        cJSON *error = cJSON_GetObjectItem(root, "error_description");
        if (error && cJSON_IsString(error)) {
            ESP_LOGE(TAG, "获取令牌失败: %s", error->valuestring);
        } else {
            ESP_LOGE(TAG, "获取令牌失败: 响应中无access_token");
        }
        cJSON_Delete(root);
        return ESP_FAIL;
    }

    // 保存令牌
    strncpy(handle->access_token, access_token->valuestring, sizeof(handle->access_token) - 1);
    
    // 计算过期时间
    if (expires_in && cJSON_IsNumber(expires_in)) {
        handle->token_expire_time = esp_timer_get_time() + (int64_t)expires_in->valueint * 1000000LL;
    }

    cJSON_Delete(root);
    ESP_LOGI(TAG, "访问令牌获取成功");
    return ESP_OK;
}

static bool asr_cancel_requested(const volatile bool *cancel_requested)
{
    return cancel_requested != NULL &&
           __atomic_load_n(cancel_requested, __ATOMIC_ACQUIRE);
}

esp_err_t baidu_asr_recognize_with_cancel(baidu_asr_handle_t *handle,
                                           const int16_t *audio_data,
                                           size_t audio_len,
                                           char *result,
                                           size_t result_size,
                                           const volatile bool *cancel_requested)
{
    if (handle == NULL || audio_data == NULL || result == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    if (asr_cancel_requested(cancel_requested)) {
        result[0] = '\0';
        return ESP_ERR_INVALID_STATE;
    }

    if (audio_len == 0) {
        ESP_LOGW(TAG, "音频数据为空");
        result[0] = '\0';
        return ESP_OK;
    }

    // 确保有有效的访问令牌
    esp_err_t err = baidu_asr_get_token(handle);
    if (err != ESP_OK) {
        return err;
    }
    if (asr_cancel_requested(cancel_requested)) {
        result[0] = '\0';
        return ESP_ERR_INVALID_STATE;
    }

    size_t used_len = audio_len;
    if (used_len > BAIDU_ASR_MAX_AUDIO_BYTES) {
        used_len = BAIDU_ASR_MAX_AUDIO_BYTES;
        used_len &= ~((size_t)1); // 保持 2 字节对齐
        ESP_LOGW(TAG, "音频过长，保留开头 %u 字节用于识别", (unsigned)used_len);
    }

    const uint8_t *audio_ptr = (const uint8_t *)audio_data;

    ESP_LOGI(TAG, "开始语音识别，音频长度: %u 字节", (unsigned)used_len);

    // 计算Base64长度并在单一JSON缓冲区中构造请求，降低内存峰值
    const char *token = handle->access_token;
    size_t token_len = strlen(token);
    size_t overhead = 128 + token_len + 64; // JSON固定字段与安全余量

    char *json_str = NULL;
    size_t json_len = 0;
    size_t cur_len = used_len;

    for (; cur_len >= 8000; cur_len &= ~((size_t)1), cur_len >>= 1) {
        if (asr_cancel_requested(cancel_requested)) {
            return ESP_ERR_INVALID_STATE;
        }
        size_t base64_len = 0;
        mbedtls_base64_encode(NULL, 0, &base64_len, audio_ptr, cur_len);

        size_t json_cap = base64_len + overhead;
        json_str = heap_caps_malloc(json_cap, MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
        if (json_str == NULL) {
            json_str = malloc(json_cap);
        }

        if (json_str == NULL) {
            ESP_LOGW(TAG, "JSON缓冲区分配失败，缩短音频到 %u 字节", (unsigned)(cur_len >> 1));
            continue;
        }

        int prefix_len = snprintf(json_str, json_cap,
                                  "{\"format\":\"pcm\",\"rate\":%lu,\"channel\":1,\"token\":\"%s\",\"cuid\":\"ESP32_ASR_Device\",\"len\":%u,\"speech\":\"",
                                  (unsigned long)handle->config.sample_rate, token, (unsigned)cur_len);
        if (prefix_len < 0 || (size_t)prefix_len >= json_cap) {
            ESP_LOGE(TAG, "JSON前缀构建失败");
            free(json_str);
            return ESP_FAIL;
        }

        size_t remain = json_cap - (size_t)prefix_len - 3; // 预留 ""} 和 \0
        size_t actual_len = 0;
        int enc_ret = mbedtls_base64_encode((unsigned char *)json_str + prefix_len, remain, &actual_len,
                                            audio_ptr, cur_len);
        if (enc_ret != 0) {
            ESP_LOGE(TAG, "Base64编码失败: %d", enc_ret);
            free(json_str);
            return ESP_FAIL;
        }

        json_len = (size_t)prefix_len + actual_len;
        if (json_len + 3 > json_cap) {
            ESP_LOGE(TAG, "JSON缓冲区不足");
            free(json_str);
            return ESP_FAIL;
        }

        json_str[json_len++] = '"';
        json_str[json_len++] = '}';
        json_str[json_len] = '\0';

        if (cur_len != used_len) {
            ESP_LOGW(TAG, "内存不足，最终使用 %u 字节音频", (unsigned)cur_len);
        }
        break;
    }

    if (json_str == NULL) {
        ESP_LOGE(TAG, "JSON缓冲区分配失败");
        return ESP_ERR_NO_MEM;
    }

    if (asr_cancel_requested(cancel_requested)) {
        free(json_str);
        return ESP_ERR_INVALID_STATE;
    }

    ESP_LOGI(TAG, "请求JSON长度: %u 字节", (unsigned)json_len);

    // 配置HTTP客户端
    uint32_t asr_timeout_ms = handle->config.timeout_ms;
    if (asr_timeout_ms < 5000) {
        asr_timeout_ms = 5000;  // 打断监听可设置较短超时；普通对话仍配置为30秒
    }
    if (asr_timeout_ms > 60000) {
        asr_timeout_ms = 60000;  // 最多60秒
    }

    esp_http_client_handle_t client = baidu_http_client_init(BAIDU_ASR_URL, HTTP_METHOD_POST,
                                                             asr_timeout_ms);
    if (client == NULL) {
        ESP_LOGE(TAG, "HTTP客户端初始化失败");
        free(json_str);
        return ESP_FAIL;
    }

    esp_http_client_set_header(client, "Content-Type", "application/json");
    esp_http_client_set_header(client, "Connection", "close");
    esp_http_client_set_timeout_ms(client, asr_timeout_ms);

    // 打开连接并发送POST数据
    err = esp_http_client_open(client, json_len);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "HTTP连接失败: %s", esp_err_to_name(err));
        free(json_str);
        esp_http_client_cleanup(client);
        return ESP_FAIL;
    }

    // 写入POST数据（分块发送，避免大数据包被拒绝）
    size_t total_written = 0;
    const size_t chunk_size = 2048;
    
    while (total_written < json_len) {
        if (asr_cancel_requested(cancel_requested)) {
            free(json_str);
            esp_http_client_close(client);
            esp_http_client_cleanup(client);
            return ESP_ERR_INVALID_STATE;
        }
        size_t to_write = json_len - total_written;
        if (to_write > chunk_size) {
            to_write = chunk_size;
        }
        
        int written = esp_http_client_write(client, json_str + total_written, to_write);
        if (written <= 0) {
            ESP_LOGE(TAG, "发送请求失败（已发送 %u/%u 字节）", total_written, json_len);
            free(json_str);
            esp_http_client_close(client);
            esp_http_client_cleanup(client);
            return ESP_FAIL;
        }
        
        total_written += written;
        ESP_LOGD(TAG, "已发送 %u/%u 字节", total_written, json_len);
        
    }
    
    ESP_LOGI(TAG, "请求发送完成: %u 字节", total_written);
    free(json_str);

    if (asr_cancel_requested(cancel_requested)) {
        esp_http_client_close(client);
        esp_http_client_cleanup(client);
        return ESP_ERR_INVALID_STATE;
    }

    // 读取响应头以确定缓冲区大小（必须在发送完POST数据后进行）
    int content_length = esp_http_client_fetch_headers(client);
    int status_code = esp_http_client_get_status_code(client);
    ESP_LOGI(TAG, "HTTP状态码: %d, 内容长度: %d", status_code, content_length);

    if (status_code != 200) {
        ESP_LOGE(TAG, "HTTP请求失败，状态码: %d", status_code);
        esp_http_client_close(client);
        esp_http_client_cleanup(client);
        return ESP_FAIL;
    }

    if (asr_cancel_requested(cancel_requested)) {
        esp_http_client_close(client);
        esp_http_client_cleanup(client);
        return ESP_ERR_INVALID_STATE;
    }

    // 动态缓冲区用于接收响应（连接成功后分配，按需缩小）
    size_t buf_cap2 = 1024;
    if (content_length > 0) {
        size_t need = (size_t)content_length + 1;
        if (need > 2048) {
            need = 2048; // 上限，响应内容通常很小
        }
        if (need > buf_cap2) {
            buf_cap2 = need;
        }
    }

    char *response_buf2 = heap_caps_malloc(buf_cap2, MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
    if (response_buf2 == NULL) {
        response_buf2 = malloc(buf_cap2);
    }
    if (response_buf2 == NULL) {
        ESP_LOGE(TAG, "响应缓冲区分配失败");
        esp_http_client_close(client);
        esp_http_client_cleanup(client);
        return ESP_ERR_NO_MEM;
    }

    // 读取响应体
    int total_read2 = 0;
    while (1) {
        if (asr_cancel_requested(cancel_requested)) {
            free(response_buf2);
            esp_http_client_close(client);
            esp_http_client_cleanup(client);
            return ESP_ERR_INVALID_STATE;
        }
        // 确保缓冲区足够大
        if (total_read2 + 512 > (int)buf_cap2) {
            // 不再扩容，避免内存峰值
            ESP_LOGW(TAG, "响应缓冲区已满，截断读取");
            break;
        }
        
        int r = esp_http_client_read(client, response_buf2 + total_read2, 512);
        if (r > 0) {
            total_read2 += r;
        } else if (r == 0) {
            // 读完
            break;
        } else {
            // 错误
            ESP_LOGE(TAG, "读取响应数据失败，返回值: %d", r);
            break;
        }
    }

    esp_http_client_close(client);
    esp_http_client_cleanup(client);

    if (total_read2 <= 0) {
        ESP_LOGE(TAG, "读取响应失败，未读取到任何数据");
        free(response_buf2);
        return ESP_FAIL;
    }
    
    response_buf2[total_read2] = '\0';
    ESP_LOGI(TAG, "成功读取响应: %d 字节", total_read2);
    
    // 打印原始响应（前1000字节）
    if (total_read2 <= 1000) {
        ESP_LOGI(TAG, "ASR原始响应: %s", response_buf2);
    } else {
        ESP_LOGI(TAG, "ASR原始响应(前1000字节): %.1000s...", response_buf2);
    }

    // 解析响应
    cJSON *root = cJSON_Parse(response_buf2);
    free(response_buf2);

    if (root == NULL) {
        ESP_LOGE(TAG, "响应JSON解析失败");
        return ESP_FAIL;
    }

    cJSON *err_no = cJSON_GetObjectItem(root, "err_no");
    if (err_no && cJSON_IsNumber(err_no) && err_no->valueint != 0) {
        cJSON *err_msg = cJSON_GetObjectItem(root, "err_msg");
        ESP_LOGE(TAG, "识别错误 %d: %s", err_no->valueint, 
                 err_msg ? err_msg->valuestring : "未知错误");
        cJSON_Delete(root);
        return ESP_FAIL;
    }

    // 获取识别结果
    cJSON *result_array = cJSON_GetObjectItem(root, "result");
    if (result_array && cJSON_IsArray(result_array) && cJSON_GetArraySize(result_array) > 0) {
        cJSON *first_result = cJSON_GetArrayItem(result_array, 0);
        if (first_result && cJSON_IsString(first_result)) {
            strncpy(result, first_result->valuestring, result_size - 1);
            result[result_size - 1] = '\0';
            ESP_LOGI(TAG, "识别结果: %s", result);
        } else {
            result[0] = '\0';
        }
    } else {
        result[0] = '\0';
        char *json_str = cJSON_Print(root);
        ESP_LOGW(TAG, "识别结果为空，完整JSON: %s", json_str ? json_str : "null");
        if (json_str) free(json_str);
    }

    cJSON_Delete(root);
    return ESP_OK;
}

esp_err_t baidu_asr_recognize(baidu_asr_handle_t *handle,
                               const int16_t *audio_data,
                               size_t audio_len,
                               char *result,
                               size_t result_size)
{
    return baidu_asr_recognize_with_cancel(handle, audio_data, audio_len,
                                           result, result_size, NULL);
}
