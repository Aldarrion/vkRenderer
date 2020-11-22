#include "Game/VkrGame.h"

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
}

}
