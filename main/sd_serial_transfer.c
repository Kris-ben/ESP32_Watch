#include "sd_serial_transfer.h"

#include <ctype.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>

#include "driver/uart.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "sd_card_fs.h"

#define TRANSFER_UART UART_NUM_0
#define TRANSFER_CHUNK 512
#define TRANSFER_MAX_BYTES (20U * 1024U * 1024U)
#define TRANSFER_NAME_MAX 63

static const char *TAG = "SD_UPLOAD";
static bool s_started;

static uint32_t read_u32_le(const uint8_t *bytes)
{
    return (uint32_t)bytes[0] | ((uint32_t)bytes[1] << 8) |
           ((uint32_t)bytes[2] << 16) | ((uint32_t)bytes[3] << 24);
}

static void send_reply(bool ok)
{
    const uint8_t reply[] = {0xa5, 0x5a, ok ? 0x06 : 0x15};
    (void)uart_write_bytes(TRANSFER_UART, reply, sizeof(reply));
}

static bool read_exact(uint8_t *buffer, size_t length, uint32_t timeout_ms)
{
    size_t received = 0;
    while (received < length) {
        int n = uart_read_bytes(TRANSFER_UART, buffer + received, length - received,
                                pdMS_TO_TICKS(timeout_ms));
        if (n <= 0) return false;
        received += (size_t)n;
    }
    return true;
}

static bool valid_name(const char *name, size_t length)
{
    if (length < 5 || length > TRANSFER_NAME_MAX) return false;
    if (strcmp(name + length - 4, ".ogg") != 0) return false;
    for (size_t i = 0; i < length; i++) {
        unsigned char c = (unsigned char)name[i];
        if (!isalnum(c) && c != '_' && c != '-' && c != '.') return false;
    }
    if (name[0] == '.') return false;
    return true;
}

static uint32_t crc32_update(uint32_t crc, const uint8_t *bytes, size_t length)
{
    for (size_t i = 0; i < length; i++) {
        crc ^= bytes[i];
        for (int bit = 0; bit < 8; bit++) {
            crc = (crc >> 1) ^ ((crc & 1U) ? 0xedb88320U : 0U);
        }
    }
    return crc;
}

static void receive_file(void)
{
    uint8_t header[9];
    if (!read_exact(header, sizeof(header), 5000)) return;

    uint8_t name_length = header[0];
    uint32_t file_size = read_u32_le(header + 1);
    uint32_t expected_crc = read_u32_le(header + 5);
    if (name_length == 0 || name_length > TRANSFER_NAME_MAX ||
        file_size == 0 || file_size > TRANSFER_MAX_BYTES) {
        ESP_LOGW(TAG, "文件头不合法");
        send_reply(false);
        return;
    }

    char name[TRANSFER_NAME_MAX + 1];
    if (!read_exact((uint8_t *)name, name_length, 5000)) return;
    name[name_length] = '\0';
    if (!valid_name(name, name_length)) {
        ESP_LOGW(TAG, "文件名不合法");
        send_reply(false);
        return;
    }

    char destination[sizeof(SD_MOUNT_POINT) + TRANSFER_NAME_MAX + 2];
    char temporary[sizeof(destination) + 5];
    snprintf(destination, sizeof(destination), "%s/%s", SD_MOUNT_POINT, name);
    snprintf(temporary, sizeof(temporary), "%s.part", destination);
    struct stat st;
    if (stat(destination, &st) == 0 || stat(temporary, &st) == 0) {
        ESP_LOGW(TAG, "目标文件已存在: %s", name);
        send_reply(false);
        return;
    }

    FILE *file = fopen(temporary, "wb");
    if (!file) {
        ESP_LOGE(TAG, "无法在 SD 卡上创建临时文件: %s", name);
        send_reply(false);
        return;
    }
    ESP_LOGI(TAG, "开始接收 %s (%lu 字节)", name, (unsigned long)file_size);
    send_reply(true);

    uint8_t chunk[TRANSFER_CHUNK];
    uint32_t remaining = file_size;
    uint32_t crc = 0xffffffffU;
    bool ok = true;
    while (remaining > 0) {
        size_t count = remaining < sizeof(chunk) ? remaining : sizeof(chunk);
        if (!read_exact(chunk, count, 10000) || fwrite(chunk, 1, count, file) != count) {
            ok = false;
            break;
        }
        crc = crc32_update(crc, chunk, count);
        remaining -= (uint32_t)count;
        send_reply(true);
    }
    if (fclose(file) != 0) ok = false;
    if ((~crc) != expected_crc) ok = false;
    if (ok && rename(temporary, destination) != 0) ok = false;
    if (!ok) {
        (void)remove(temporary);
        ESP_LOGE(TAG, "接收或校验失败: %s", name);
    } else {
        ESP_LOGI(TAG, "接收并校验成功: %s", name);
    }
    send_reply(ok);
}

static void transfer_task(void *arg)
{
    (void)arg;
    const uint8_t magic[] = {'W', 'S', 'D', '1'};
    size_t matched = 0;
    while (true) {
        uint8_t byte;
        if (!read_exact(&byte, 1, 1000)) continue;
        matched = byte == magic[matched] ? matched + 1 : (byte == magic[0] ? 1 : 0);
        if (matched == sizeof(magic)) {
            matched = 0;
            receive_file();
        }
    }
}

esp_err_t sd_serial_transfer_start(void)
{
    if (s_started) return ESP_OK;
    if (!uart_is_driver_installed(TRANSFER_UART)) {
        esp_err_t err = uart_driver_install(TRANSFER_UART, 4096, 0, 0, NULL, 0);
        if (err != ESP_OK) return err;
    }
    if (xTaskCreate(transfer_task, "sd_upload", 4096, NULL, 3, NULL) != pdPASS) {
        return ESP_ERR_NO_MEM;
    }
    s_started = true;
    ESP_LOGI(TAG, "USB 串口音乐导入已就绪");
    return ESP_OK;
}
