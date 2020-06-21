#include "Camera.h"

#include "Input.h"
#include "Render.h"

namespace vkr
{

//------------------------------------------------------------------------------
void Camera::UpdateCameraVectors()
{
    if (angles_.x > 89.0f)
        angles_.x = 89;
    else if (angles_.x < -89.0f)
        angles_.x = -89;

    forward_.x = cos(DegToRad(angles_.y)) * cos(DegToRad(angles_.x));
    forward_.y = sin(DegToRad(angles_.x));
    forward_.z = sin(DegToRad(angles_.y)) * cos(DegToRad(angles_.x));

    right_ = math::UP.Cross(forward_).Normalized();
}

//------------------------------------------------------------------------------
void Camera::Update()
{
    if (g_Input->GetState(VK_RBUTTON))
    {
        g_Input->SetMouseMode(MouseMode::Relative);
        Vec2 mouseDelta = g_Input->GetMouseDelta();
        angles_.x -= mouseDelta.y * 0.1f;
        angles_.y -= mouseDelta.x * 0.1f;
        
        if (mouseDelta != Vec2{})
        {
            UpdateCameraVectors();
        }
    }
    else
    {
        g_Input->SetMouseMode(MouseMode::Absolute);
    }

    if (g_Input->GetState('W'))
    {
        pos_ += forward_ * speed_ * g_Render->GetDTime();
    }
    else if (g_Input->GetState('S'))
    {
        pos_ -= forward_ * speed_ * g_Render->GetDTime();
    }
    
    if (g_Input->GetState('D'))
    {
        pos_ += right_ * speed_ * g_Render->GetDTime();
    }
    else if (g_Input->GetState('A'))
    {
        pos_ -= right_ * speed_ * g_Render->GetDTime();
    }

    float extent = 20;
    //ubo->projection = MakeOrthographicProjection(-extent, extent, -extent / g_Render->GetAspect(), extent / g_Render->GetAspect(), 0.1f, 1000);
    projection_ = MakePerspectiveProjection(DegToRad(fovy_), g_Render->GetAspect(), near_, far_);

    toCamera_ = MakeLookAt(pos_, pos_ + forward_);
}

//------------------------------------------------------------------------------
const Mat44& Camera::ToCamera() const
{
    return toCamera_;
}

//------------------------------------------------------------------------------
const Mat44& Camera::ToProjection() const
{
    return projection_;
}

}