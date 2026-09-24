#include "esp_http_client.h"
#include <string.h>
#include "cjson.h"
#include "weather.h"
#include "weather_config.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "wifi_manager.h"
#include <ctype.h>
#include "ui/custom/custom.h"
#define  TAG  "Weather"

#define WEATHER_BUFF_LEN 2048

extern lv_ui guider_ui;
extern SemaphoreHandle_t lvgl_mutex;

//地理位置
static char city_name[48] = {0};
static char amap_city_code[8] = {0};

// 缓存的天气数据（供界面创建后使用）
static weather_data_pkt_t g_weather_data[3] = {0};
static bool g_weather_data_valid = false;
static int g_weather_data_count = 0;

// URL编码函数
static void url_encode(const char *src, char *dst, size_t dst_size)
{
    const char *hex = "0123456789ABCDEF";
    size_t dst_idx = 0;
    
    for (size_t i = 0; src[i] != '\0' && dst_idx < dst_size - 1; i++)
    {
        unsigned char c = (unsigned char)src[i];
        
        // 保留字母、数字、-、_、.、~
        if (isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~')
        {
            dst[dst_idx++] = c;
        }
        else
        {
            // 其他字符进行百分号编码
            if (dst_idx + 3 < dst_size)
            {
                dst[dst_idx++] = '%';
                dst[dst_idx++] = hex[c >> 4];
                dst[dst_idx++] = hex[c & 0x0F];
            }
            else
            {
                break;
            }
        }
    }
    dst[dst_idx] = '\0';
}

static uint8_t weather_data_buff[WEATHER_BUFF_LEN] = {0};

//http接收到的数据长度
static int weather_data_size = 0;
static esp_err_t http_client_event_handler(esp_http_client_event_t *evt)
{
    switch(evt->event_id) {
        case HTTP_EVENT_ERROR:    //错误事件
            //ESP_LOGI(TAG, "HTTP_EVENT_ERROR");
            break;
        case HTTP_EVENT_ON_CONNECTED:    //连接成功事件
            //ESP_LOGI(TAG, "HTTP_EVENT_ON_CONNECTED");
            break;
        case HTTP_EVENT_HEADER_SENT:    //发送头事件
            //ESP_LOGI(TAG, "HTTP_EVENT_HEADER_SENT");
            break;
        case HTTP_EVENT_ON_HEADER:    //接收头事件
            //ESP_LOGI(TAG, "HTTP_EVENT_ON_HEADER");
            printf("%.*s", evt->data_len, (char*)evt->data);
            break;
        case HTTP_EVENT_ON_DATA:    //接收数据事件
            {
                size_t copy_len = 0;
                ESP_LOGI(TAG, "HTTP_EVENT_ON_DATA, len=%d", evt->data_len);
                printf("HTTP_EVENT_ON_DATA data=%.*s\r\n", evt->data_len,(char*)evt->data);
                if(evt->data_len > WEATHER_BUFF_LEN - 1 - weather_data_size)
                {
                    copy_len = WEATHER_BUFF_LEN - 1 - weather_data_size;
                }
                else
                {
                    copy_len = evt->data_len;
                }
                memcpy(&weather_data_buff[weather_data_size],evt->data,copy_len);
                weather_data_size += copy_len;
                weather_data_buff[weather_data_size] = '\0';
            }
            break;
        case HTTP_EVENT_ON_FINISH:    //会话完成事件
            weather_data_size = 0;
            break;
        case HTTP_EVENT_DISCONNECTED:    //断开事件
            //ESP_LOGI(TAG, "HTTP_EVENT_DISCONNECTED");
            weather_data_size = 0;
            break;
        case HTTP_EVENT_REDIRECT:
            //ESP_LOGI(TAG, "HTTP_EVENT_REDIRECT");
            break;
    }
    return ESP_OK;
}

// 设置今日天气图片和温度
static void set_today_img(lv_ui *ui, const char *img_path, int low_temp, int high_temp, const char *text_day)
{
    if (!ui || !ui->screen_weather) return;
    
    if (ui->screen_weather_img_today && lv_obj_is_valid(ui->screen_weather_img_today)) {
        lv_image_set_src(ui->screen_weather_img_today, img_path);
    }
    if (ui->screen_weather_label_today_temp && lv_obj_is_valid(ui->screen_weather_label_today_temp)) {
        lv_label_set_text_fmt(ui->screen_weather_label_today_temp, "%d-%d℃", low_temp, high_temp);
    }
    if (ui->screen_weather_label_today1 && lv_obj_is_valid(ui->screen_weather_label_today1)) {
        lv_label_set_text(ui->screen_weather_label_today1, text_day);
    }
}

// 设置明日天气图片和温度
static void set_tomorrow_img(lv_ui *ui, const char *img_path, int low_temp, int high_temp, const char *text_day)
{
    if (!ui || !ui->screen_weather) return;
    
    if (ui->screen_weather_img_tomorrow && lv_obj_is_valid(ui->screen_weather_img_tomorrow)) {
        lv_image_set_src(ui->screen_weather_img_tomorrow, img_path);
    }
    if (ui->screen_weather_label_tomorrow_temp && lv_obj_is_valid(ui->screen_weather_label_tomorrow_temp)) {
        lv_label_set_text_fmt(ui->screen_weather_label_tomorrow_temp, "%d-%d℃", low_temp, high_temp);
    }
    if (ui->screen_weather_label_tomorrow2 && lv_obj_is_valid(ui->screen_weather_label_tomorrow2)) {
        lv_label_set_text(ui->screen_weather_label_tomorrow2, text_day);
    }
}

// 设置后日天气图片和温度
static void set_after_img(lv_ui *ui, const char *img_path, int low_temp, int high_temp, const char *text_day)
{
    if (!ui || !ui->screen_weather) return;
    
    if (ui->screen_weather_img_later && lv_obj_is_valid(ui->screen_weather_img_later)) {
        lv_image_set_src(ui->screen_weather_img_later, img_path);
    }
    if (ui->screen_weather_label_later_temp && lv_obj_is_valid(ui->screen_weather_label_later_temp)) {
        lv_label_set_text_fmt(ui->screen_weather_label_later_temp, "%d-%d℃", low_temp, high_temp);
    }
    if (ui->screen_weather_label_later2 && lv_obj_is_valid(ui->screen_weather_label_later2)) {
        lv_label_set_text(ui->screen_weather_label_later2, text_day);
    }
}

// 设置天气日期
static void set_weather_date(lv_ui *ui, const char *date)
{
    if (!ui || !ui->screen_weather) return;
    
    if (ui->screen_weather_datetext_1 && lv_obj_is_valid(ui->screen_weather_datetext_1)) {
        lv_label_set_text(ui->screen_weather_datetext_1, date);
    }
}

// 设置天气城市名
static void set_weather_city(lv_ui *ui, const char *city)
{
    if (!ui || !ui->screen_weather) return;
    
    if (ui->screen_weather_label_city && lv_obj_is_valid(ui->screen_weather_label_city)) {
        lv_label_set_text(ui->screen_weather_label_city, city);
    }
}

static void clear_weather_day(lv_obj_t *image, lv_obj_t *temp, lv_obj_t *description)
{
    if (image && lv_obj_is_valid(image)) lv_obj_add_flag(image, LV_OBJ_FLAG_HIDDEN);
    if (temp && lv_obj_is_valid(temp)) lv_label_set_text(temp, "--℃");
    if (description && lv_obj_is_valid(description)) lv_label_set_text(description, "--");
}

void weather_refresh_screen(lv_ui *ui)
{
    if (!ui || !ui->screen_weather || !lv_obj_is_valid(ui->screen_weather)) return;

    set_weather_city(ui, city_name[0] ? city_name : "定位中");
    set_weather_date(ui, g_weather_data_valid ? g_weather_data[0].date : "--/--/--");

    lv_obj_t *images[3] = {ui->screen_weather_img_today, ui->screen_weather_img_tomorrow,
                           ui->screen_weather_img_later};
    lv_obj_t *temps[3] = {ui->screen_weather_label_today_temp, ui->screen_weather_label_tomorrow_temp,
                          ui->screen_weather_label_later_temp};
    lv_obj_t *descriptions[3] = {ui->screen_weather_label_today1, ui->screen_weather_label_tomorrow2,
                                 ui->screen_weather_label_later2};
    for (int i = 0; i < 3; ++i) {
        if (!g_weather_data_valid || i >= g_weather_data_count) {
            clear_weather_day(images[i], temps[i], descriptions[i]);
            continue;
        }
        char img_path[32];
        snprintf(img_path, sizeof(img_path), "/img/%s@1x.png", g_weather_data[i].code);
        if (images[i] && lv_obj_is_valid(images[i])) lv_obj_remove_flag(images[i], LV_OBJ_FLAG_HIDDEN);
        if (i == 0) {
            set_today_img(ui, img_path, g_weather_data[i].low_temp, g_weather_data[i].high_temp,
                          g_weather_data[i].text_day);
        } else if (i == 1) {
            set_tomorrow_img(ui, img_path, g_weather_data[i].low_temp, g_weather_data[i].high_temp,
                             g_weather_data[i].text_day);
        } else {
            set_after_img(ui, img_path, g_weather_data[i].low_temp, g_weather_data[i].high_temp,
                          g_weather_data[i].text_day);
        }
    }
}

//解析天气数据 JSON格式
static esp_err_t parse_weather(char *weather_data)
{
    cJSON *wt_js = cJSON_Parse(weather_data);
    if(!wt_js)
    {
        ESP_LOGE(TAG,"parse weather data error");
        return ESP_FAIL;
    }
    cJSON *result_js = cJSON_GetObjectItem(wt_js,"results");
    if(!result_js)
    {
        ESP_LOGE(TAG,"get results object error");
        cJSON_Delete(wt_js);
        return ESP_FAIL;
    }
    cJSON *result_child_js = result_js->child;
    if(!result_child_js)
    {
        ESP_LOGE(TAG,"results child is null");
        cJSON_Delete(wt_js);
        return ESP_FAIL;
    }
    
    cJSON *daily_js = cJSON_GetObjectItem(result_child_js,"daily");
    if(!daily_js)
    {
        ESP_LOGE(TAG,"get daily object error");
        cJSON_Delete(wt_js);
        return ESP_FAIL;
    }
    
    weather_data_pkt_t data[3];
    memset(data, 0, sizeof(data));
    int index = 0;
    cJSON *daily_child_js = daily_js->child;
    while(daily_child_js && index < 3)
    {
        cJSON *high_js = cJSON_GetObjectItem(daily_child_js,"high");
        cJSON *low_js = cJSON_GetObjectItem(daily_child_js,"low");
        cJSON *code_js = cJSON_GetObjectItem(daily_child_js,"code_day");
        cJSON *text_day_js = cJSON_GetObjectItem(daily_child_js,"text_day");
        cJSON *date_js = cJSON_GetObjectItem(daily_child_js,"date");
        
        // 检查所有字段是否存在
        if(high_js && low_js && code_js)
        {
            const char *high_str = cJSON_GetStringValue(high_js);
            const char *low_str = cJSON_GetStringValue(low_js);
            const char *code_str = cJSON_GetStringValue(code_js);
            const char *text_day_str = text_day_js ? cJSON_GetStringValue(text_day_js) : "";
            const char *date_str = date_js ? cJSON_GetStringValue(date_js) : "";
            
            if(high_str && low_str && code_str)
            {
                sscanf(high_str,"%d",&data[index].high_temp);
                sscanf(low_str,"%d",&data[index].low_temp);
                snprintf(data[index].code,sizeof(data[index].code),"%s",code_str);
                snprintf(data[index].text_day,sizeof(data[index].text_day),"%s",text_day_str ? text_day_str : "");
                snprintf(data[index].date,sizeof(data[index].date),"%s",date_str ? date_str : "");

                ESP_LOGI(TAG,"day[%d]->date:%s,text:%s,high:%d,low:%d,code:%s",
                         index, data[index].date, data[index].text_day,
                         data[index].high_temp, data[index].low_temp, data[index].code);
                index++;
            }
            else
            {
                ESP_LOGW(TAG,"day[%d] has null string values", index);
            }
        }
        else
        {
            ESP_LOGW(TAG,"day[%d] missing required fields", index);
        }
        
        daily_child_js = daily_child_js->next;
    }
    
    if(index == 0)
    {
        ESP_LOGE(TAG,"No valid weather data parsed");
        cJSON_Delete(wt_js);
        return ESP_FAIL;
    }
    
    if (lvgl_mutex && xSemaphoreTake(lvgl_mutex, portMAX_DELAY) == pdTRUE) {
        memcpy(g_weather_data, data, sizeof(g_weather_data));
        g_weather_data_count = index;
        g_weather_data_valid = true;
        weather_refresh_screen(&guider_ui);
        xSemaphoreGive(lvgl_mutex);
    }
    ESP_LOGI(TAG, "天气数据已缓存");
    
    //数据设置到显示界面
    ESP_LOGI(TAG,"Successfully parsed %d days of weather data", index);
    
    cJSON_Delete(wt_js);
    return ESP_OK;
}

// ========== 高德地图天气API实现 ==========


// 缓存高德实时天气数据
static amap_realtime_weather_t g_amap_weather = {0};
static bool g_amap_weather_valid = false;

// 解析高德天气数据
static esp_err_t parse_amap_weather(char *weather_data)
{
    cJSON *root = cJSON_Parse(weather_data);
    if(!root)
    {
        ESP_LOGE(TAG, "parse amap weather data error");
        return ESP_FAIL;
    }
    
    // 检查status
    cJSON *status = cJSON_GetObjectItem(root, "status");
    if(!status || strcmp(cJSON_GetStringValue(status), "1") != 0)
    {
        ESP_LOGE(TAG, "amap API status error");
        cJSON_Delete(root);
        return ESP_FAIL;
    }
    
    // 获取lives数组
    cJSON *lives = cJSON_GetObjectItem(root, "lives");
    if(!lives || !cJSON_IsArray(lives) || cJSON_GetArraySize(lives) == 0)
    {
        ESP_LOGE(TAG, "amap lives array error");
        cJSON_Delete(root);
        return ESP_FAIL;
    }
    
    cJSON *live = cJSON_GetArrayItem(lives, 0);
    if(!live)
    {
        ESP_LOGE(TAG, "amap live data error");
        cJSON_Delete(root);
        return ESP_FAIL;
    }
    
    // 解析温度
    cJSON *temperature = cJSON_GetObjectItem(live, "temperature");
    if(temperature && cJSON_GetStringValue(temperature))
    {
        g_amap_weather.temperature = atoi(cJSON_GetStringValue(temperature));
    }
    
    // 解析湿度
    cJSON *humidity = cJSON_GetObjectItem(live, "humidity");
    if(humidity && cJSON_GetStringValue(humidity))
    {
        g_amap_weather.humidity = atoi(cJSON_GetStringValue(humidity));
    }
    
    // 解析天气现象
    cJSON *weather = cJSON_GetObjectItem(live, "weather");
    if(weather && cJSON_GetStringValue(weather))
    {
        snprintf(g_amap_weather.weather, sizeof(g_amap_weather.weather), "%s", cJSON_GetStringValue(weather));
    }
    
    // 解析城市名
    cJSON *city = cJSON_GetObjectItem(live, "city");
    if(city && cJSON_GetStringValue(city))
    {
        snprintf(g_amap_weather.city, sizeof(g_amap_weather.city), "%s", cJSON_GetStringValue(city));
    }
    
    g_amap_weather_valid = true;
    
    ESP_LOGI(TAG, "高德天气: 城市=%s, 天气=%s, 温度=%d℃, 湿度=%d%%", 
             g_amap_weather.city, g_amap_weather.weather, 
             g_amap_weather.temperature, g_amap_weather.humidity);
    
    // 更新主界面
    update_home_temp_humidity(&guider_ui);
    
    cJSON_Delete(root);
    return ESP_OK;
}

/**
 * @brief 读取最近一次获取到的实时天气数据
 *
 * 供语音助手等模块直接播报，避免为了问一句天气再去调用大模型。
 * @param out 输出参数
 * @return true 数据有效；false 表示尚未成功获取过
 */
bool weather_get_realtime(amap_realtime_weather_t *out)
{
    if (out == NULL || !g_amap_weather_valid) {
        return false;
    }
    *out = g_amap_weather;
    return true;
}

// 获取高德实时天气
esp_err_t amap_get_realtime_weather(void)
{
    if (amap_city_code[0] == '\0') {
        ESP_LOGW(TAG, "没有定位得到城市编码，跳过实时天气，避免显示其他城市数据");
        return ESP_ERR_INVALID_STATE;
    }
    static char url[256];
    snprintf(url, sizeof(url), 
             "http://restapi.amap.com/v3/weather/weatherInfo?key=%s&city=%s&extensions=base",
             AMAP_API_KEY, amap_city_code);
    
    ESP_LOGI(TAG, "Amap Weather API URL: %s", url);
    
    esp_http_client_config_t config = {
        .url = url,
        .event_handler = http_client_event_handler,
        .timeout_ms = 15000,
        .user_agent = "Mozilla/5.0 (ESP32)",
        .buffer_size = 2048,
        .buffer_size_tx = 1024,
    };
    
    esp_http_client_handle_t client = esp_http_client_init(&config);
    if(client == NULL)
    {
        ESP_LOGE(TAG, "Failed to initialize HTTP client for amap");
        return ESP_FAIL;
    }
    
    memset(weather_data_buff, 0, WEATHER_BUFF_LEN);
    weather_data_size = 0;
    esp_err_t err = esp_http_client_perform(client);
    
    if(err == ESP_OK)
    {
        int status_code = esp_http_client_get_status_code(client);
        ESP_LOGI(TAG, "Amap HTTP Status Code: %d", status_code);
        if(status_code == 200)
        {
            err = parse_amap_weather((char*)weather_data_buff);
        }
        else
        {
            ESP_LOGE(TAG, "Amap HTTP request failed with status code: %d", status_code);
            err = ESP_FAIL;
        }
    }
    else
    {
        ESP_LOGE(TAG, "Amap HTTP perform failed: %s", esp_err_to_name(err));
    }
    
    esp_http_client_cleanup(client);
    return err;
}

// 内部函数：更新主界面温湿度显示（不获取mutex，调用者需确保已持有mutex或在LVGL上下文中）
static void update_home_temp_humidity_internal(lv_ui *ui)
{
    if(!ui || !g_amap_weather_valid)
    {
        return;
    }
    
    // 检查主界面是否存在
    if(ui->screen_home && lv_obj_is_valid(ui->screen_home))
    {
        char temp_str[16];
        char humidity_str[16];
        
        // 更新温度arc和label
        if(ui->screen_home_arc_temp && lv_obj_is_valid(ui->screen_home_arc_temp))
        {
            // 温度范围限制在0-100
            int temp_val = g_amap_weather.temperature;
            if(temp_val < 0) temp_val = 0;
            if(temp_val > 100) temp_val = 100;
            lv_arc_set_value(ui->screen_home_arc_temp, temp_val);
        }
        
        if(ui->screen_home_label_te && lv_obj_is_valid(ui->screen_home_label_te))
        {
            snprintf(temp_str, sizeof(temp_str), "%d℃", g_amap_weather.temperature);
            lv_label_set_text(ui->screen_home_label_te, temp_str);
        }
        
        // 更新湿度arc和label
        if(ui->screen_home_arc_humidity && lv_obj_is_valid(ui->screen_home_arc_humidity))
        {
            // 湿度范围限制在0-100
            int humidity_val = g_amap_weather.humidity;
            if(humidity_val < 0) humidity_val = 0;
            if(humidity_val > 100) humidity_val = 100;
            lv_arc_set_value(ui->screen_home_arc_humidity, humidity_val);
        }
        
        if(ui->screen_home_label_him && lv_obj_is_valid(ui->screen_home_label_him))
        {
            snprintf(humidity_str, sizeof(humidity_str), "%d%%", g_amap_weather.humidity);
            lv_label_set_text(ui->screen_home_label_him, humidity_str);
        }
        
        ESP_LOGI(TAG, "主界面温湿度已更新: 温度=%d℃, 湿度=%d%%", 
                 g_amap_weather.temperature, g_amap_weather.humidity);
    }
}

// 更新主界面温湿度显示（外部调用，会获取mutex）
void update_home_temp_humidity(lv_ui *ui)
{
    if(!ui || !g_amap_weather_valid)
    {
        // 数据未就绪时静默返回，不打印警告（避免刷屏）
        return;
    }
    
    if(!lvgl_mutex)
    {
        ESP_LOGW(TAG, "update_home_temp_humidity: lvgl_mutex未初始化");
        return;
    }
    
    if(xSemaphoreTake(lvgl_mutex, pdMS_TO_TICKS(200)) == pdTRUE)
    {
        update_home_temp_humidity_internal(ui);
        xSemaphoreGive(lvgl_mutex);
    }
}

static esp_err_t weather_http_connect()
{
    // 检查城市名称是否有效
    if(strlen(city_name) == 0)
    {
        ESP_LOGE(TAG, "city_name is empty, cannot fetch weather");
        return ESP_FAIL;
    }
    
    // URL编码城市名称
    char encoded_city[128] = {0};
    url_encode(city_name, encoded_city, sizeof(encoded_city));
    
    static char url[512];
    snprintf(url,sizeof(url),"http://api.seniverse.com/v3/weather/daily.json?key=%s&location=%s&language=zh-Hans&unit=c&start=0&days=3"
    ,WEATHER_PRIVATE_KEY, encoded_city);
    
    ESP_LOGI(TAG, "Weather API URL: %s", url);
    
    esp_http_client_config_t config =
    {
        .url = url,
        .event_handler = http_client_event_handler,
        .timeout_ms = 15000,  // 增加超时时间到15秒
        .user_agent = "Mozilla/5.0 (ESP32)",  // 添加User-Agent
        .buffer_size = 2048,  // 增加缓冲区大小
        .buffer_size_tx = 1024,
    };
    //初始化结构体
    esp_http_client_handle_t http_client = esp_http_client_init(&config);	//初始化http连接
    if(http_client == NULL)
    {
        ESP_LOGE(TAG, "Failed to initialize HTTP client");
        return ESP_FAIL;
    }
    
    memset(weather_data_buff,0,WEATHER_BUFF_LEN);
    weather_data_size = 0;
    esp_err_t err  = esp_http_client_perform(http_client);
    
    if(err == ESP_OK)
    {
        int status_code = esp_http_client_get_status_code(http_client);
        ESP_LOGI(TAG, "HTTP Status Code: %d", status_code);
        if(status_code == 200)
        {
            err = parse_weather((char*)weather_data_buff);
        }
        else
        {
            ESP_LOGE(TAG, "HTTP request failed with status code: %d", status_code);
            err = ESP_FAIL;
        }
    }
    else
    {
        ESP_LOGE(TAG, "HTTP perform failed: %s", esp_err_to_name(err));
    }
    
    esp_http_client_cleanup(http_client);
    return err;
}
//解析IP地址 JSON格式 (pconline格式)
static esp_err_t parse_location(char *location_data)
{
    cJSON *location_js = cJSON_Parse(location_data);
    if(!location_js)
    {
        ESP_LOGE(TAG,"parse location data error");
        return ESP_FAIL;
    }
    cJSON *city_js = cJSON_GetObjectItem(location_js,"city");
    const char *city = cJSON_GetStringValue(city_js);
    if(!city || !city[0])
    {
        ESP_LOGE(TAG,"get city object error");
        cJSON_Delete(location_js);
        return ESP_FAIL;
    }
    snprintf(city_name,sizeof(city_name),"%s",city);
    ESP_LOGI(TAG,"location city:%s",city_name);
    cJSON_Delete(location_js);
    return ESP_OK;
}

//解析IP地址 JSON格式 (ip-api.com格式)
static esp_err_t parse_location_ipapi(char *location_data)
{
    cJSON *location_js = cJSON_Parse(location_data);
    if(!location_js)
    {
        ESP_LOGE(TAG,"parse location data error");
        return ESP_FAIL;
    }
    cJSON *city_js = cJSON_GetObjectItem(location_js,"city");
    cJSON *status_js = cJSON_GetObjectItem(location_js, "status");
    const char *city = cJSON_GetStringValue(city_js);
    if(!city || !city[0] || !cJSON_IsString(status_js) ||
       strcmp(cJSON_GetStringValue(status_js), "success") != 0)
    {
        ESP_LOGE(TAG,"ip-api 定位失败或城市为空");
        cJSON_Delete(location_js);
        return ESP_FAIL;
    }
    snprintf(city_name,sizeof(city_name),"%s",city);
    ESP_LOGI(TAG,"location city (from ip-api):%s",city_name);
    cJSON_Delete(location_js);
    return ESP_OK;
}

/* 高德 IP 定位同时返回城市名和天气接口要求的 adcode。 */
static esp_err_t location_http_connect_amap(void)
{
    char url[160];
    snprintf(url, sizeof(url), "http://restapi.amap.com/v3/ip?key=%s", AMAP_API_KEY);
    esp_http_client_config_t config = {
        .url = url,
        .event_handler = http_client_event_handler,
        .timeout_ms = 15000,
        .buffer_size = 2048,
    };
    esp_http_client_handle_t client = esp_http_client_init(&config);
    if (!client) return ESP_FAIL;

    memset(weather_data_buff, 0, WEATHER_BUFF_LEN);
    weather_data_size = 0;
    esp_err_t err = esp_http_client_perform(client);
    int status_code = esp_http_client_get_status_code(client);
    if (err == ESP_OK && status_code == 200) {
        cJSON *root = cJSON_Parse((char *)weather_data_buff);
        const char *status = root ? cJSON_GetStringValue(cJSON_GetObjectItem(root, "status")) : NULL;
        const char *city = root ? cJSON_GetStringValue(cJSON_GetObjectItem(root, "city")) : NULL;
        const char *adcode = root ? cJSON_GetStringValue(cJSON_GetObjectItem(root, "adcode")) : NULL;
        bool code_valid = adcode && strlen(adcode) == 6;
        for (size_t i = 0; code_valid && i < 6; ++i) {
            if (!isdigit((unsigned char)adcode[i])) code_valid = false;
        }
        if (status && strcmp(status, "1") == 0 && city && city[0] && code_valid) {
            snprintf(city_name, sizeof(city_name), "%s", city);
            snprintf(amap_city_code, sizeof(amap_city_code), "%s", adcode);
            ESP_LOGI(TAG, "高德 IP 定位: %s (%s)", city_name, amap_city_code);
        } else {
            ESP_LOGW(TAG, "高德 IP 定位未返回有效城市和编码");
            err = ESP_FAIL;
        }
        cJSON_Delete(root);
    } else {
        ESP_LOGW(TAG, "高德 IP 定位请求失败: %s, HTTP %d", esp_err_to_name(err), status_code);
        err = ESP_FAIL;
    }
    esp_http_client_cleanup(client);
    return err;
}

static esp_err_t location_http_connect()
{
    static char url[256];
    snprintf(url,sizeof(url),"http://whois.pconline.com.cn/ipJson.jsp?json=true");
    
    ESP_LOGI(TAG, "Location API URL: %s", url);
    
    esp_http_client_config_t config =
    {
        .url = url,
        .event_handler = http_client_event_handler,
        .timeout_ms = 15000,  // 增加超时时间到15秒
        .user_agent = "Mozilla/5.0 (ESP32)",  // 添加User-Agent
        .buffer_size = 2048,  // 增加缓冲区大小
        .buffer_size_tx = 1024,
    };
    //初始化结构体
    esp_http_client_handle_t client = esp_http_client_init(&config);	//初始化http连接
    if(client == NULL)
    {
        ESP_LOGE(TAG, "Failed to initialize HTTP client");
        return ESP_FAIL;
    }
    
    memset(weather_data_buff,0,WEATHER_BUFF_LEN);
    weather_data_size = 0;
    esp_err_t err  = esp_http_client_perform(client);
    
    if(err == ESP_OK)
    {
        int status_code = esp_http_client_get_status_code(client);
        ESP_LOGI(TAG, "HTTP Status Code: %d", status_code);
        if(status_code == 200)
        {
            err = parse_location((char*)weather_data_buff);
        }
        else
        {
            ESP_LOGE(TAG, "HTTP request failed with status code: %d", status_code);
            err = ESP_FAIL;
        }
    }
    else
    {
        ESP_LOGE(TAG, "HTTP perform failed: %s", esp_err_to_name(err));
    }
    
    esp_http_client_cleanup(client);
    return err;
}

// 备用位置API (ip-api.com)
static esp_err_t location_http_connect_backup()
{
    static char url[256];
    snprintf(url,sizeof(url),"http://ip-api.com/json/?lang=zh-CN");
    
    ESP_LOGI(TAG, "Backup Location API URL: %s", url);
    
    esp_http_client_config_t config =
    {
        .url = url,
        .event_handler = http_client_event_handler,
        .timeout_ms = 15000,
        .user_agent = "Mozilla/5.0 (ESP32)",
        .buffer_size = 2048,
        .buffer_size_tx = 1024,
    };
    
    esp_http_client_handle_t client = esp_http_client_init(&config);
    if(client == NULL)
    {
        ESP_LOGE(TAG, "Failed to initialize HTTP client");
        return ESP_FAIL;
    }
    
    memset(weather_data_buff,0,WEATHER_BUFF_LEN);
    weather_data_size = 0;
    esp_err_t err  = esp_http_client_perform(client);
    
    if(err == ESP_OK)
    {
        int status_code = esp_http_client_get_status_code(client);
        ESP_LOGI(TAG, "HTTP Status Code: %d", status_code);
        if(status_code == 200)
        {
            err = parse_location_ipapi((char*)weather_data_buff);
        }
        else
        {
            ESP_LOGE(TAG, "HTTP request failed with status code: %d", status_code);
            err = ESP_FAIL;
        }
    }
    else
    {
        ESP_LOGE(TAG, "HTTP perform failed: %s", esp_err_to_name(err));
    }
    
    esp_http_client_cleanup(client);
    return err;
}

static void weather_task(void *param)
{
    while(1)
    {
        if(!wifi_manager_is_connect())
        {
            vTaskDelay(pdMS_TO_TICKS(2000));
        }
        else
        {
            char previous_city[sizeof(city_name)];
            char previous_code[sizeof(amap_city_code)];
            snprintf(previous_city, sizeof(previous_city), "%s", city_name);
            snprintf(previous_code, sizeof(previous_code), "%s", amap_city_code);
            city_name[0] = '\0';
            amap_city_code[0] = '\0';
            // 同一定位结果供三天预报和实时天气使用，避免城市不一致。
            esp_err_t err = location_http_connect_amap();

            if (err != ESP_OK) {
                ESP_LOGW(TAG, "高德定位失败，尝试 ip-api 定位");
                err = location_http_connect_backup();
            }
            
            // 如果备用API失败，尝试pconline (注意：可能有GBK编码问题)
            if(err != ESP_OK)
            {
                ESP_LOGW(TAG, "ip-api 定位失败，尝试 pconline 定位");
                vTaskDelay(pdMS_TO_TICKS(2000)); // 等待2秒后尝试另一个API
                err = location_http_connect();
            }
            
            if(err != ESP_OK)
            {
                ESP_LOGE(TAG, "All location APIs failed, will retry later");
                snprintf(city_name, sizeof(city_name), "%s", previous_city);
                snprintf(amap_city_code, sizeof(amap_city_code), "%s", previous_code);
                vTaskDelay(pdMS_TO_TICKS(60000)); // 失败后等待60秒重试
                continue;
            }

            if (strcmp(previous_city, city_name) != 0 && lvgl_mutex &&
                xSemaphoreTake(lvgl_mutex, portMAX_DELAY) == pdTRUE) {
                g_weather_data_valid = false;
                g_weather_data_count = 0;
                g_amap_weather_valid = false;
                weather_refresh_screen(&guider_ui);
                xSemaphoreGive(lvgl_mutex);
            }
            
            // 位置获取成功后再获取天气
            ESP_LOGI(TAG, "Location obtained successfully, fetching weather...");
            err = weather_http_connect();
            if(err != ESP_OK)
            {
                ESP_LOGE(TAG, "Failed to get weather data");
            }
            else
            {
                ESP_LOGI(TAG, "Weather data fetched successfully");
            }
            
            // 获取高德实时天气（温度、湿度）
            if (amap_city_code[0]) {
                ESP_LOGI(TAG, "Fetching Amap realtime weather...");
                err = amap_get_realtime_weather();
                if(err != ESP_OK) {
                    ESP_LOGE(TAG, "Failed to get Amap weather data");
                } else {
                    ESP_LOGI(TAG, "Amap weather data fetched successfully");
                }
            }
            
            vTaskDelay(pdMS_TO_TICKS(1000*1800)); // 30分钟更新一次
        }
    }
}

void weather_start(void)
{
   xTaskCreatePinnedToCore(weather_task,"weather_task",4096,NULL,2,NULL,1);
}

