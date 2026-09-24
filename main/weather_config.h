#pragma once

// 本机私有配置可覆盖天气服务密钥，公开仓库只保留空值。
#if defined(__has_include)
#if __has_include("weather_config_user.h")
#include "weather_config_user.h"
#endif
#endif

#ifndef WEATHER_PRIVATE_KEY
#define WEATHER_PRIVATE_KEY ""
#endif

#ifndef AMAP_API_KEY
#define AMAP_API_KEY ""
#endif
