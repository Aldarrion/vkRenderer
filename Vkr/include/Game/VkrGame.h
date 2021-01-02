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
    RESULT Init() override;
    RESULT OnWindowResized() override;
    void Update() override;
};

}
