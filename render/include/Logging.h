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