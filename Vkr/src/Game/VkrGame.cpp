#include "Game/VkrGame.h"

#include "Render/Material.h"

#include "Render/Render.h"

#include "Input/Input.h"

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
RESULT VkrGame::InitWin32()
{
    if (HS_FAILED(g_Render->AddMaterial(MakeUnique<SkyboxMaterial>())))
        return R_FAIL;

    if (HS_FAILED(g_Render->AddMaterial(MakeUnique<PBRMaterial>())))
        return R_FAIL;

    g_Render->GetCamera().InitAsPerspective(Vec3(-8, -7, 15), Vec3(0, 0, 0));

    return R_OK;
}

//------------------------------------------------------------------------------
RESULT VkrGame::OnWindowResized()
{
    return R_OK;
}

//------------------------------------------------------------------------------
void VkrGame::Update()
{
    g_Render->GetCamera().UpdateFreeFly();
}

}
