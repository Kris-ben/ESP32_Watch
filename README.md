# ESP32-S3 智能手表

基于 ESP-IDF 与 LVGL 的 1.83 英寸触屏手表固件，提供表盘、应用列表、本地音乐、Wi-Fi、天气、日历、闹钟、语音助手和无线固件更新。

> GitHub 项目描述：基于 ESP32-S3、ESP-IDF 与 LVGL 的智能手表固件，支持触屏表盘、本地音乐、语音助手、天气、日历和 OTA 更新。

## 硬件与环境

- ESP32-S3，16 MB Flash，八线 PSRAM。
- 物理屏幕为 240 × 284 的 ST7789V 显示屏和触摸屏；屏幕规格见仓库中的 `P183B001-V4-CTP.pdf`。
- 音频相关功能使用 PDM 麦克风、MAX98357A 功放和 SD 卡。实际引脚以 `components/display/include/board_display_config.h`、`main/ai_chat/ai_chat_config.h` 与 `main/sd_spi_config.h` 为准。
- 推荐 ESP-IDF 5.5.1。LVGL 及 `esp_lvgl_port` 由 `main/idf_component.yml` 和 `dependencies.lock` 管理。

## 显示方向与本次更改

- 当前固件将物理屏幕旋转 90°，画面按 **284 × 240 横屏**显示。实体屏幕、PCB 和 3D 打印外壳的摆放方向没有更换。
- 触摸坐标同步交换 X/Y 并镜像两个方向，使触摸位置与横屏控件对应。
- 原有竖屏页面在加载时转换为横屏布局；AI 动画、弹窗和快捷设置等后续更新的控件也使用横屏坐标。
- 页面首次显示前同步最近的电量和 Wi-Fi 状态；电池图标与百分比原位刷新，首页和天气页的 Wi-Fi 图标位置已重新校准。

## 快速开始

1. 安装并激活 ESP-IDF 5.5.1，克隆本仓库，然后进入项目根目录。
2. 运行 `idf.py set-target esp32s3`，再运行 `idf.py build`。`sdkconfig.defaults` 已包含本项目所需的 16 MB Flash、自定义分区表、八线 PSRAM、中文 SD 文件名及界面字体设置；首次构建后仍应按实物检查 `sdkconfig`。
3. 用 `idf.py -p <串口号> flash monitor` 烧录并查看日志。固件文件是 `build/esp32_s3_watch.bin`。

Windows 下也可在 ESP-IDF 命令行运行 `quick_build.cmd`（编译、烧录、监视）或 `build_and_flash.cmd`（先清理再编译、烧录、监视）。这两个脚本会操作当前连接的设备，使用前先确认串口。

## 配置网络与服务

1. 复制 `main/ai_chat/ai_chat_config_user.h.example` 为同目录的 `ai_chat_config_user.h`，填写自己的 Wi-Fi、大模型和百度语音参数。
2. 复制 `main/weather_config_user.h.example` 为同目录的 `weather_config_user.h`，填写自己的天气和高德地图密钥。
3. 两个 `_user.h` 文件只保存在本机，已加入 `.gitignore`。不配置服务密钥也能编译，但相应联网功能不可用。Wi-Fi 也可以通过设备界面配置。

本地音乐读取 SD 卡根目录，当前最多显示前 32 首，支持真正的 Ogg Vorbis 和 FLAC；仅改文件扩展名无法播放。电脑若无法稳定读取 SD 卡，可在手表成功挂载 SD 卡后通过 USB 串口导入 Ogg 文件：安装 Python 的 `pyserial`，执行 `python tools/send_music_serial.py COM7 歌曲1.ogg 歌曲2.ogg`（按实际端口和文件路径替换）。同名文件不会被覆盖；传输中断留下的 `.part` 文件需从 SD 卡中手动删除后重试。无线更新页面要求手机与手表在同一 Wi-Fi 下，上传本工程构建的 `.bin` 文件。

## 目录说明

| 路径 | 内容 |
| --- | --- |
| `main/` | 应用入口、音频、传感器、网络与功能模块 |
| `main/ui/` | 手表页面与主题；`generated/` 是原有生成界面 |
| `components/` | 显示和 Wi-Fi 组件 |
| `html/`、`img/` | 构建时写入 SPIFFS 分区的资源 |
| `partitions_webserver.csv` | 固件、OTA 与资源分区 |

## 已知限制

- 当前中文字体是子集，新歌曲名可能出现缺字。
- 状态栏的电池图标与百分比由电压估算；电压尚未用万用表校准，充电时读数也不能视作准确的剩余电量。
- 在线语音和天气需要自行申请服务凭据与可用网络。
