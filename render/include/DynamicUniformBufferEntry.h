#pragma once

#include "VkTypes.h"
#include "Types.h"

namespace vkr
{

//------------------------------------------------------------------------------
struct DynamicUBOEntry
{
    VkBuffer buffer_{};
    uint dynOffset_{};
    uint size_{};
};

}
