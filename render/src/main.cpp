#include "vkr_Windows.h"

#include "Logging.h"
#include "Render.h"
#include "Types.h"
#include "vkr_Assert.h"

//------------------------------------------------------------------------------
HWND g_hwnd{};

//------------------------------------------------------------------------------
static LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
        case WM_DESTROY:
            PostQuitMessage(0);
            break;
        default:
            return DefWindowProc(hWnd, msg, wParam, lParam);
    }

    return 0;
}

//------------------------------------------------------------------------------
static RESULT InitWindow(int width, int height, HINSTANCE instance)
{
    WNDCLASSA wCls{};
    wCls.style = CS_HREDRAW | CS_VREDRAW;
    wCls.lpfnWndProc = WndProc;
    wCls.cbClsExtra = 0;
    wCls.cbWndExtra = 0;
    wCls.hInstance = instance;
    wCls.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wCls.lpszClassName = "VkRenderWindowClass";
    if (!RegisterClassA(&wCls))
    {
        Log(vkr::LogLevel::Error, "Failed to register window class, terminating");
        return R_FAIL;
    }

    RECT rc = { 0, 0, width, height};
    AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);

    g_hwnd = CreateWindowA(
        wCls.lpszClassName,
        "VkRenderer",
        WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
        CW_USEDEFAULT, CW_USEDEFAULT,
        rc.right - rc.left, rc.bottom - rc.top,
        nullptr, nullptr,
        instance,
        nullptr
    );

    if (!g_hwnd)
    {
        vkr::Log(vkr::LogLevel::Error, "Failed to create window, terminating");
        return R_FAIL;
    }

    auto hr = ShowWindow(g_hwnd, SW_SHOW);

    if (FAILED(hr))
    {
        vkr::Log(vkr::LogLevel::Error, "Failed to show window, terminating");
        return R_FAIL;
    }

    return R_OK;
}

//------------------------------------------------------------------------------
int CALLBACK WinMain(HINSTANCE instance, HINSTANCE prevInstance, LPSTR cmdLine, int showCmd)
{
    vkr::Log(vkr::LogLevel::Info, "VkRenderer start\n");

    int WIDTH = 1280;
    int HEIGHT = 720;

    if (FAILED(InitWindow(WIDTH, HEIGHT, instance)))
        return -1;

    if (FAILED(vkr::CreateRender(WIDTH, HEIGHT)))
    {
        vkr::Log(vkr::LogLevel::Error, "Failed to init render, terminating");
        return -1;
    }

    vkr_assert(vkr::g_Render);

    if (FAILED(vkr::g_Render->InitWin32(g_hwnd, instance)))
    {
        vkr::Log(vkr::LogLevel::Error, "Failed to init render");
        return -1;
    }

    bool shouldQuit = false;

    MSG msg{};
    while (!shouldQuit)
    {
        if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE | PM_NOYIELD))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);

            switch (msg.message)
            {
                case WM_QUIT:
                    shouldQuit = true;
                    break;
                case WM_KEYDOWN:
                    if (msg.wParam == VK_ESCAPE)
                        shouldQuit = true;
                    break;
                case WM_KEYUP:
                    if (msg.wParam == VK_F5)
                        vkr::g_Render->ReloadShaders();
                    break;
                default:
                    break;
            }
        }
        else
        {
            vkr::g_Render->Update();
        }
    }

    return 0;
}