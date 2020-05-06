#pragma once

#include "Shader.h"
#include "Enums.h"

namespace vkr
{

class Material
{
public:
    void Draw();
    RESULT ReloadShaders();

private:
    Shader triangleVert_{};
    Shader triangleFrag_{};
};

}
