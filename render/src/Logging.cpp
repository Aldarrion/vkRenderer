#include "Logging.h"

#include <stdio.h>
#include <stdarg.h>

#include "vkr_Assert.h"
#include "vkr_Windows.h"

namespace vkr
{

//------------------------------------------------------------------------------
void Log(LogLevel level, const char* formatString, ...)
{
    va_list args;
    va_start(args, formatString);

    constexpr size_t buffSize = 2 << 13;
    char buffer[buffSize];

    const char* prefix = nullptr;

    switch (level)
    {
        case LogLevel::Info:
            prefix = "Info:    ";
            break;
        case LogLevel::Warning:
            prefix = "Warning: ";
            break;
        case LogLevel::Error:
            prefix = "Error:   ";
            break;
    }

    snprintf(buffer, buffSize - 1, prefix);

    const size_t prefixSize = strlen(prefix);
    const int len = vsnprintf(buffer + prefixSize, buffSize - 1 - prefixSize, formatString, args);
    vkr_assert(len >= 0 && "Logging failed, check the format string or exceeded length.");

    buffer[prefixSize + len] = '\n';
    buffer[prefixSize + len + 1] = '\0';

    OutputDebugStringA(buffer);
}

}
