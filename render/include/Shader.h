#pragma once

#include "VkTypes.h"
#include "Types.h"

namespace vkr
{

//------------------------------------------------------------------------------
class Shader
{
public:
    VkShaderModule vkShader_;
    uint16 id_;
};

}
