#ifndef SD_SERIAL_TRANSFER_H
#define SD_SERIAL_TRANSFER_H

#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

/* SD 卡挂载成功后启动；通过控制台 UART0 接收 Ogg 文件。 */
esp_err_t sd_serial_transfer_start(void);

#ifdef __cplusplus
}
#endif

#endif
