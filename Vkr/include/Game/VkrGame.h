#pragma once

#include "Game/GameBase.h"

namespace hs
{

extern class VkrGame* g_Game;

//------------------------------------------------------------------------------
RESULT CreateGame();
void DestroyGame();

//------------------------------------------------------------------------------
class VkrGame : public GameBase
{
public:
    #if HS_WINDOWS
        RESULT InitWin32() override;
    #endif
    RESULT OnWindowResized() override;
    void Update() override;
};

}
