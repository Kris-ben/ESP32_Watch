@echo off
REM ESP32-S3 智能手表 - 编译和烧录
REM 使用方法：在 ESP-IDF 命令提示符中运行此脚本

echo ========================================
echo ESP32-S3 智能手表 - 编译和烧录
echo ========================================
echo.

echo [1/3] 清理旧的构建文件...
idf.py fullclean
if %errorlevel% neq 0 (
    echo 清理失败！
    pause
    exit /b 1
)

echo.
echo [2/3] 编译项目...
idf.py build
if %errorlevel% neq 0 (
    echo 编译失败！
    pause
    exit /b 1
)

echo.
echo [3/3] 烧录到设备...
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
