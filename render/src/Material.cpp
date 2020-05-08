#include "Material.h"

#include "Render.h"
#include "Texture.h"

#include "vkr_Shaderc.h"
#include "vkr_Image.h"
#include <string>

namespace vkr
{

//------------------------------------------------------------------------------
RESULT Material::Init()
{
    int texWidth, texHeight, texChannels;
    stbi_uc* pixels = stbi_load("textures/grass_tile.png", &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);

    texture_ = new Texture(VK_FORMAT_R8G8B8A8_UNORM, VkExtent3D{ (uint)texWidth, (uint)texHeight, 1 });

    auto texAllocRes = texture_->Allocate(pixels, "GrassTile");
    stbi_image_free(pixels);
    
    if (FAILED(texAllocRes))
        return R_FAIL;

    return R_OK;
}

//------------------------------------------------------------------------------
void Material::Draw()
{
    g_Render->SetShader<PS_VERT>(&triangleVert_);
    g_Render->SetShader<PS_FRAG>(&triangleFrag_);
    g_Render->Draw(3, 0);
}

//------------------------------------------------------------------------------
RESULT Material::ReloadShaders()
{
    // TODO reload shaders in render only, materials will have only a pointer to the shader, the shader will be held in render
    // on shader reload we just change the internals of the pointed to object

    if (g_Render->CompileShader("../shaders/triangle.vert", PipelineStage::PS_VERT, triangleVert_) != R_OK)
        return R_FAIL;
    if (g_Render->CompileShader("../shaders/triangle.frag", PipelineStage::PS_FRAG, triangleFrag_) != R_OK)
        return R_FAIL;

    return R_OK;
}


}
