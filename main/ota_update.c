#include "ota_update.h"

#include <stdio.h>
#include <string.h>

#include "esp_http_server.h"
#include "esp_log.h"
#include "esp_netif.h"
#include "esp_ota_ops.h"
#include "esp_timer.h"
#include "freertos/FreeRTOS.h"
#include "freertos/portmacro.h"
#include "wifi_connect.h"

#define OTA_HTTP_PORT 8080
#define OTA_RECV_BUFFER_SIZE 2048

static const char *TAG = "OtaUpdate";
static httpd_handle_t s_server;
static portMUX_TYPE s_status_lock = portMUX_INITIALIZER_UNLOCKED;
static ota_update_status_t s_status;

static const char s_page[] =
    "<!doctype html><html lang='zh-CN'><head><meta charset='utf-8'>"
    "<meta name='viewport' content='width=device-width,initial-scale=1'>"
    "<title>设备固件更新</title><style>"
    "body{background:#0b1320;color:#eef5ff;font-family:system-ui,sans-serif;margin:0;padding:24px}"
    "main{max-width:420px;margin:30px auto;background:#1a2a3d;border-radius:18px;padding:22px}"
    "h1{font-size:22px;margin:0 0 14px}p{color:#b9cce2;line-height:1.6}"
    "input{display:block;margin:18px 0;width:100%;color:#dcecff}"
    "button{width:100%;border:0;border-radius:10px;background:#2585df;color:white;padding:13px;font-size:16px}"
    "button:disabled{opacity:.5}progress{width:100%;height:18px;margin-top:18px}"
    "#msg{min-height:24px}</style></head><body><main>"
    "<h1>无线更新固件</h1><p>请选择本项目编译生成的 esp32_s3_watch.bin。"
    "手机与设备需连接同一 WiFi；上传完成前保持设备供电。</p>"
    "<input id='file' type='file' accept='.bin,application/octet-stream'>"
    "<button id='send' onclick='upload()'>开始更新</button>"
    "<progress id='bar' max='100' value='0'></progress><p id='msg'>等待选择固件</p>"
    "<script>function upload(){const f=document.getElementById('file').files[0],"
    "b=document.getElementById('send'),m=document.getElementById('msg'),"
    "p=document.getElementById('bar');if(!f){m.textContent='请先选择 .bin 文件';return;}"
    "if(!confirm('确定更新固件吗？'))return;b.disabled=true;"
    "const x=new XMLHttpRequest();x.open('POST','/update');"
    "x.setRequestHeader('Content-Type','application/octet-stream');"
    "x.upload.onprogress=e=>{if(e.lengthComputable){p.value=Math.round(e.loaded/e.total*100);"
    "m.textContent='上传中 '+p.value+'%';}};"
    "x.onload=()=>{m.textContent=x.responseText||'设备无响应';if(x.status!==200)b.disabled=false;};"
    "x.onerror=()=>{m.textContent='连接中断，请检查设备状态';b.disabled=false;};"
    "x.send(f);}</script></main></body></html>";

static void set_status(bool uploading, bool finished, int percent,
                       const char *message)
{
    portENTER_CRITICAL(&s_status_lock);
    s_status.uploading = uploading;
    s_status.finished = finished;
    s_status.progress_percent = percent;
    snprintf(s_status.message, sizeof(s_status.message), "%s", message);
    portEXIT_CRITICAL(&s_status_lock);
}

void ota_update_get_status(ota_update_status_t *status)
{
    if (!status) return;
    portENTER_CRITICAL(&s_status_lock);
    *status = s_status;
    portEXIT_CRITICAL(&s_status_lock);
}

static void reboot_cb(void *arg)
{
    (void)arg;
    esp_restart();
}

static void schedule_reboot(void)
{
    esp_timer_handle_t timer;
    const esp_timer_create_args_t args = {
        .callback = reboot_cb,
        .name = "ota_reboot",
    };
    if (esp_timer_create(&args, &timer) == ESP_OK) {
        (void)esp_timer_start_once(timer, 1500000);
    } else {
        esp_restart();
    }
}

static esp_err_t page_handler(httpd_req_t *req)
{
    httpd_resp_set_type(req, "text/html; charset=utf-8");
    httpd_resp_set_hdr(req, "Cache-Control", "no-store");
    return httpd_resp_send(req, s_page, HTTPD_RESP_USE_STRLEN);
}

static esp_err_t send_error(httpd_req_t *req, const char *http_status,
                            const char *message)
{
    httpd_resp_set_status(req, http_status);
    httpd_resp_set_type(req, "text/plain; charset=utf-8");
    return httpd_resp_sendstr(req, message);
}

