#include "DrawCanvas.h"

#include "Render.h"
#include "ShaderManager.h"
#include "VertexTypes.h"
#include "VertexBuffer.h"

namespace vkr
{

//------------------------------------------------------------------------------
RESULT DrawCanvas::Init()
{
    lineVert_ = g_Render->GetShaderManager()->GetOrCreateShader("Shape_vs.hlsl");
    lineFrag_ = g_Render->GetShaderManager()->GetOrCreateShader("Shape_fs.hlsl");

    if (!lineVert_ || !lineVert_)
        return R_FAIL;

    lineVertType_ = ShapeVertexLayout();

    lines_ = new VertexBuffer(1024 * sizeof(ShapeVertex));
    if (FAILED(lines_->Init()))
        return R_FAIL;

    auto verts = (ShapeVertex*)lines_->Map();

    verts[0].position_ = Vec4(50, 50, 0, 0);
    verts[1].position_ = Vec4(500, 50, 0, 0);
    verts[2].position_ = Vec4(600, 200, 0, 0);

    verts[0].color_ =
    verts[1].color_ =
    verts[2].color_ = 0xff2222dd;

    lines_->Unmap();

    return R_OK;
}

//------------------------------------------------------------------------------
void DrawCanvas::Draw()
{
    g_Render->SetVertexBuffer(0, lines_, 0);
    g_Render->SetShader<PS_VERT>(lineVert_);
    g_Render->SetShader<PS_FRAG>(lineFrag_);
    g_Render->SetPrimitiveTopology(VkrPrimitiveTopology::LINE_STRIP);

    g_Render->SetVertexLayout(0, lineVertType_);

    g_Render->Draw(3, 0);
}

}
