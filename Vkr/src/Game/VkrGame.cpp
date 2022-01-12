#include "Game/VkrGame.h"

#include "Render/Material.h"
#include "Render/Render.h"
#include "Render/RenderBufferCache.h"
#include "Render/Buffer.h"

#include "Input/Input.h"

#include "Math/Math.h"

#include "imgui/imgui.h"

namespace hs
{

//------------------------------------------------------------------------------
VkrGame* g_Game{};

//------------------------------------------------------------------------------
RESULT CreateGame()
{
    g_Game = new VkrGame;
    g_GameBase = g_Game;

    return R_OK;
}

//------------------------------------------------------------------------------
void DestroyGame()
{
    delete g_Game;
    g_Game = nullptr;
    g_GameBase = nullptr;
}

//------------------------------------------------------------------------------
// TODO better obviously
static VisualObject skybox;
static VisualObject pbrBox[4];
RenderBuffer g_BoxVertexBuffer;
RenderBuffer g_BoxIndexBuffer;

static UniquePtr<Material> skyboxMaterial;
static UniquePtr<PBRMaterial> pbrMaterial[HS_ARR_LEN(pbrBox)];

void CreatePbrBoxBuffers()
{
    // Vertex buffer
    int boxBufferSize = 3 * 8 * sizeof(ObjectVertex);
    HS_CHECK(g_BoxVertexBuffer.Init(RenderBufferType::Vertex, RenderBufferMemory::DeviceLocal, boxBufferSize));

    ObjectVertex* verts;
    RenderBufferEntry stagingVerts = g_Render->GetVertexCache()->BeginAlloc(boxBufferSize, sizeof(ObjectVertex), (void**)&verts);

    verts[0] = ObjectVertex{ Vec4(-1.0, 1.0, -1.0, 1), Vec4(0.0, 1.0, 0.0, 1) };
    verts[1] = ObjectVertex{ Vec4(1.0, 1.0, -1.0, 1), Vec4(0.0, 1.0, 0.0, 1) };
    verts[2] = ObjectVertex{ Vec4(1.0, 1.0, 1.0, 1), Vec4(0.0, 1.0, 0.0, 1) };
    verts[3] = ObjectVertex{ Vec4(-1.0, 1.0, 1.0, 1), Vec4(0.0, 1.0, 0.0, 1) };
    verts[4] = ObjectVertex{ Vec4(-1.0, -1.0, -1.0, 1), Vec4(0.0, -1.0, 0.0, 1) };
    verts[5] = ObjectVertex{ Vec4(1.0, -1.0, -1.0, 1), Vec4(0.0, -1.0, 0.0, 1) };
    verts[6] = ObjectVertex{ Vec4(1.0, -1.0, 1.0, 1), Vec4(0.0, -1.0, 0.0, 1) };
    verts[7] = ObjectVertex{ Vec4(-1.0, -1.0, 1.0, 1), Vec4(0.0, -1.0, 0.0, 1) };
    verts[8] = ObjectVertex{ Vec4(-1.0, -1.0, 1.0, 1), Vec4(-1.0, 0.0, 0.0, 1) };
    verts[9] = ObjectVertex{ Vec4(-1.0, -1.0, -1.0, 1), Vec4(-1.0, 0.0, 0.0, 1) };
    verts[10] = ObjectVertex{ Vec4(-1.0, 1.0, -1.0, 1), Vec4(-1.0, 0.0, 0.0, 1) };
    verts[11] = ObjectVertex{ Vec4(-1.0, 1.0, 1.0, 1), Vec4(-1.0, 0.0, 0.0, 1) };
    verts[12] = ObjectVertex{ Vec4(1.0, -1.0, 1.0, 1), Vec4(1.0, 0.0, 0.0, 1) };
    verts[13] = ObjectVertex{ Vec4(1.0, -1.0, -1.0, 1), Vec4(1.0, 0.0, 0.0, 1) };
    verts[14] = ObjectVertex{ Vec4(1.0, 1.0, -1.0, 1), Vec4(1.0, 0.0, 0.0, 1) };
    verts[15] = ObjectVertex{ Vec4(1.0, 1.0, 1.0, 1), Vec4(1.0, 0.0, 0.0, 1) };
    verts[16] = ObjectVertex{ Vec4(-1.0, -1.0, -1.0, 1), Vec4(0.0, 0.0, -1.0, 1) };
    verts[17] = ObjectVertex{ Vec4(1.0, -1.0, -1.0, 1), Vec4(0.0, 0.0, -1.0, 1) };
    verts[18] = ObjectVertex{ Vec4(1.0, 1.0, -1.0, 1), Vec4(0.0, 0.0, -1.0, 1) };
    verts[19] = ObjectVertex{ Vec4(-1.0, 1.0, -1.0, 1), Vec4(0.0, 0.0, -1.0, 1) };
    verts[20] = ObjectVertex{ Vec4(-1.0, -1.0, 1.0, 1), Vec4(0.0, 0.0, 1.0, 1) };
    verts[21] = ObjectVertex{ Vec4(1.0, -1.0, 1.0, 1), Vec4(0.0, 0.0, 1.0, 1) };
    verts[22] = ObjectVertex{ Vec4(1.0, 1.0, 1.0, 1), Vec4(0.0, 0.0, 1.0, 1) };
    verts[23] = ObjectVertex{ Vec4(-1.0, 1.0, 1.0, 1), Vec4(0.0, 0.0, 1.0, 1) };

    g_Render->GetVertexCache()->EndAlloc();

    RenderBufferEntry vertexBuffer{ g_BoxVertexBuffer.GetBuffer(), 0, g_BoxVertexBuffer.GetSize() };
    RenderCopyBuffer(g_Render->CmdBuff(), vertexBuffer, stagingVerts);

    // Index buffer
    constexpr uint cubeIndices[] =
    {
        3, 1, 0,
        2, 1, 3,

        6, 4, 5,
        7, 4, 6,

        11, 9, 8,
        10, 9, 11,

        14, 12, 13,
        15, 12, 14,

        19, 17, 16,
        18, 17, 19,

        22, 20, 21,
        23, 20, 22
    };

    HS_CHECK(g_BoxIndexBuffer.Init(RenderBufferType::Index, RenderBufferMemory::DeviceLocal, sizeof(cubeIndices)));

    uint* indices;
    RenderBufferEntry stagingIndices = g_Render->GetIndexCache()->BeginAlloc<uint>(HS_ARR_LEN(cubeIndices), &indices);

    memcpy(indices, cubeIndices, sizeof(cubeIndices));
    g_Render->GetIndexCache()->EndAlloc();

    RenderBufferEntry indexBuffer{ g_BoxIndexBuffer.GetBuffer(), 0, g_BoxIndexBuffer.GetSize() };
    RenderCopyBuffer(g_Render->CmdBuff(), indexBuffer, stagingIndices);

    VkBufferMemoryBarrier barriers[2]{};
    barriers[0].sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER;
    barriers[0].srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
    barriers[0].dstAccessMask = VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT;
    barriers[0].buffer = g_BoxVertexBuffer.GetBuffer();
    barriers[0].size = g_BoxVertexBuffer.GetSize();

    barriers[1].sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER;
    barriers[1].srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
    barriers[1].dstAccessMask = VK_ACCESS_INDEX_READ_BIT;
    barriers[1].buffer = g_BoxIndexBuffer.GetBuffer();
    barriers[1].size = g_BoxIndexBuffer.GetSize();

    vkCmdPipelineBarrier(g_Render->CmdBuff(),
        VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_VERTEX_INPUT_BIT, 0,
        0, nullptr,
        HS_ARR_LEN(barriers), barriers,
        0, nullptr
    );
}

//------------------------------------------------------------------------------
RESULT VkrGame::Init()
{
    CameraInitAsPerspective(g_Render->GetCamera(), Vec3(0, 0, 0), Vec3(0, 0, 1));
    freeflyCamera_.camera_ = g_Render->GetCamera();

    // Skybox
    skyboxMaterial = MakeUnique<SkyboxMaterial>();
    if (HS_FAILED(skyboxMaterial->Init()))
        return R_FAIL;

    skybox.material_ = skyboxMaterial.Get();

    // Pbr boxes
    CreatePbrBoxBuffers();

    for (uint i = 0; i < HS_ARR_LEN(pbrBox); ++i)
    {
        pbrMaterial[i] = MakeUnique<PBRMaterial>();
        if (HS_FAILED(pbrMaterial[i]->Init()))
            return R_FAIL;

        pbrBox[i].material_ = pbrMaterial[i].Get();
        pbrBox[i].transform_ = Mat44::Scale(3);

        VertexBuffer vb;
        vb.buffer_.buffer_ = g_BoxVertexBuffer.GetBuffer();
        vb.buffer_.offset_ = 0;
        vb.buffer_.size_ = g_BoxVertexBuffer.GetSize();
        vb.vertexSize_ = sizeof(ObjectVertex);

        pbrBox[i].vertexBuffer_ = vb;

        IndexBuffer ib;
        ib.buffer_.buffer_ = g_BoxIndexBuffer.GetBuffer();
        ib.buffer_.offset_ = 0;
        ib.buffer_.size_ = g_BoxIndexBuffer.GetSize();

        pbrBox[i].indexBuffer_ = ib;
    }
    pbrBox[0].transform_.SetPosition(Vec3(5, -5, 20));
    pbrBox[1].transform_.SetPosition(Vec3(-5, -5, 20));
    pbrBox[2].transform_.SetPosition(Vec3(5, 5, 20));
    pbrBox[3].transform_.SetPosition(Vec3(-5, 5, 20));

    return R_OK;
}

//------------------------------------------------------------------------------
RESULT VkrGame::OnWindowResized()
{
    return R_OK;
}

int boxIdx = 0;
//------------------------------------------------------------------------------
void VkrGame::Update()
{
    CameraUpdateFreeFly(&freeflyCamera_);

    g_Render->RenderObject(&skybox);

    ImGui::Begin("PBR settings");
        ImGui::SliderInt("Box idx", &boxIdx, 0, HS_ARR_LEN(pbrBox) - 1);

        auto mat = ((PBRMaterial*)pbrBox[boxIdx].material_);
        Vec3 albedo = mat->GetAlbedo();
        float roughness = mat->GetRoughness();
        float metallic = mat->GetMetallic();
        float ao = mat->GetAo();

        ImGui::ColorPicker3("Albedo", albedo.v);
        ImGui::SliderFloat("Roughness", &roughness, 0, 1);
        ImGui::SliderFloat("Metellic", &metallic, 0, 1);
        ImGui::SliderFloat("Ambient", &ao, 0, 1);

        mat->SetAlbedo(albedo);
        mat->SetRoughness(roughness);
        mat->SetMetallic(metallic);
        mat->SetAo(ao);
    ImGui::End();

    g_Render->RenderObjects(MakeSpan(pbrBox));
}

//------------------------------------------------------------------------------
void VkrGame::Free()
{
    g_BoxVertexBuffer.Free();
}

}
