#include "Material.h"

#include "Render.h"
#include "Texture.h"
#include "ShaderManager.h"
#include "VertexBuffer.h"
#include "DynamicUniformBuffer.h"
#include "VertexTypes.h"

#include "vkr_Image.h"
#include <string>

namespace vkr
{

//------------------------------------------------------------------------------
RESULT TexturedTriangleMaterial::Init()
{
    {
        int texWidth, texHeight, texChannels;
        stbi_uc* pixels = stbi_load("textures/grass_tile.png", &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);

        texture_ = new Texture(VK_FORMAT_R8G8B8A8_UNORM, VkExtent3D{ (uint)texWidth, (uint)texHeight, 1 });

        auto texAllocRes = texture_->Allocate(pixels, "GrassTile");
        stbi_image_free(pixels);
    
        if (FAILED(texAllocRes))
            return R_FAIL; // TODO release resources
    }

    {
        int texWidth, texHeight, texChannels;
        stbi_uc* pixels = stbi_load("textures/tree.png", &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);

        textureTree_ = new Texture(VK_FORMAT_R8G8B8A8_UNORM, VkExtent3D{ (uint)texWidth, (uint)texHeight, 1 });

        auto texAllocRes = textureTree_->Allocate(pixels, "Tree");
        stbi_image_free(pixels);
    
        if (FAILED(texAllocRes))
            return R_FAIL; // TODO release resources
    }

    {
        int texWidth, texHeight, texChannels;
        stbi_uc* pixels = stbi_load("textures/box.png", &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);

        textureBox_ = new Texture(VK_FORMAT_R8G8B8A8_UNORM, VkExtent3D{ (uint)texWidth, (uint)texHeight, 1 });

        auto texAllocRes = textureBox_->Allocate(pixels, "Box");
        stbi_image_free(pixels);
    
        if (FAILED(texAllocRes))
            return R_FAIL; // TODO release resources
    }

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
    g_Render->SetTexture(1, textureBox_);
    g_Render->SetTexture(2, textureTree_);
    g_Render->Draw(3, 0);
}


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

    mapped[0].position_.v[0] = 100;
    mapped[0].position_.v[1] = 100;

    mapped[1].position_.v[0] = 400;
    mapped[1].position_.v[1] = 100;

    mapped[2].position_.v[0] = 200;
    mapped[2].position_.v[1] = 400;

    mapped[0].color_ = 
    mapped[1].color_ = 
    mapped[2].color_ = 0xffff3333;

    vertexBuffer_->Unmap();

    vertexLayout_ = ShapeVertexLayout();

    return R_OK;
}

//------------------------------------------------------------------------------
void ShapeMaterial::Draw()
{
    g_Render->SetVertexLayout(0, vertexLayout_);

    g_Render->SetShader<PS_VERT>(shapeVert_);
    g_Render->SetShader<PS_FRAG>(shapeFrag_);

    g_Render->SetVertexBuffer(0, vertexBuffer_, 0);

    g_Render->Draw(3, 0);
}

//------------------------------------------------------------------------------
RESULT PhongMaterial::Init()
{
    phongVert_ = g_Render->GetShaderManager()->GetOrCreateShader("Phong_vs.hlsl");
    phongFrag_ = g_Render->GetShaderManager()->GetOrCreateShader("Phong_fs.hlsl");

    if (!phongVert_ || !phongFrag_)
        return R_FAIL;

    return R_OK;
}

//------------------------------------------------------------------------------
void PhongMaterial::Draw()
{
    struct MatrixUBO
    {
        Mat44 projection;
    };


    void* mapped;
    DynamicUBOEntry constBuffer = g_Render->GetUBOCache()->BeginAlloc(sizeof(MatrixUBO), &mapped);

    auto ubo = (MatrixUBO*)mapped;
    float extent = 20;
    //ubo->projection = MakeOrthographicProjection(-extent, extent, -extent / g_Render->GetAspect(), extent / g_Render->GetAspect(), 0.1f, 1000);
    ubo->projection = MakePerspectiveProjection(DegToRad(75), g_Render->GetAspect(), 0.1f, 100);

    Vec4 res = Vec4(100, 100, 10, 1) * ubo->projection;

    g_Render->GetUBOCache()->EndAlloc();

    g_Render->SetDynamicUbo(1, &constBuffer);


    g_Render->SetShader<PS_VERT>(phongVert_);
    g_Render->SetShader<PS_FRAG>(phongFrag_);

    g_Render->Draw(3, 0);
}

}
