#pragma once

#include "vkr_Math.h"

namespace vkr
{

//------------------------------------------------------------------------------
class Camera
{
public:
    const Mat44& ToCamera() const;
    const Mat44& ToProjection() const;

    void Update();

private:
    Mat44 toCamera_;
    Mat44 projection_;

    Vec3 pos_{ 0, 0, 0 };
    Vec3 forward_{ math::FORWARD };
    Vec3 right_{ math::RIGHT };

    Vec2 angles_{ 0, 90 };
    float speed_{ 20 };
    float fovy_{ 75 };
    float near_{ 0.01f };
    float far_{ 1000 };

    void UpdateCameraVectors();
};

}