static esp_err_t upload_handler(httpd_req_t *req)
{
    const esp_partition_t *partition = esp_ota_get_next_update_partition(NULL);
    if (!partition || req->content_len <= 0 ||
        (size_t)req->content_len > partition->size) {
        return send_error(req, "413 Payload Too Large", "固件为空或超过更新分区容量");
    }

    ota_update_status_t status;
    ota_update_get_status(&status);
    if (status.uploading || status.finished) {
        return send_error(req, "409 Conflict", "设备正在更新或等待重启");
    }

    set_status(true, false, 0, "正在接收固件");
    esp_ota_handle_t handle = 0;
    esp_err_t err = esp_ota_begin(partition, req->content_len, &handle);
    if (err != ESP_OK) {
        set_status(false, false, 0, "无法准备更新分区");
        ESP_LOGE(TAG, "esp_ota_begin: %s", esp_err_to_name(err));
        return send_error(req, "500 Internal Server Error", "无法准备更新分区");
    }

    uint8_t buffer[OTA_RECV_BUFFER_SIZE];
    size_t received = 0;
    int timeouts = 0;
    while (received < (size_t)req->content_len) {
        size_t wanted = (size_t)req->content_len - received;
        if (wanted > sizeof(buffer)) wanted = sizeof(buffer);
        int count = httpd_req_recv(req, (char *)buffer, wanted);
        if (count == HTTPD_SOCK_ERR_TIMEOUT && ++timeouts < 5) continue;
        if (count <= 0) {
            err = ESP_ERR_TIMEOUT;
            break;
        }
        timeouts = 0;
        if (received == 0 && buffer[0] != 0xE9) {
            err = ESP_ERR_INVALID_ARG;
            break;
        }
        err = esp_ota_write(handle, buffer, count);
        if (err != ESP_OK) break;
        received += count;
        int percent = (int)(received * 100 / (size_t)req->content_len);
        if (percent != status.progress_percent) {
            status.progress_percent = percent;
            set_status(true, false, percent, "正在写入固件");
        }
    }

    if (err != ESP_OK || received != (size_t)req->content_len) {
        esp_ota_abort(handle);
        set_status(false, false, 0, "更新失败，可重新上传");
        ESP_LOGE(TAG, "固件接收失败: %s (%u/%d)", esp_err_to_name(err),
                 (unsigned)received, req->content_len);
        return send_error(req, "400 Bad Request", "固件无效或传输中断，设备没有重启");
    }

    err = esp_ota_end(handle);
    if (err != ESP_OK) {
        set_status(false, false, 0, "固件校验失败");
        ESP_LOGE(TAG, "esp_ota_end: %s", esp_err_to_name(err));
        return send_error(req, "400 Bad Request", "固件校验失败，请确认 .bin 文件正确");
    }

    err = esp_ota_set_boot_partition(partition);
    if (err != ESP_OK) {
        set_status(false, false, 0, "设置启动分区失败");
        ESP_LOGE(TAG, "esp_ota_set_boot_partition: %s", esp_err_to_name(err));
        return send_error(req, "500 Internal Server Error", "设置启动分区失败");
    }

    set_status(false, true, 100, "更新成功，即将重启");
    ESP_LOGI(TAG, "升级成功，启动分区: %s", partition->label);
    httpd_resp_set_type(req, "text/plain; charset=utf-8");
    esp_err_t response = httpd_resp_sendstr(req, "固件更新成功，设备即将重启");
    schedule_reboot();
    return response;
}

esp_err_t ota_update_start(char *url, size_t url_size)
{
    if (!url || url_size == 0) return ESP_ERR_INVALID_ARG;
    if (!wifi_connect_is_connected()) return ESP_ERR_INVALID_STATE;

    esp_netif_t *netif = esp_netif_get_handle_from_ifkey("WIFI_STA_DEF");
    esp_netif_ip_info_t ip = {0};
    if (!netif || esp_netif_get_ip_info(netif, &ip) != ESP_OK || ip.ip.addr == 0) {
        return ESP_ERR_INVALID_STATE;
    }
    snprintf(url, url_size, "http://" IPSTR ":%d/", IP2STR(&ip.ip), OTA_HTTP_PORT);

    if (s_server) return ESP_OK;
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    config.server_port = OTA_HTTP_PORT;
    config.ctrl_port = 32769;
    config.stack_size = 8192;
    config.recv_wait_timeout = 15;
    config.send_wait_timeout = 15;
    esp_err_t err = httpd_start(&s_server, &config);
    if (err != ESP_OK) return err;

    const httpd_uri_t page = { .uri = "/", .method = HTTP_GET, .handler = page_handler };
    const httpd_uri_t upload = { .uri = "/update", .method = HTTP_POST, .handler = upload_handler };
    err = httpd_register_uri_handler(s_server, &page);
    if (err == ESP_OK) err = httpd_register_uri_handler(s_server, &upload);
    if (err != ESP_OK) {
        httpd_stop(s_server);
        s_server = NULL;
        return err;
    }

    portENTER_CRITICAL(&s_status_lock);
    s_status.server_running = true;
    portEXIT_CRITICAL(&s_status_lock);
    set_status(false, false, 0, "等待上传固件");
    ESP_LOGI(TAG, "局域网升级页面已启动: %s", url);
    return ESP_OK;
}

esp_err_t ota_update_stop(void)
{
    ota_update_status_t status;
    ota_update_get_status(&status);
    if (status.uploading) return ESP_ERR_INVALID_STATE;
    if (!s_server) return ESP_OK;
    esp_err_t err = httpd_stop(s_server);
    if (err == ESP_OK) s_server = NULL;
    portENTER_CRITICAL(&s_status_lock);
    s_status.server_running = s_server != NULL;
    portEXIT_CRITICAL(&s_status_lock);
    return err;
}
