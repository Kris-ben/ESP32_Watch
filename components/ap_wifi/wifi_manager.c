#include "wifi_manager.h"
#include <stdio.h>
#include "esp_log.h"
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include <stdlib.h>

#include "esp_netif.h"
#include "esp_mac.h"
#include "esp_wifi.h"
#include "esp_event.h"

#include "lwip/ip4_addr.h"
#define TAG     "wifi_manager"

//重连次数
#define MAX_CONNECT_RETRY   6
static int sta_connect_count = 0;

static esp_netif_t *esp_netif_sta = NULL;
static esp_netif_t *esp_netif_ap = NULL;

//AP模式下的SSID名称
static const char* ap_ssid_name = "ESP32-AP";

//AP模式下的密码
static const char* ap_password = "12345678";

//回调函数
static p_wifi_state_callback    wifi_state_cb = NULL;

//当前sta连接状态
static bool is_sta_connected = false;
static volatile bool scan_in_progress = false;
static volatile bool scan_reconnect_needed = false;

/** 把断开原因码翻译成更容易看懂的说明（定位连不上WiFi的原因用）
 * @param reason 断开原因码
 * @return 说明字符串
*/
static const char *disconnect_reason_str(uint8_t reason)
{
    switch (reason)
    {
    case WIFI_REASON_AUTH_EXPIRE:            return "认证超时(2)";
    case WIFI_REASON_4WAY_HANDSHAKE_TIMEOUT: return "四次握手超时(15)，多半是密码不对";
    case WIFI_REASON_BEACON_TIMEOUT:         return "丢失信标(200)，信号太弱";
    case WIFI_REASON_NO_AP_FOUND:            return "找不到这个热点(201)：名字不对/不在范围/不是2.4G";
    case WIFI_REASON_AUTH_FAIL:              return "认证失败(202)，密码不对";
    case WIFI_REASON_ASSOC_FAIL:             return "关联失败(203)";
    case WIFI_REASON_HANDSHAKE_TIMEOUT:      return "握手超时(204)，多半是密码不对";
    case WIFI_REASON_CONNECTION_FAIL:        return "连接失败(205)，热点可能不在范围";
    default:                                 return "其他原因";
    }
}

/** 事件回调函数
 * @param arg   用户传递的参数
 * @param event_base    事件类别
 * @param event_id      事件ID
 * @param event_data    事件携带的数据
 * @return 无
*/
static void event_handler(void* arg, esp_event_base_t event_base,int32_t event_id, void* event_data)
{   
    if(event_base == WIFI_EVENT)
    {
        switch (event_id)
        {
        case WIFI_EVENT_STA_START:      //WIFI以STA模式启动后触发此事件
        {
            wifi_mode_t mode;
            esp_wifi_get_mode(&mode);
            if(mode == WIFI_MODE_STA)
                esp_wifi_connect();         //启动WIFI连接
            break;
        }
        case WIFI_EVENT_STA_CONNECTED:  //WIFI连上路由器后，触发此事件
            ESP_LOGI(TAG, "Connected to AP");
            break;
        case WIFI_EVENT_STA_DISCONNECTED:   //WIFI从路由器断开连接后触发此事件
        {
            wifi_event_sta_disconnected_t *disc = (wifi_event_sta_disconnected_t *)event_data;
            ESP_LOGW(TAG, "connect to the AP fail: reason=%d %s",
                     disc ? disc->reason : -1,
                     disc ? disconnect_reason_str(disc->reason) : "");
            if(is_sta_connected)
            {
                if(wifi_state_cb)
                    wifi_state_cb(WIFI_STATE_DISCONNECTED);
                is_sta_connected = false;
            }
            if (scan_in_progress) {
                // 扫描需要暂时让出连接过程，完成后统一重连。
                scan_reconnect_needed = true;
                break;
            }
            if(sta_connect_count < MAX_CONNECT_RETRY)
            {
                wifi_mode_t mode;
                esp_wifi_get_mode(&mode);
                if(mode == WIFI_MODE_STA)
                    esp_wifi_connect();             //继续重连
                sta_connect_count++;
            }
            break;
        }
        case WIFI_EVENT_AP_STACONNECTED:
        {
            //有设备连接了热点，把它的MAC打印出来
            wifi_event_ap_staconnected_t *event = (wifi_event_ap_staconnected_t *) event_data;
            ESP_LOGI(TAG, "Station "MACSTR" joined, AID=%d",
                    MAC2STR(event->mac), event->aid);
            break;
        }
        case WIFI_EVENT_AP_STADISCONNECTED:
        {
            //有设备断开了热点
            wifi_event_ap_stadisconnected_t *event = (wifi_event_ap_stadisconnected_t *) event_data;
            ESP_LOGI(TAG, "Station "MACSTR" left, AID=%d",
                    MAC2STR(event->mac), event->aid);
            break;
        }
        default:
            break;
        }
    }
    if(event_base == IP_EVENT)                  //IP相关事件
    {
        switch(event_id)
        {
            case IP_EVENT_STA_GOT_IP:           //只有获取到路由器分配的IP，才认为是连上了路由器
                ESP_LOGI(TAG,"Get ip address");
                is_sta_connected = true;
                if(wifi_state_cb)
                    wifi_state_cb(WIFI_STATE_CONNECTED);
                break;
            default:break;
        }
    }
}

