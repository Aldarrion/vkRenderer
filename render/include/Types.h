#pragma once
#include "Config.h"

#include <cstdint>

using uint8     = uint8_t;
using uint16    = uint16_t;
using uint      = uint32_t;
using uint64    = uint64_t;

namespace internal
{

template<class T, uint N>
constexpr uint ArrSizeInternal(T(&)[N])
{
    return N;
}

}

#define vkr_arr_len(arr) internal::ArrSizeInternal(arr)
//#define VKR_ARR_COUNT(arr) sizeof(arr)/sizeof(arr[0])
