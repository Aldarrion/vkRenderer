#pragma once

#include "Config.h"
#include "enums.h"
#include "Types.h"
#include "Array.h"
#include "VkTypes.h"

#include "vkr_Windows.h"

//------------------------------------------------------------------------------
bool CheckResult(VkResult result, const char* file, int line, const char* fun);

#define VKR_SUCCEED(x) CheckResult(x, __FILE__, __LINE__, #x)
#define VKR_CHECK(x) VKR_SUCCEED(x)
#define VKR_FAILED(x) !VKR_SUCCEED(x)

#define VKR_ALLOCA(size) _alloca(size)


//------------------------------------------------------------------------------
namespace vkr
{

//------------------------------------------------------------------------------
static constexpr uint FRAG_TEX_COUNT = 8;
static constexpr uint IMMUTABLE_SAMPLER_COUNT = 1;

//------------------------------------------------------------------------------
class Render;
extern Render* g_Render;

//------------------------------------------------------------------------------
RESULT CreateRender(uint width, uint height);

//------------------------------------------------------------------------------
VkResult SetDiagName(VkDevice device, uint64 object, VkObjectType type, const char* name);

//------------------------------------------------------------------------------
class Shader;
class Material;
class Texture;
class ShaderManager;

//------------------------------------------------------------------------------
struct RenderState
{
    Shader*     shaders_[PS_COUNT]{};
    Texture*    fsTextures_[FRAG_TEX_COUNT]{};

    uint64      fsDirtyTextures_{};

    void Reset();
};

//------------------------------------------------------------------------------
class Render
{
    friend RESULT CreateRender(uint width, uint height);

public:
    RESULT ReloadShaders();
    RESULT InitWin32(HWND hwnd, HINSTANCE hinst);

    template<PipelineStage stage>
    void SetShader(Shader* shader)
    {
        state_.shaders_[stage] = shader;
    }

    void SetTexture(uint slot, Texture* texture);

    void Draw(uint vertexCount, uint firstVertex);

    void Update();

    VkDevice GetDevice() const;
    VmaAllocator GetAllocator() const;

    VkCommandBuffer CmdBuff() const;

    ShaderManager* GetShaderManager() const;

    void TransitionBarrier(
        VkImage img, VkImageSubresourceRange subresource,
        VkAccessFlags accessBefore, VkAccessFlags accessAfter,
        VkImageLayout layoutBefore, VkImageLayout layoutAfter,
        VkPipelineStageFlags stageBefore, VkPipelineStageFlags stageAfter
    );

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

    uint64              m_iFrame{};

    // Synchronization
    #if defined(VKR_USE_TIMELINE_SEMAPHORES)
        VkSemaphore         directQueueSemaphore_{};
        uint64              semaphoreValues[BB_IMG_COUNT]{ 1, 1 };
    #endif

    VkFence             directQueueFences_[BB_IMG_COUNT]{};
    VkFence             nextImageFence_;

    // Queues
    uint                directQueueFamilyIdx_{ VKR_INVALID };
    VkQueue             vkDirectQueue_{};
    
    // Command buffers
    VkCommandPool       directCmdPool_{};
    VkCommandBuffer     directCmdBuffers_[BB_IMG_COUNT]{};

    VkRenderPass        renderPass_[BB_IMG_COUNT]{};

    // Descriptors
    VkDescriptorPool    fsTexDescPools_[BB_IMG_COUNT]{};

    // Allocator
    VmaAllocator allocator_;

    // Keep alive objects

    // Shaders
    ShaderManager*          shaderManager_{};
    VkDescriptorSetLayout   fsTexLayout_{};
    VkPipelineLayout        pipelineLayout_{};

    RenderState state_;

    Array<Material*> materials_;

    RESULT PrepareForDraw();
    RESULT AfterDraw();

    RESULT WaitForFence(VkFence fence);
};



}