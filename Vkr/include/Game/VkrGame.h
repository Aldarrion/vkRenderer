#pragma once

#include "Game/GameBase.h"
#include "World/Camera.h"

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
    void Free() override;
    void Update() override;

private:
    CameraFreelyController freeflyCamera_;
};

}
