#include "Input.h   "

#include "Logging.h"
#include "vkr_Windows.h"

namespace vkr
{

//------------------------------------------------------------------------------
Input* g_Input{};

//------------------------------------------------------------------------------
RESULT CreateInput()
{
    g_Input = new Input();

    return R_OK;
}

//------------------------------------------------------------------------------
RESULT Input::InitWin32(HWND hwnd)
{
    hwnd_ = hwnd;

    return R_OK;
}

//------------------------------------------------------------------------------
void Input::EndFrame()
{
    memset(&buttons_, 0, sizeof(buttons_));
}

//------------------------------------------------------------------------------
Vec2 Input::GetMousePos() const
{
    POINT cursorPos;
    if (GetCursorPos(&cursorPos) == 0)
    {
        Log(LogLevel::Error, "Could not retrieve the cursor position, error: %d", GetLastError());
    }
    else
    {
        if (!ScreenToClient(hwnd_, &cursorPos))
        {
            Log(LogLevel::Error, "Could not cast cursor pos to client pos, error: %d", GetLastError());
        }
        else
        {
            return Vec2(cursorPos.x, cursorPos.y);
        }
    }

    return Vec2(0, 0);
}

//------------------------------------------------------------------------------
bool Input::IsButtonDown(MouseButton btn) const
{
    vkr_assert(btn < BTN_COUNT);

    return buttons_[btn] == ButtonState::Down;
}

//------------------------------------------------------------------------------
bool Input::IsButtonUp(MouseButton btn) const
{
    vkr_assert(btn < BTN_COUNT);

    return buttons_[btn] == ButtonState::Up;
}

//------------------------------------------------------------------------------
void Input::ButtonDown(MouseButton button)
{
    vkr_assert(button < BTN_COUNT);

    buttons_[button] = ButtonState::Down;
}

//------------------------------------------------------------------------------
void Input::ButtonUp(MouseButton button)
{
    vkr_assert(button < BTN_COUNT);

    buttons_[button] = ButtonState::Up;
}

}
