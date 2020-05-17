#pragma once

#include "Config.h"

#include "DynamicUniformBufferEntry.h"

#include "Enums.h"
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
static constexpr uint SRV_SLOT_COUNT = 8;
static constexpr uint IMMUTABLE_SAMPLER_COUNT = 1;
static constexpr uint DYNAMIC_UBO_COUNT = 1;

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
class VertexBuffer;
class DynamicUBOCache;
struct DynamicUBOEntry;

//------------------------------------------------------------------------------
struct RenderState
{
    static constexpr uint MAX_VERT_BUFF = 1;
    static constexpr uint INVALID_DESC = (uint)-1;

    Shader*             shaders_[PS_COUNT]{};
    uint                fsTextures_[SRV_SLOT_COUNT]{};
    DynamicUBOEntry*    dynamicUBOs_[DYNAMIC_UBO_COUNT]{};
    
    DynamicUBOEntry     bindlessUBO_{};

    uint64              fsDirtyTextures_{};

    VkBuffer            vertexBuffers_[MAX_VERT_BUFF];
    VkDeviceSize        vbOffsets_[MAX_VERT_BUFF];
    VkDescriptorSet     uboDescSet_;

    VkPipelineVertexInputStateCreateInfo* vertexLayouts_[MAX_VERT_BUFF];

    void Reset();
};

//------------------------------------------------------------------------------
class Render
{
    friend RESULT CreateRender(uint width, uint height);

public:
    RESULT ReloadShaders();
    RESULT InitWin32(HWND hwnd, HINSTANCE hinst);

    // Setting state
    template<PipelineStage stage>
    void SetShader(Shader* shader)
    {
        state_.shaders_[stage] = shader;
    }
    void SetTexture(uint slot, Texture* texture);
    void SetVertexBuffer(uint slot, VertexBuffer* buffer, uint offset);
    void SetVertexLayout(uint slot, VkPipelineVertexInputStateCreateInfo* layout);
    void SetDynamicUbo(uint slot, DynamicUBOEntry* entry);

    // Drawing
    void Draw(uint vertexCount, uint firstVertex);

    void Update();

    VkDevice GetDevice() const;
    VmaAllocator GetAllocator() const;
    ShaderManager* GetShaderManager() const;
    VkCommandBuffer CmdBuff() const;
    uint64 GetCurrentFrame() const;
    uint64 GetSafeFrame() const;


    void TransitionBarrier(
        VkImage img, VkImageSubresourceRange subresource,
        VkAccessFlags accessBefore, VkAccessFlags accessAfter,
        VkImageLayout layoutBefore, VkImageLayout layoutAfter,
        VkPipelineStageFlags stageBefore, VkPipelineStageFlags stageAfter
    );

    uint AddBindlessTexture(VkImageView view);

    const VkPhysicalDeviceProperties& GetPhysDevProps() const;

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

    VkPhysicalDeviceProperties vkPhysicalDeviceProperties_{};

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

    uint64              frame_{};

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
    VkDescriptorPool    bindlessPool_{};
    VkDescriptorSet     bindlessSet_{};
    uint                lastFreeBindlessIndex_{};

    VkDescriptorPool    immutableSamplerPool_{};
    VkDescriptorSet     immutableSamplerSet_{};

    VkDescriptorPool    dynamicUBODPool_[BB_IMG_COUNT]{};

    // UBO cache
    DynamicUBOCache*    uboCache_;

    // Allocator
    VmaAllocator allocator_;

    // Keep alive objects

    // Shaders
    ShaderManager*          shaderManager_{};
    VkDescriptorSetLayout   fsSamplerLayout_{};
    VkDescriptorSetLayout   bindlessTexturesLayout_{};
    VkDescriptorSetLayout   dynamicUBOLayout_{};
    VkPipelineLayout        pipelineLayout_{};

    RenderState state_;

    Array<Material*> materials_;

    RESULT PrepareForDraw();
    RESULT AfterDraw();

    RESULT WaitForFence(VkFence fence);
};



}