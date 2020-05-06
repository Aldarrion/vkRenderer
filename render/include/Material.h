#pragma once

#include "Shader.h"
#include "Enums.h"


namespace vkr
{
class Texture;

class Material
{
public:
    RESULT Init();

    void Draw();
    RESULT ReloadShaders();

private:
    Shader triangleVert_{};
    Shader triangleFrag_{};

    Texture* texture_;
};

}
