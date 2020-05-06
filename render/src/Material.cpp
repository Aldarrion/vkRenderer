#include "Material.h"

#include "Render.h"

#include "vkr_Shaderc.h"

namespace vkr
{

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
