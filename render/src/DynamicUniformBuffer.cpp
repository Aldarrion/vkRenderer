#include "DynamicUniformBuffer.h"

#include "Allocator.h"
#include "Render.h"

namespace vkr
{

//------------------------------------------------------------------------------
DynamicUniformBuffer::DynamicUniformBuffer(uint size)
    : size_(size)
{
}

//------------------------------------------------------------------------------
RESULT DynamicUniformBuffer::Init()
{
    VkBufferCreateInfo buffInfo{};
    buffInfo.sType          = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    buffInfo.size           = size_;
    buffInfo.usage          = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
    buffInfo.sharingMode    = VK_SHARING_MODE_EXCLUSIVE;

    VmaAllocationCreateInfo allocInfo{};
    allocInfo.usage = VMA_MEMORY_USAGE_CPU_TO_GPU;

    if (VKR_FAILED(vmaCreateBuffer(g_Render->GetAllocator(), &buffInfo, &allocInfo, &buffer_, &allocation_, nullptr)))
        return R_FAIL;

    return R_OK;
}

//------------------------------------------------------------------------------
void* DynamicUniformBuffer::Map()
{
    void* mapped{};
    if (VKR_FAILED(vmaMapMemory(g_Render->GetAllocator(), allocation_, &mapped)))
        return nullptr;
    
    return mapped;
}

//------------------------------------------------------------------------------
void DynamicUniformBuffer::Unmap()
{
    vmaUnmapMemory(g_Render->GetAllocator(), allocation_);
}

//------------------------------------------------------------------------------
VkBuffer DynamicUniformBuffer::GetBuffer() const
{
    return buffer_;
}

//------------------------------------------------------------------------------
uint DynamicUniformBuffer::GetSize() const
{
    return size_;
}

}
