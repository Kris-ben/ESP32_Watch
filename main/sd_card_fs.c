/**
 * @file sd_card_fs.c
 * @brief SD卡文件系统模块实现
 *
 * 使用 ESP-IDF 的 VFS FAT + SDSPI 驱动挂载SD卡，
 * 引脚配置来自 sd_spi_config.h。
 */

#include "sd_card_fs.h"
#include "sd_spi_config.h"
#include "esp_vfs_fat.h"
#include "sdmmc_cmd.h"
#include "esp_log.h"
#include <dirent.h>
#include <string.h>
#include <ctype.h>
#include <sys/stat.h>

static const char *TAG = "SD_FS";
static sdmmc_card_t *s_card = NULL;
static bool s_mounted = false;

esp_err_t sd_card_fs_mount(void)
{
    if (s_mounted) {
        ESP_LOGW(TAG, "SD卡已挂载");
        return ESP_OK;
    }

    ESP_LOGI(TAG, "初始化SD卡 (MISO=%d MOSI=%d CLK=%d CS=%d)",
             SD_PIN_MISO, SD_PIN_MOSI, SD_PIN_CLK, SD_PIN_CS);

    /* --- SPI总线初始化 --- */
    spi_bus_config_t bus_cfg = {
        .mosi_io_num = SD_PIN_MOSI,
        .miso_io_num = SD_PIN_MISO,
        .sclk_io_num = SD_PIN_CLK,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .max_transfer_sz = 4096,
    };

    esp_err_t ret = spi_bus_initialize(SD_SPI_HOST, &bus_cfg, SD_SPI_DMA_CHAN);
    if (ret != ESP_OK && ret != ESP_ERR_INVALID_STATE) {
        /* ESP_ERR_INVALID_STATE 表示总线已初始化，可继续 */
        ESP_LOGE(TAG, "SPI总线初始化失败: %s", esp_err_to_name(ret));
        return ret;
    }

    /* --- FATFS挂载 --- */
    esp_vfs_fat_sdmmc_mount_config_t mount_config = {
        .format_if_mount_failed = true,   // 无文件系统时自动格式化为FAT32
        .max_files = 5,
        .allocation_unit_size = 16 * 1024,
    };

    sdspi_device_config_t slot_config = SDSPI_DEVICE_CONFIG_DEFAULT();
    slot_config.gpio_cs = SD_PIN_CS;
    slot_config.host_id = SD_SPI_HOST;

    sdmmc_host_t host = SDSPI_HOST_DEFAULT();
    host.slot = SD_SPI_HOST;

    ret = esp_vfs_fat_sdspi_mount(SD_MOUNT_POINT, &host, &slot_config,
                                  &mount_config, &s_card);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "SD卡挂载失败: %s", esp_err_to_name(ret));
        if (ret == ESP_FAIL) {
            ESP_LOGE(TAG, "  无法识别文件系统，请确认SD卡已格式化为FAT32");
        }
        return ret;
    }

    s_mounted = true;

    /* 打印SD卡信息 */
    sdmmc_card_print_info(stdout, s_card);

    ESP_LOGI(TAG, "SD卡挂载成功: %s", SD_MOUNT_POINT);
    return ESP_OK;
}

static bool is_music_file(const char *name)
{
    if (!name) return false;
    const char *ext = strrchr(name, '.');
    if (!ext || *(ext + 1) == '\0') return false;
    ext++;

    char lower_ext[8] = {0};
    size_t len = strlen(ext);
    if (len >= sizeof(lower_ext)) len = sizeof(lower_ext) - 1;
    for (size_t i = 0; i < len; i++) {
        lower_ext[i] = (char)tolower((unsigned char)ext[i]);
    }

    return strcmp(lower_ext, "mfl") == 0 || strcmp(lower_ext, "mflac") == 0 ||
           strcmp(lower_ext, "flac") == 0 ||
           strcmp(lower_ext, "mgg") == 0 || strcmp(lower_ext, "ogg") == 0;
}

int sd_card_fs_get_music_files(char names[][SD_MUSIC_NAME_MAX], int max_count)
{
    if (!s_mounted || !names || max_count <= 0) {
        return 0;
    }

    DIR *dir = opendir(SD_MOUNT_POINT);
    if (!dir) {
        ESP_LOGE(TAG, "无法打开目录: %s", SD_MOUNT_POINT);
        return 0;
    }

    int count = 0;
    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL && count < max_count) {
        char full_path[512];
        snprintf(full_path, sizeof(full_path), "%s/%s", SD_MOUNT_POINT, entry->d_name);

        struct stat st;
        if (stat(full_path, &st) != 0 || !S_ISREG(st.st_mode)) {
            continue;
        }

        if (!is_music_file(entry->d_name)) {
            continue;
        }

        // 文件名用于后续打开音频；不能截断后继续显示，否则点歌会找不到文件。
        if (strlen(entry->d_name) >= SD_MUSIC_NAME_MAX) {
            ESP_LOGW(TAG, "跳过过长文件名");
            continue;
        }
        strcpy(names[count], entry->d_name);
        count++;
    }

    closedir(dir);
    return count;
}

void sd_card_fs_list_all(const char *path, int depth)
{
    if (!s_mounted) {
        ESP_LOGW(TAG, "SD卡未挂载");
        return;
    }
    if (depth > 5) return;  /* 防止无限递归 */

    const char *dir_path = path ? path : SD_MOUNT_POINT;
    DIR *dir = opendir(dir_path);
    if (!dir) return;

    /* 生成缩进 */
    char indent[32] = "";
    for (int i = 0; i < depth && i < 15; i++) {
        strcat(indent, "  ");
    }

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        char full_path[512];
        snprintf(full_path, sizeof(full_path), "%s/%s", dir_path, entry->d_name);

        struct stat st;
        if (stat(full_path, &st) == 0) {
            if (S_ISDIR(st.st_mode)) {
                ESP_LOGI(TAG, "%s[DIR]  %s/", indent, entry->d_name);
                sd_card_fs_list_all(full_path, depth + 1);
            } else {
                ESP_LOGI(TAG, "%s[FILE] %s  (%ld bytes)", indent, entry->d_name, (long)st.st_size);
            }
        }
    }

    closedir(dir);
}
