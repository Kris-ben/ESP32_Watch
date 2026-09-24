#ifndef OTA_UPDATE_H
#define OTA_UPDATE_H

#include <stdbool.h>
#include <stddef.h>
#include "esp_err.h"

typedef struct {
    bool server_running;
    bool uploading;
    bool finished;
    int progress_percent;
    char message[64];
} ota_update_status_t;

/** 仅在 WiFi 已连接时启动局域网升级页面，监听 8080 端口。 */
esp_err_t ota_update_start(char *url, size_t url_size);

/** 停止等待上传；正在写入固件时返回 ESP_ERR_INVALID_STATE。 */
esp_err_t ota_update_stop(void);

/** 供设备界面轮询上传状态。 */
void ota_update_get_status(ota_update_status_t *status);

#endif
