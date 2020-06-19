#include "Logging.h"
#include "Render.h"
#include "Types.h"
#include "Input.h"

#include "vkr_Assert.h"
#include "vkr_Windows.h"

#include <chrono>

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
static vkr::RESULT InitWindow(int width, int height, HINSTANCE instance)
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
        return vkr::R_FAIL;
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
        return vkr::R_FAIL;
    }

    auto hr = ShowWindow(g_hwnd, SW_SHOW);

    if (FAILED(hr))
    {
        vkr::Log(vkr::LogLevel::Error, "Failed to show window, terminating");
        return vkr::R_FAIL;
    }

    return vkr::R_OK;
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
        vkr::Log(vkr::LogLevel::Error, "Failed to create render");
        return -1;
    }
    vkr_assert(vkr::g_Render);

    if (FAILED(vkr::g_Render->InitWin32(g_hwnd, instance)))
    {
        vkr::Log(vkr::LogLevel::Error, "Failed to init render");
        return -1;
    }

    if (FAILED(vkr::CreateInput()))
    {
        vkr::Log(vkr::LogLevel::Error, "Failed to crete input");
        return -1;
    }
    vkr_assert(vkr::g_Input);

    if (FAILED(vkr::g_Input->InitWin32(g_hwnd)))
    {
        vkr::Log(vkr::LogLevel::Error, "Failed to init input");
        return -1;
    }

    bool shouldQuit = false;

    auto start = std::chrono::high_resolution_clock::now();

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
                {
                    // If bit 30 is 1 the key was down even before, and this is just a repeat event
                    if (msg.lParam & (1 << 30))
                        break;

                    if (msg.wParam == VK_ESCAPE)
                        shouldQuit = true;
                    vkr::g_Input->KeyDown(msg.wParam);
                    break;
                }
                case WM_KEYUP:
                {
                    if (msg.wParam == VK_F5)
                        vkr::g_Render->ReloadShaders();
                    vkr::g_Input->KeyUp(msg.wParam);
                    break;
                }
                case WM_LBUTTONDOWN:
                    vkr::g_Input->ButtonDown(vkr::BTN_LEFT);
                    break;
                case WM_RBUTTONDOWN:
                    vkr::g_Input->ButtonDown(vkr::BTN_RIGHT);
                    break;
                case WM_MBUTTONDOWN:
                    vkr::g_Input->ButtonDown(vkr::BTN_MIDDLE);
                    break;
                case WM_LBUTTONUP:
                    vkr::g_Input->ButtonUp(vkr::BTN_LEFT);
                    break;
                case WM_RBUTTONUP:
                    vkr::g_Input->ButtonUp(vkr::BTN_RIGHT);
                    break;
                case WM_MBUTTONUP:
                    vkr::g_Input->ButtonUp(vkr::BTN_MIDDLE);
                    break;

                default:
                    break;
            }
        }
        else
        {
            auto elapsed = std::chrono::high_resolution_clock::now() - start;
            start = std::chrono::high_resolution_clock::now();

            float dTime = elapsed.count() / (1000.0f * 1000 * 1000);

            vkr::g_Input->Update();

            vkr::g_Render->Update(dTime);

            vkr::g_Input->EndFrame();
        }
    }

    return 0;
}