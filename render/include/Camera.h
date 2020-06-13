#pragma once

#include "vkr_Math.h"

namespace vkr
{

//------------------------------------------------------------------------------
class Camera
{
public:
    Vec4 GetPosition() const;
    Mat44 ToCamera() const;
    Mat44 ToProjection() const;

private:
    Mat44 camera_;
    Mat44 projection_;
};

}
