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

//------------------------------------------------------------------------------
struct Vec4
{
    union
    {
        float v_[4];
        struct
        {
            float x_, y_, z_, w_;
        };
    };

    Vec4() = default;
    Vec4(float x, float y, float z, float w) : x_(x), y_(y), z_(z), w_(w) {}
};

//------------------------------------------------------------------------------
struct Vec2
{
    union
    {
        float v_[2];
        struct
        {
            float x_, y_;
        };
    };

    Vec2() = default;
    Vec2(float x, float y) : x_(x), y_(y) {}
};

}
