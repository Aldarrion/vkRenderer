#pragma once

#include "Config.h"
#include "Types.h"
#include "Enums.h"

#include "vkr_Math.h"

#include "vkr_Windows.h"

namespace vkr
{

//------------------------------------------------------------------------------
extern class Input* g_Input;

//------------------------------------------------------------------------------
RESULT CreateInput();

//------------------------------------------------------------------------------
enum MouseButton
{
    BTN_LEFT,
    BTN_MIDDLE,
    BTN_RIGHT,
    BTN_COUNT
};

//------------------------------------------------------------------------------
class Input
{
public:
    RESULT InitWin32(HWND hwnd);

    void EndFrame();

    Vec2 GetMousePos() const;

    bool IsKeyDown() const;
    bool IsButtonDown(MouseButton button) const;

    bool IsKeyUp() const;
    bool IsButtonUp(MouseButton button) const;

    void KeyDown(uint64 key);
    void KeyUp(uint64 key);

    void ButtonDown(MouseButton button);
    void ButtonUp(MouseButton button);

private:
    // Win32
    HWND hwnd_;

    enum class ButtonState
    {
        None,
        Down,
        Up
    };

    ButtonState buttons_[BTN_COUNT]{};
};

}
