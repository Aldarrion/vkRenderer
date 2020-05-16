#include "Material.h"

#include "Render.h"
#include "Texture.h"
#include "ShaderManager.h"
#include "VertexBuffer.h"

#include "vkr_Shaderc.h"
#include "vkr_Image.h"
#include <string>

namespace vkr
{

//------------------------------------------------------------------------------
RESULT TexturedTriangleMaterial::Init()
{
    int texWidth, texHeight, texChannels;
    stbi_uc* pixels = stbi_load("textures/grass_tile.png", &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);

    texture_ = new Texture(VK_FORMAT_R8G8B8A8_UNORM, VkExtent3D{ (uint)texWidth, (uint)texHeight, 1 });

    auto texAllocRes = texture_->Allocate(pixels, "GrassTile");
    stbi_image_free(pixels);
    
    if (FAILED(texAllocRes))
        return R_FAIL;

    triangleVert_ = g_Render->GetShaderManager()->GetOrCreateShader("Triangle_vs.hlsl");
    triangleFrag_ = g_Render->GetShaderManager()->GetOrCreateShader("Triangle_fs.hlsl");

    if (!triangleVert_ || !triangleFrag_)
        return R_FAIL;

    return R_OK;
}

//------------------------------------------------------------------------------
void TexturedTriangleMaterial::Draw()
{
    g_Render->SetShader<PS_VERT>(triangleVert_);
    g_Render->SetShader<PS_FRAG>(triangleFrag_);
    g_Render->SetTexture(0, texture_);
    g_Render->Draw(3, 0);
}


//------------------------------------------------------------------------------
struct ShapeVertex
{
    float position_[4];
    float color_[4];
};

//------------------------------------------------------------------------------
RESULT ShapeMaterial::Init()
{
    shapeVert_ = g_Render->GetShaderManager()->GetOrCreateShader("Shape_vs.hlsl");
    shapeFrag_ = g_Render->GetShaderManager()->GetOrCreateShader("Shape_fs.hlsl");

    if (!shapeVert_ || !shapeFrag_)
        return R_FAIL;

    vertexBuffer_ = new VertexBuffer(1024);
    if (FAILED(vertexBuffer_->Init()))
        return R_FAIL;

    auto mapped = (ShapeVertex*) vertexBuffer_->Map();

    mapped[0].position_[0] = 100;
    mapped[0].position_[1] = 100;

    mapped[1].position_[0] = 400;
    mapped[1].position_[1] = 100;

    mapped[2].position_[0] = 200;
    mapped[2].position_[1] = 400;

    vertexBuffer_->Unmap();

    return R_OK;
}

//------------------------------------------------------------------------------
void ShapeMaterial::Draw()
{
    // TODO move away
    VkVertexInputAttributeDescription attributeDescriptions[2]{};
    attributeDescriptions[0].binding = 0;
    attributeDescriptions[0].location = 0;
    attributeDescriptions[0].format = VK_FORMAT_R32G32B32A32_SFLOAT;
    attributeDescriptions[0].offset = 0;

    attributeDescriptions[1].binding = 0;
    attributeDescriptions[1].location = 1;
    attributeDescriptions[1].format = VK_FORMAT_R32G32B32A32_SFLOAT;
    attributeDescriptions[1].offset = 4;

    VkVertexInputBindingDescription bindingDescription{};
    bindingDescription.binding = 0;
    bindingDescription.stride = sizeof(ShapeVertex);
    bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

    VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
    vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertexInputInfo.vertexBindingDescriptionCount = 1;
    vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
    vertexInputInfo.vertexAttributeDescriptionCount = VKR_ARR_LEN(attributeDescriptions);
    vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions;


    g_Render->SetVertexLayout(0, &vertexInputInfo);

    g_Render->SetShader<PS_VERT>(shapeVert_);
    g_Render->SetShader<PS_FRAG>(shapeFrag_);

    g_Render->SetVertexBuffer(0, vertexBuffer_, 0);

    g_Render->Draw(3, 0);
}

}
