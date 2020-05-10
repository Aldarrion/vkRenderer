#pragma once

#include "Config.h"

#include "Enums.h"

namespace vkr
{

class Texture;
class Shader;

//------------------------------------------------------------------------------
class Material
{
public:
    virtual RESULT Init() = 0;
    virtual void Draw() = 0;
};


//------------------------------------------------------------------------------
class TexturedTriangleMaterial : public Material
{
public:
    RESULT Init() override;
    void Draw() override;

private:
    Shader*     triangleVert_{};
    Shader*     triangleFrag_{};
    Texture*    texture_;
};


//------------------------------------------------------------------------------
class ShapeMaterial : public Material
{
public:
    RESULT Init() override;
    void Draw() override;

private:
    Shader* shapeVert_{};
    Shader* shapeFrag_{};

};

}
