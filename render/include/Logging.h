#pragma once
#include "Config.h"

namespace vkr
{

enum class LogLevel
{
    Info,
    Warning,
    Error
};

void Log(LogLevel level, const char* formatString, ...);

}

#if VKR_DEBUG
    #define DBG_LOG(msg, ...) vkr::Log(vkr::LogLevel::Info, msg, __VA_ARGS__)
#else
    #define DBG_LOG(msg, ...)
#endif

