#include "Texture.h"

#include "Render.h"
#include "Allocator.h"
#include "Buffer.h"

namespace vkr
{

//------------------------------------------------------------------------------
Texture::Texture(VkFormat format, VkExtent3D size)
    : format_(format)
    , size_(size)
{
}

//------------------------------------------------------------------------------
RESULT Texture::Allocate(void* data)
{
    VkImageLayout initLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    if (data)
        initLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;

    VkImageCreateInfo imgInfo{};
    imgInfo.sType           = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imgInfo.imageType       = VK_IMAGE_TYPE_2D;
    imgInfo.format          = format_;
    imgInfo.extent          = size_;
    imgInfo.mipLevels       = 1; // TODO implement mipmapping
    imgInfo.arrayLayers     = 1;
    imgInfo.samples         = VK_SAMPLE_COUNT_1_BIT;
    imgInfo.tiling          = VK_IMAGE_TILING_OPTIMAL;
    imgInfo.usage           = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
    imgInfo.sharingMode     = VK_SHARING_MODE_EXCLUSIVE;
    imgInfo.initialLayout   = initLayout;

    VmaAllocationCreateInfo allocInfo{};
    allocInfo.usage         = VMA_MEMORY_USAGE_GPU_ONLY;

    if (VKR_FAILED(vmaCreateImage(g_Render->GetAllocator(), &imgInfo, &allocInfo, &image_, &allocation_, nullptr)))
        return R_FAIL;

    VkImageSubresourceRange subres{};
    subres.aspectMask       = VK_IMAGE_ASPECT_COLOR_BIT;
    subres.baseMipLevel     = 0;
    subres.levelCount       = 1;
    subres.baseArrayLayer   = 0;
    subres.layerCount       = 1;

    if (data)
    {
        uint buffSize = size_.width * size_.height * 4;
        StagingBuffer staging(buffSize);
        if (FAILED(staging.Allocate(data)))
            return R_FAIL;

        VkBufferImageCopy region{};
        region.bufferOffset = 0;
        region.bufferRowLength = 0;
        region.bufferImageHeight = 0;

        region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        region.imageSubresource.mipLevel = 0;
        region.imageSubresource.baseArrayLayer = 0;
        region.imageSubresource.layerCount = 1;

        region.imageOffset = { 0, 0, 0 };
        region.imageExtent = size_;

        vkCmdCopyBufferToImage(g_Render->CmdBuff(), staging.GetBuffer(), image_, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

        VkImageMemoryBarrier barrier{};
        barrier.sType               = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        barrier.srcAccessMask       = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.dstAccessMask       = VK_ACCESS_SHADER_READ_BIT;
        barrier.oldLayout           = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
        barrier.newLayout           = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.image               = image_;
        barrier.subresourceRange    = subres;

        vkCmdPipelineBarrier(
            g_Render->CmdBuff(), 
            VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0,
            0, nullptr,
            0, nullptr,
            1, &barrier
        );

        // TODO put staging to a "keep alive" array
    }

    VkImageViewCreateInfo imgViewInfo{};
    imgViewInfo.sType               = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    imgViewInfo.image               = image_;
    imgViewInfo.viewType            = VK_IMAGE_VIEW_TYPE_2D;
    imgViewInfo.format              = format_;
    imgViewInfo.subresourceRange    = subres;

    vkCreateImageView(g_Render->GetDevice(), &imgViewInfo, nullptr, &srv_);

    return R_OK;
}

//------------------------------------------------------------------------------
void Texture::Free()
{
    // TODO release
}

}
