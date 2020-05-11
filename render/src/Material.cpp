#include "Material.h"

#include "Render.h"
#include "Texture.h"
#include "ShaderManager.h"

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

    triangleVert_ = g_Render->GetShaderManager()->GetOrCreateShader("triangle.vert");
    triangleFrag_ = g_Render->GetShaderManager()->GetOrCreateShader("triangle.frag");

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
RESULT ShapeMaterial::Init()
{
    shapeVert_ = g_Render->GetShaderManager()->GetOrCreateShader("shape.vert");
    shapeFrag_ = g_Render->GetShaderManager()->GetOrCreateShader("shape.frag");

    if (!shapeVert_ || !shapeFrag_)
        return R_FAIL;

    return R_OK;
}

//------------------------------------------------------------------------------
void ShapeMaterial::Draw()
{
    g_Render->SetShader<PS_VERT>(shapeVert_);
    g_Render->SetShader<PS_FRAG>(shapeFrag_);

    g_Render->Draw(20 * 3, 0);
}

}