/** 初始化wifi，默认进入STA模式
 * @param 无
 * @return 无 
*/
void wifi_manager_init(p_wifi_state_callback f)
{
    ESP_ERROR_CHECK(esp_netif_init());  //用于初始化tcpip协议栈
    ESP_ERROR_CHECK(esp_event_loop_create_default());       //创建一个默认系统事件调度循环，之后可以注册回调函数来处理系统的一些事件
    esp_netif_sta = esp_netif_create_default_wifi_sta();    //使用默认配置创建STA对象
    esp_netif_ap = esp_netif_create_default_wifi_ap();      //使用默认配置创建AP对象
    //初始化WIFI
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    
    //注册事件
    ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT,ESP_EVENT_ANY_ID,&event_handler,NULL));
    ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT,IP_EVENT_STA_GOT_IP,&event_handler,NULL));

    wifi_state_cb = f;
    //启动WIFI
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA) );         //设置工作模式为STA
    ESP_ERROR_CHECK(esp_wifi_start() );                         //启动WIFI
    
    ESP_LOGI(TAG, "wifi_init finished.");
}

/** 进入ap+sta模式
 * @param 无
 * @return 成功/失败
*/
esp_err_t wifi_manager_ap(void)
{
    wifi_mode_t mode;
    esp_wifi_get_mode(&mode);
    if(mode == WIFI_MODE_APSTA) //需要使用AP+STA模式，才可以执行扫描同时保持客户端连接
        return ESP_OK;
    esp_wifi_disconnect();
    esp_wifi_stop();
    esp_wifi_set_mode(WIFI_MODE_APSTA);
    wifi_config_t wifi_config = 
    {
        .ap = 
        {
            .channel = 5,               //wifi的通信信道
            .max_connection = 2,        //最大连接数
            .authmode = WIFI_AUTH_WPA2_PSK, //加密方式
        }
    };
    //填充ap的ssid名称
    snprintf((char*)wifi_config.ap.ssid,31,"%s",ap_ssid_name);
    wifi_config.ap.ssid_len = strlen(ap_ssid_name);
    //填充密码
    snprintf((char*)wifi_config.ap.password,63,"%s",ap_password);

    //设置wifi
    esp_wifi_set_config(WIFI_IF_AP,&wifi_config);

    //如果是AP模式，则需要设置如下网络层信息
    esp_netif_ip_info_t ipInfo;
    IP4_ADDR(&ipInfo.ip, 192,168,100,1);    //本地的IP地址
	IP4_ADDR(&ipInfo.gw, 192,168,100,1);    //网关IP地址
	IP4_ADDR(&ipInfo.netmask, 255,255,255,0);   //子网掩码
	esp_netif_dhcps_stop(esp_netif_ap);
	esp_netif_set_ip_info(esp_netif_ap, &ipInfo);
	esp_netif_dhcps_start(esp_netif_ap);

    esp_wifi_start();
    return ESP_OK;
}

static SemaphoreHandle_t scan_sem = NULL;

/** 扫描任务
 * @param 无
 * @return 成功/失败
*/
static void scan_task(void* param)
{
    p_wifi_scan_callback callback = (p_wifi_scan_callback)param;
    uint16_t number = 20;
    wifi_ap_record_t *ap_info = malloc(sizeof(wifi_ap_record_t)*number);
    uint16_t ap_count = 0;
    ESP_LOGI(TAG,"Start wifi scan");

    if (!ap_info) {
        ESP_LOGE(TAG, "wifi scan ap_info malloc failed");
        if (callback) callback(-1, NULL);
        bool reconnect = scan_reconnect_needed;
        scan_reconnect_needed = false;
        scan_in_progress = false;
        if (reconnect) esp_wifi_connect();
        xSemaphoreGive(scan_sem);
        vTaskDelete(NULL);
        return;
    }

    esp_err_t err = esp_wifi_scan_start(NULL, true);
    if (err == ESP_ERR_WIFI_STATE) {
        // 正在连接热点时，IDF会拒绝扫描；暂停这次连接再重试扫描。
        esp_err_t disconnect_err = esp_wifi_disconnect();
        if (disconnect_err == ESP_OK) scan_reconnect_needed = true;
        for (int retry = 0; retry < 10 && err == ESP_ERR_WIFI_STATE; retry++) {
            vTaskDelay(pdMS_TO_TICKS(100));
            err = esp_wifi_scan_start(NULL, true);
        }
    }
    if (err == ESP_OK) {
        err = esp_wifi_scan_get_ap_num(&ap_count);
    }
    if (err == ESP_OK) {
        err = esp_wifi_scan_get_ap_records(&number, ap_info);
    }

    if (err == ESP_OK) {
        ESP_LOGI(TAG, "Total APs scanned = %u, actual AP number ap_info holds = %u", ap_count, number);
        if(callback)
            callback(number,ap_info);
    } else {
        ESP_LOGE(TAG, "wifi scan failed: %s", esp_err_to_name(err));
        esp_wifi_clear_ap_list();
        if (callback) callback(-1, NULL);
    }

    free(ap_info);
    bool reconnect = scan_reconnect_needed;
    scan_reconnect_needed = false;
    scan_in_progress = false;
    if (reconnect) {
        esp_err_t connect_err = esp_wifi_connect();
        if (connect_err != ESP_OK) {
            ESP_LOGW(TAG, "wifi reconnect after scan failed: %s", esp_err_to_name(connect_err));
        }
    }
    xSemaphoreGive(scan_sem);
    vTaskDelete(NULL);
}

