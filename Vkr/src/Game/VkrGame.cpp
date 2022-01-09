#include "Game/VkrGame.h"

#include "Render/Material.h"

#include "Render/Render.h"

#include "Input/Input.h"

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

static UniquePtr<Material> skyboxMaterial;
static UniquePtr<PBRMaterial> pbrMaterial[HS_ARR_LEN(pbrBox)];

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
    for (uint i = 0; i < HS_ARR_LEN(pbrBox); ++i)
    {
        pbrMaterial[i] = MakeUnique<PBRMaterial>();
        if (HS_FAILED(pbrMaterial[i]->Init()))
            return R_FAIL;

        pbrBox[i].material_ = pbrMaterial[i].Get();
        pbrBox[i].transform_ = Mat44::Scale(3);
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
}

}
