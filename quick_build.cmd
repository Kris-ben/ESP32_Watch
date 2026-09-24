@echo off
REM ESP32-S3 智能手表 - 快速编译（不清理）
REM 使用方法：在 ESP-IDF 命令提示符中运行此脚本

echo ========================================
echo ESP32-S3 智能手表 - 快速编译
echo ========================================
echo.

echo 编译项目...
idf.py build
if %errorlevel% neq 0 (
    echo 编译失败！
    pause
    exit /b 1
)

echo.
echo 烧录到设备...
idf.py flash monitor
if %errorlevel% neq 0 (
    echo 烧录失败！
    pause
    exit /b 1
)

echo.
echo ========================================
echo 完成！设备正在运行...
echo 按 Ctrl+] 退出监视器
echo ========================================
