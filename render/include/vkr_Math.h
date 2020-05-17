#pragma once

#include "Types.h"
#include "vkr_Assert.h"

namespace vkr
{

//------------------------------------------------------------------------------
inline uint Max(uint a, uint b)
{
    return a > b ? a : b;
}

//------------------------------------------------------------------------------
inline uint Align(uint x, uint align)
{
    vkr_assert(align);
    return ((x + align - 1) / align) * align;
}

}
