#pragma once

#include "Types.h"

namespace vkr
{

//------------------------------------------------------------------------------
template<class KeyT>
struct FibonacciHash
{
    constexpr uint64 operator()(const KeyT key) const
    {
        return (key * 11400714819323198485llu) >> 61;
    }
};

}
