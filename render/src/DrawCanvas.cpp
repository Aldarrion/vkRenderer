#include "DrawCanvas.h"

#include "Render.h"
#include "ShaderManager.h"
#include "VertexTypes.h"
#include "VertexBuffer.h"
#include "Input.h"

namespace vkr
{

//------------------------------------------------------------------------------
static constexpr uint MAX_LINE_VERTS = 1024;

//------------------------------------------------------------------------------
RESULT DrawCanvas::Init()
{
    lineVert_ = g_Render->GetShaderManager()->GetOrCreateShader("Shape_vs.hlsl");
    lineFrag_ = g_Render->GetShaderManager()->GetOrCreateShader("Shape_fs.hlsl");

    if (!lineVert_ || !lineVert_)
        return R_FAIL;

    lineVertType_ = ShapeVertexLayout();

    lines_ = new VertexBuffer(MAX_LINE_VERTS * sizeof(ShapeVertex));
    if (FAILED(lines_->Init()))
        return R_FAIL;

    return R_OK;
}

//------------------------------------------------------------------------------
void DrawCanvas::Draw()
{
    // Update
    if (g_Input->IsButtonDown(BTN_LEFT))
    {
        if (lineCount_ == MAX_LINE_VERTS)
            lineCount_ = 0;

        Vec2 mousePos = g_Input->GetMousePos();

        auto verts = (ShapeVertex*)lines_->Map();
        verts[lineCount_].position_ = Vec4(mousePos.x_, mousePos.y_, 0, 0);
        verts[lineCount_].color_ = 0xff2222dd;

        lines_->Unmap();

        ++lineCount_;
    }

    // Render
    if (lineCount_ < 2)
        return;

    g_Render->SetVertexBuffer(0, lines_, 0);
    g_Render->SetShader<PS_VERT>(lineVert_);
    g_Render->SetShader<PS_FRAG>(lineFrag_);
    g_Render->SetPrimitiveTopology(VkrPrimitiveTopology::LINE_STRIP);

    g_Render->SetVertexLayout(0, lineVertType_);

    g_Render->Draw(lineCount_, 0);
}

}
