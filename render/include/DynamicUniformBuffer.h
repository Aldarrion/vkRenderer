#pragma once

#include "Types.h"
#include "VkTypes.h"
#include "Enums.h"

namespace vkr
{

//------------------------------------------------------------------------------
class DynamicUniformBuffer
{
public:
    DynamicUniformBuffer(uint size);

    RESULT Init();

    void* Map();
    void Unmap();

    VkBuffer GetBuffer() const;
    uint GetSize() const;

private:
    VkBuffer        buffer_{};
    VmaAllocation   allocation_{};
    VkBufferView    view_{};
    uint            size_;
};

//------------------------------------------------------------------------------
struct DynamicUBOEntry
{
    VkBuffer buffer_{};
    uint dynOffset_{};
    uint size_{};
};

//------------------------------------------------------------------------------
/*class DynamicUBOCache
{
public:
    DynamicUBOEntry 

private:


};*/

}
