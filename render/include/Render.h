#pragma once

#include "Config.h"
#include "enums.h"
#include "Types.h"
#include "Array.h"

#include "vkr_Windows.h"
#include "vkr_Vulkan.h"

struct shaderc_compiler;

namespace vkr
{

class Render;
extern Render* g_Render;

//------------------------------------------------------------------------------
enum class ShaderType
{
    Vertex,
    Fragment
};

//------------------------------------------------------------------------------
RESULT CreateRender(uint width, uint height);

//------------------------------------------------------------------------------
struct Shader
{
    uint*   Code;
    uint64  Length;
};

//------------------------------------------------------------------------------
class Render
{
    friend RESULT CreateRender(uint width, uint height);

public:
    RESULT InitWin32(HWND hwnd, HINSTANCE hinst);
    void Update();

private:
    static constexpr auto VK_VERSION = VK_API_VERSION_1_1;
    static constexpr uint VKR_INVALID = -1;

    static constexpr uint BB_IMG_COUNT = 2;

    // Win32
    HWND hwnd_;

    uint                width_{};
    uint                height_{};

    // Core Vulkan
    VkInstance          vkInstance_{};
    VkPhysicalDevice    vkPhysicalDevice_{};
    VkDevice            vkDevice_{};

    // Debug
    #if VKR_DEBUG
        VkDebugReportCallbackEXT debugReportCallback_{};
    #endif

    // Swapchain
    VkSurfaceKHR        vkSurface_{};
    VkSwapchainKHR      vkSwapchain_{};
    uint                currentBBIdx_{};
    VkImage             bbImages_[BB_IMG_COUNT]{};
    VkImageView         bbViews_[BB_IMG_COUNT]{};
    VkFormat            swapChainFormat_{};
    
    // Synchronization
    #if defined(VKR_USE_TIMELINE_SEMAPHORES)
        VkSemaphore         directQueueSemaphore_{};
        uint64              semaphoreValues[BB_IMG_COUNT]{ 1, 1 };
    #endif

    VkFence             directQueueFences_[BB_IMG_COUNT]{};

    // Queues
    uint                directQueueFamilyIdx_{ VKR_INVALID };
    VkQueue             vkDirectQueue_{};
    
    // Command buffers
    VkCommandPool       directCmdPool_{};
    VkCommandBuffer     directCmdBuffers_[BB_IMG_COUNT]{};

    // Keep alive objects

    // Shaders
    shaderc_compiler* shadercCompiler_{};

    Shader triangleVert_{};
    Shader triangleFrag_{};

private:
    RESULT CompileShader(const char* file, ShaderType type, Shader& shader);
};



}