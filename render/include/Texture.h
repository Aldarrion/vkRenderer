#pragma once

#include "VkTypes.h"
#include "Enums.h"

namespace vkr
{

class Texture
{
public:
    Texture(VkFormat format, VkExtent3D size);

    RESULT Allocate(void* data, const char* diagName = nullptr);
    void Free();

private:
    VkImage         image_;
    VmaAllocation   allocation_;
    VkFormat        format_;
    VkExtent3D      size_;

    VkImageView     srv_;
};

}