/** 启动扫描
 * @param 无
 * @return 成功/失败
*/
esp_err_t wifi_manager_scan(p_wifi_scan_callback f)
{
    if(!scan_sem)
    {
        scan_sem = xSemaphoreCreateBinary();
        if (!scan_sem) return ESP_ERR_NO_MEM;
        xSemaphoreGive(scan_sem);
    }
    if(pdTRUE == xSemaphoreTake(scan_sem,0))
    {
        scan_reconnect_needed = false;
        scan_in_progress = true;
        //清除上次的扫描信息
        esp_wifi_clear_ap_list();
        //启动一个扫描任务
        if(pdTRUE == xTaskCreatePinnedToCore(scan_task,"scan",8192,f,3,NULL,0))
            return ESP_OK;
        scan_in_progress = false;
        xSemaphoreGive(scan_sem);
        return ESP_ERR_NO_MEM;
    }
    return ESP_ERR_INVALID_STATE;
}

/** 连接wifi
 * @param ssid
 * @param password
 * @return 成功/失败
*/
esp_err_t wifi_manager_connect(const char* ssid,const char* password)
{
    sta_connect_count = 0;
    wifi_config_t wifi_config = 
    {
        .sta = 
        {
	        .threshold.authmode = WIFI_AUTH_WPA2_PSK,   //加密方式
        },
    };
    size_t ssid_len = strlen(ssid);
    if (ssid_len > sizeof(wifi_config.sta.ssid)) ssid_len = sizeof(wifi_config.sta.ssid);
    memcpy(wifi_config.sta.ssid, ssid, ssid_len);
    snprintf((char*)wifi_config.sta.password, sizeof(wifi_config.sta.password), "%s", password);

    // 先同步停掉 WiFi，再改配置。
    // 原因：STA 处于 connecting/connected 状态下调用 esp_wifi_set_config() 会返回
    // ESP_ERR_WIFI_STATE(0x3006)；若是用 esp_wifi_disconnect() 来断开，它是异步的，
    // 返回后状态仍是 connecting，紧接着 set_config 一样会失败并 abort 重启。
    // esp_wifi_stop() 是同步接口，返回后一定处于停止态，此时设置配置才合法。
    esp_err_t err = esp_wifi_stop();
    if (err != ESP_OK && err != ESP_ERR_WIFI_NOT_STARTED) {
        ESP_LOGW(TAG, "esp_wifi_stop failed: %s", esp_err_to_name(err));
    }

    err = esp_wifi_set_mode(WIFI_MODE_STA);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "esp_wifi_set_mode failed: %s", esp_err_to_name(err));
        return err;
    }

    // 这里不用 ESP_ERROR_CHECK：失败只记录并返回错误码，避免直接 abort 重启
    err = esp_wifi_set_config(WIFI_IF_STA, &wifi_config);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "esp_wifi_set_config failed: %s", esp_err_to_name(err));
        return err;
    }

    // 启动后 WIFI_EVENT_STA_START 回调里会自动 esp_wifi_connect()
    err = esp_wifi_start();
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "esp_wifi_start failed: %s", esp_err_to_name(err));
        return err;
    }
    return ESP_OK;
}

/** 是否已经连接了路由器
 * @param 无
 * @return 是/否
*/
bool wifi_manager_is_connect(void)
{
    if (!is_sta_connected || esp_netif_sta == NULL) {
        return false;
    }

    // 事件回调可能晚于实际掉线；直接确认 STA 当前仍关联热点且持有 IP。
    wifi_ap_record_t ap_info;
    esp_netif_ip_info_t ip_info;
    return esp_wifi_sta_get_ap_info(&ap_info) == ESP_OK &&
           esp_netif_get_ip_info(esp_netif_sta, &ip_info) == ESP_OK &&
           ip_info.ip.addr != 0;
}
