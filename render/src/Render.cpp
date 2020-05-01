#include "Render.h"

#include "Logging.h"
#include "vkr_Assert.h"

#include <vulkan/vulkan_win32.h>

#include <malloc.h>
#include <cstdio>

//------------------------------------------------------------------------------
const char* ResultToString(VkResult result)
{
    switch(result)
    {
        case VK_SUCCESS: return "VK_SUCCESS";
        case VK_NOT_READY: return "VK_NOT_READY";
        case VK_TIMEOUT: return "VK_TIMEOUT";
        case VK_EVENT_SET: return "VK_EVENT_SET";
        case VK_EVENT_RESET: return "VK_EVENT_RESET";
        case VK_INCOMPLETE: return "VK_INCOMPLETE";
        case VK_ERROR_OUT_OF_HOST_MEMORY: return "VK_ERROR_OUT_OF_HOST_MEMORY";
        case VK_ERROR_OUT_OF_DEVICE_MEMORY: return "VK_ERROR_OUT_OF_DEVICE_MEMORY";
        case VK_ERROR_INITIALIZATION_FAILED: return "VK_ERROR_INITIALIZATION_FAILED";
        case VK_ERROR_DEVICE_LOST: return "VK_ERROR_DEVICE_LOST";
        case VK_ERROR_MEMORY_MAP_FAILED: return "VK_ERROR_MEMORY_MAP_FAILED";
        case VK_ERROR_LAYER_NOT_PRESENT: return "VK_ERROR_LAYER_NOT_PRESENT";
        case VK_ERROR_EXTENSION_NOT_PRESENT: return "VK_ERROR_EXTENSION_NOT_PRESENT";
        case VK_ERROR_FEATURE_NOT_PRESENT: return "VK_ERROR_FEATURE_NOT_PRESENT";
        case VK_ERROR_INCOMPATIBLE_DRIVER: return "VK_ERROR_INCOMPATIBLE_DRIVER";
        case VK_ERROR_TOO_MANY_OBJECTS: return "VK_ERROR_TOO_MANY_OBJECTS";
        case VK_ERROR_FORMAT_NOT_SUPPORTED: return "VK_ERROR_FORMAT_NOT_SUPPORTED";
        case VK_ERROR_FRAGMENTED_POOL: return "VK_ERROR_FRAGMENTED_POOL";
        case VK_ERROR_UNKNOWN: return "VK_ERROR_UNKNOWN";
        case VK_ERROR_OUT_OF_POOL_MEMORY: return "VK_ERROR_OUT_OF_POOL_MEMORY";
        case VK_ERROR_INVALID_EXTERNAL_HANDLE: return "VK_ERROR_INVALID_EXTERNAL_HANDLE";
        case VK_ERROR_FRAGMENTATION: return "VK_ERROR_FRAGMENTATION";
        case VK_ERROR_INVALID_OPAQUE_CAPTURE_ADDRESS: return "VK_ERROR_INVALID_OPAQUE_CAPTURE_ADDRESS";
        case VK_ERROR_SURFACE_LOST_KHR: return "VK_ERROR_SURFACE_LOST_KHR";
        case VK_ERROR_NATIVE_WINDOW_IN_USE_KHR: return "VK_ERROR_NATIVE_WINDOW_IN_USE_KHR";
        case VK_SUBOPTIMAL_KHR: return "VK_SUBOPTIMAL_KHR";
        case VK_ERROR_OUT_OF_DATE_KHR: return "VK_ERROR_OUT_OF_DATE_KHR";
        case VK_ERROR_INCOMPATIBLE_DISPLAY_KHR: return "VK_ERROR_INCOMPATIBLE_DISPLAY_KHR";
        case VK_ERROR_VALIDATION_FAILED_EXT: return "VK_ERROR_VALIDATION_FAILED_EXT";
        case VK_ERROR_INVALID_SHADER_NV: return "VK_ERROR_INVALID_SHADER_NV";
        case VK_ERROR_INCOMPATIBLE_VERSION_KHR: return "VK_ERROR_INCOMPATIBLE_VERSION_KHR";
        case VK_ERROR_INVALID_DRM_FORMAT_MODIFIER_PLANE_LAYOUT_EXT: return "VK_ERROR_INVALID_DRM_FORMAT_MODIFIER_PLANE_LAYOUT_EXT";
        case VK_ERROR_NOT_PERMITTED_EXT: return "VK_ERROR_NOT_PERMITTED_EXT";
        case VK_ERROR_FULL_SCREEN_EXCLUSIVE_MODE_LOST_EXT: return "VK_ERROR_FULL_SCREEN_EXCLUSIVE_MODE_LOST_EXT";
        case VK_THREAD_IDLE_KHR: return "VK_THREAD_IDLE_KHR";
        case VK_THREAD_DONE_KHR: return "VK_THREAD_DONE_KHR";
        case VK_OPERATION_DEFERRED_KHR: return "VK_OPERATION_DEFERRED_KHR";
        case VK_OPERATION_NOT_DEFERRED_KHR: return "VK_OPERATION_NOT_DEFERRED_KHR";
        case VK_ERROR_PIPELINE_COMPILE_REQUIRED_EXT: return "VK_ERROR_PIPELINE_COMPILE_REQUIRED_EXT";
        //case VK_ERROR_OUT_OF_POOL_MEMORY_KHR: return "VK_ERROR_OUT_OF_POOL_MEMORY_KHR";
        //case VK_ERROR_INVALID_EXTERNAL_HANDLE_KHR: return "VK_ERROR_INVALID_EXTERNAL_HANDLE_KHR";
        //case VK_ERROR_FRAGMENTATION_EXT: return "VK_ERROR_FRAGMENTATION_EXT";
        //case VK_ERROR_INVALID_DEVICE_ADDRESS_EXT: return "VK_ERROR_INVALID_DEVICE_ADDRESS_EXT";
        //case VK_ERROR_INVALID_OPAQUE_CAPTURE_ADDRESS_KHR: return "VK_ERROR_INVALID_OPAQUE_CAPTURE_ADDRESS_KHR";
        //case VK_RESULT_BEGIN_RANGE: return "VK_RESULT_BEGIN_RANGE";
        //case VK_RESULT_END_RANGE: return "VK_RESULT_END_RANGE";
        case VK_RESULT_RANGE_SIZE: return "VK_RESULT_RANGE_SIZE";
        default: return "UNKNOWN CODE";
    }
}

//------------------------------------------------------------------------------
bool CheckResult(VkResult result, const char* file, int line, const char* fun)
{
    if (result != VK_SUCCESS)
    {
        vkr::Log(vkr::LogLevel::Error, "Operation failed, %s(%d), %s, error: %d %s", file, line, fun, result, ResultToString(result));
        vkr_assert(false);
        return false;
    }
    return true;
}

/*#define VKR_CHECK(x) \
    do {\
        auto result = x;\
        if (result != VK_SUCCESS) {\
            vkr::Log(vkr::LogLevel::Error, "Failed to %s, error: %d", #x, result);\
        }\
    } while(false)
    */
#define VKR_SUCCEED(x) CheckResult(x, __FILE__, __LINE__, #x)
#define VKR_CHECK(x) VKR_SUCCEED(x)
#define VKR_FAILED(x) !VKR_SUCCEED(x)

#define VKR_ALLOCA(size) _alloca(size)

#if VKR_DEBUG
    #define DBG_LOG(msg, ...) vkr::Log(vkr::LogLevel::Info, msg, __VA_ARGS__)
#else
    #define DBG_LOG(msg, ...)
#endif

#if VKR_DEBUG
    //------------------------------------------------------------------------------
    VkResult CreateDebugReportCallbackEXT(
        VkInstance instance,
        const VkDebugReportCallbackCreateInfoEXT* pCreateInfo,
        const VkAllocationCallbacks* pAllocator,
        VkDebugReportCallbackEXT* pCallback)
    {
        auto func = (PFN_vkCreateDebugReportCallbackEXT) vkGetInstanceProcAddr(instance, "vkCreateDebugReportCallbackEXT");
        if (func)
            return func(instance, pCreateInfo, pAllocator, pCallback);
        else
            return VK_ERROR_EXTENSION_NOT_PRESENT;
    }
    /*
    void DestroyDebugReportCallbackEXT(
        VkInstance instance, 
        VkDebugReportCallbackEXT callback, 
        const VkAllocationCallbacks* pAllocator
    );*/
#endif

namespace vkr
{

#if VKR_DEBUG
    //------------------------------------------------------------------------------
    void QueueFamiliesToString(uint bits, char* str)
    {
        str[0] = 0;
        char* start = str;
        if (bits & VK_QUEUE_GRAPHICS_BIT)
            start += sprintf(start, "VK_QUEUE_GRAPHICS_BIT ");
        if (bits & VK_QUEUE_COMPUTE_BIT)
            start += sprintf(start, "VK_QUEUE_COMPUTE_BIT ");
        if (bits & VK_QUEUE_TRANSFER_BIT)
            start += sprintf(start, "VK_QUEUE_TRANSFER_BIT ");
        if (bits & VK_QUEUE_SPARSE_BINDING_BIT)
            start += sprintf(start, "VK_QUEUE_SPARSE_BINDING_BIT ");
        if (bits & VK_QUEUE_PROTECTED_BIT)
            start += sprintf(start, "VK_QUEUE_PROTECTED_BIT ");
    }
#endif

VkPhysicalDeviceFeatures CreateRequiredFeatures()
{
    VkPhysicalDeviceFeatures features{};
    
    features.samplerAnisotropy = VK_TRUE;
    features.fillModeNonSolid = VK_TRUE;
    
    return features;
}

//------------------------------------------------------------------------------
Render* g_Render;

//------------------------------------------------------------------------------
RESULT CreateRender(uint width, uint height)
{
    g_Render = new Render();

    g_Render->width_ = width;
    g_Render->height_ = height;

    return R_OK;
}

//------------------------------------------------------------------------------
VkBool32 ValidationCallback(
    VkDebugReportFlagsEXT flags,
    VkDebugReportObjectTypeEXT objType,
    uint64_t obj,
    size_t location,
    int32_t code,
    const char* layerPrefix,
    const char* msg,
    void* userData)
{
    Log(LogLevel::Error, "VALIDATION : %s", msg);

    // Return true only when we want to test the VL themselves
    return VK_FALSE;
}

//------------------------------------------------------------------------------
RESULT Render::InitWin32(HWND hwnd, HINSTANCE hinst)
{
    hwnd_ = hwnd;

    //-----------------------
    // Create Vulkan instance
    uint apiVersion;
    if (VKR_FAILED(vkEnumerateInstanceVersion(&apiVersion)))
        return R_FAIL;

    if (apiVersion < VK_VERSION)
    {
        Log(LogLevel::Error, "Vulkan version too low, %d, expected %d", apiVersion, VK_VERSION);
        return R_FAIL;
    }

    VkApplicationInfo appInfo{};
    appInfo.sType               = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName    = "VkRenderer";
    appInfo.applicationVersion  = VK_MAKE_VERSION(0, 0, 1);
    appInfo.pEngineName         = "VkRenderer";
    appInfo.engineVersion       = VK_MAKE_VERSION(0, 0, 1);
    appInfo.apiVersion          = VK_VERSION;

    VkInstanceCreateInfo instInfo{};
    instInfo.sType              = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    instInfo.pApplicationInfo   = &appInfo;

    #if VKR_DEBUG
        const char* validationLayers[] = {
            "VK_LAYER_LUNARG_standard_validation",
        };

        instInfo.enabledLayerCount      = (uint)VKR_ARR_LEN(validationLayers);
        instInfo.ppEnabledLayerNames    = validationLayers;
    #endif

    const char* instanceExt[] = {
        "VK_KHR_win32_surface",
        VK_EXT_DEBUG_REPORT_EXTENSION_NAME,
        VK_KHR_SURFACE_EXTENSION_NAME,
    };

    instInfo.enabledExtensionCount      = VKR_ARR_LEN(instanceExt);
    instInfo.ppEnabledExtensionNames    = instanceExt;

    if (VKR_FAILED(vkCreateInstance(&instInfo, nullptr, &vkInstance_)))
        return R_FAIL;

    #if VKR_DEBUG
        VkDebugReportCallbackCreateInfoEXT createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT;
        createInfo.flags = VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT;
        createInfo.pfnCallback = ValidationCallback;

        if (VKR_FAILED(CreateDebugReportCallbackEXT(vkInstance_, &createInfo, nullptr, &debugReportCallback_)))
            return R_FAIL;
    #endif

    //-----------------------
    // Create surface
    VkWin32SurfaceCreateInfoKHR winSurfaceInfo{};
    winSurfaceInfo.sType        = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
    winSurfaceInfo.hinstance    = hinst;
    winSurfaceInfo.hwnd         = hwnd_;

    if (VKR_FAILED(vkCreateWin32SurfaceKHR(vkInstance_, &winSurfaceInfo, nullptr, &vkSurface_)))
        return R_FAIL;

    //-----------------------
    // Find physical device
    uint physicalDeviceCount = 32;
    static VkPhysicalDevice physicalDevices[32];
    if (VKR_FAILED(vkEnumeratePhysicalDevices(vkInstance_, &physicalDeviceCount, physicalDevices)))
        return R_FAIL;

    vkr_assert(physicalDeviceCount > 0 && physicalDeviceCount < VKR_ARR_LEN(physicalDevices));

    int bestDevice = 0;
    // TODO Actually find the best device and check capabilities here
    vkPhysicalDevice_ = physicalDevices[0];

    //-----------------------
    // Queues
    uint queueCount;
    vkGetPhysicalDeviceQueueFamilyProperties(vkPhysicalDevice_, &queueCount, nullptr);

    auto queueProps = (VkQueueFamilyProperties*) VKR_ALLOCA(queueCount * sizeof(VkQueueFamilyProperties));
    vkGetPhysicalDeviceQueueFamilyProperties(vkPhysicalDevice_, &queueCount, queueProps);

    uint directQueueBits = (VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_COMPUTE_BIT);
    DBG_LOG("Enuerating device queue families, %d found", queueCount);

    for (uint i = 0; i < queueCount; ++i)
    {
        VkBool32 presentSupport{};
        presentSupport = vkGetPhysicalDeviceWin32PresentationSupportKHR(vkPhysicalDevice_, i);
        //VKR_CHECK(vkGetPhysicalDeviceSurfaceSupportKHR(vkPhysicalDevice_, i, vkSurface_, &presentSupport));
        
        #if VKR_DEBUG
            static char buff[512];
            QueueFamiliesToString(queueProps[i].queueFlags, buff);
            Log(LogLevel::Info, "\t%d: count %d, present %d, bits %s", i, queueProps[i].queueCount, presentSupport, buff);
        #endif

        if (directQueueFamilyIdx_ == VKR_INVALID 
            && (queueProps[i].queueFlags & directQueueBits) == directQueueBits
            && queueProps[i].queueCount > 0
            && presentSupport)
        {
            directQueueFamilyIdx_ = i;
        }
    }

    vkr_assert(directQueueFamilyIdx_ != VKR_INVALID);

    VkDeviceQueueCreateInfo queues[1]{};
    float prioritites[1]{ 1.0f };
    queues[0].sType             = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queues[0].queueFamilyIndex  = directQueueFamilyIdx_;
    queues[0].queueCount        = 1;
    queues[0].pQueuePriorities  = prioritites;


    //-----------------------
    // Create Vulkan device
    VkPhysicalDeviceFeatures deviceFeatures = CreateRequiredFeatures();

    const char* deviceExt[] = {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME,
        VK_KHR_TIMELINE_SEMAPHORE_EXTENSION_NAME,
    };

    VkDeviceCreateInfo deviceInfo{};
    deviceInfo.sType                    = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    deviceInfo.queueCreateInfoCount     = 1;
    deviceInfo.pQueueCreateInfos        = queues;
    deviceInfo.enabledLayerCount        = instInfo.enabledLayerCount;
    deviceInfo.ppEnabledLayerNames      = instInfo.ppEnabledLayerNames;
    deviceInfo.enabledExtensionCount    = VKR_ARR_LEN(deviceExt);
    deviceInfo.ppEnabledExtensionNames  = deviceExt;
    deviceInfo.pEnabledFeatures         = &deviceFeatures;
        
    if (VKR_FAILED(vkCreateDevice(vkPhysicalDevice_, &deviceInfo, nullptr, &vkDevice_)))
        return R_FAIL;

    vkGetDeviceQueue(vkDevice_, directQueueFamilyIdx_, 0, &vkDirectQueue_);

    //-----------------------
    // Create swapchain
    VkSurfaceCapabilitiesKHR capabilities;
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(vkPhysicalDevice_, vkSurface_, &capabilities);

    VkSwapchainCreateInfoKHR swapchainInfo{};
    swapchainInfo.sType             = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    swapchainInfo.surface           = vkSurface_;
    swapchainInfo.minImageCount     = 2;
    swapchainInfo.imageFormat       = swapChainFormat_ = VK_FORMAT_R8G8B8A8_SRGB;
    swapchainInfo.imageColorSpace   = VK_COLORSPACE_SRGB_NONLINEAR_KHR;
    swapchainInfo.imageExtent       = VkExtent2D{ width_, height_ };
    swapchainInfo.imageArrayLayers  = 1; // Non-stereoscopic view
    swapchainInfo.imageUsage        = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT; // VK_IMAGE_USAGE_TRANSFER_DST_BIT if not drawing directly to images
    swapchainInfo.imageSharingMode  = VK_SHARING_MODE_EXCLUSIVE; // We assume the same queue will draw and present
    swapchainInfo.preTransform      = capabilities.currentTransform;
    swapchainInfo.compositeAlpha    = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    swapchainInfo.presentMode       = VK_PRESENT_MODE_IMMEDIATE_KHR; //VK_PRESENT_MODE_FIFO_RELAXED_KHR;
    swapchainInfo.clipped           = VK_FALSE; // Just to be safe VK_TRUE if we know we will never read the buffers back

    if (VKR_FAILED(vkCreateSwapchainKHR(vkDevice_, &swapchainInfo, nullptr, &vkSwapchain_)))
        return R_FAIL;

    //-----------------------
    // Create semaphores
    #if defined(VKR_USE_TIMELINE_SEMAPHORES)
        VkSemaphoreTypeCreateInfo dqSemaphoreTypeInfo{};
        dqSemaphoreTypeInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_TYPE_CREATE_INFO;
        dqSemaphoreTypeInfo.semaphoreType = VK_SEMAPHORE_TYPE_TIMELINE;
        dqSemaphoreTypeInfo.initialValue = 0;

        VkSemaphoreCreateInfo dqSemaphoreInfo{};
        dqSemaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
        dqSemaphoreInfo.pNext = &dqSemaphoreTypeInfo;

        vkCreateSemaphore(vkDevice_, &dqSemaphoreInfo, nullptr, &directQueueSemaphore_);
    #endif

    //-----------------------
    // Create fences
    VkFenceCreateInfo fenceInfo{};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    for (int i = 0; i < BB_IMG_COUNT; ++i)
    {
        if (VKR_FAILED(vkCreateFence(vkDevice_, &fenceInfo, nullptr, &directQueueFences_[i])))
            return R_FAIL;
    }

    //-----------------------
    // Get current back buffer
    uint swapchainImageCount{};
    if (VKR_FAILED(vkGetSwapchainImagesKHR(vkDevice_, vkSwapchain_, &swapchainImageCount, nullptr)))
        return R_FAIL;

    vkr_assert(swapchainImageCount < 10);
    DBG_LOG("Swapchain image count: %d", swapchainImageCount);

    if (VKR_FAILED(vkGetSwapchainImagesKHR(vkDevice_, vkSwapchain_, &swapchainImageCount, bbImages_)))
        return R_FAIL;

    if (VKR_FAILED(vkAcquireNextImageKHR(vkDevice_, vkSwapchain_, (uint64)-1, nullptr, nullptr, &currentBBIdx_)))
        return R_FAIL;

    //-----------------------
    // Create swapchain views
    for (int i = 0; i < BB_IMG_COUNT; ++i)
    {
        VkImageSubresourceRange subresource{};
        subresource.aspectMask      = VK_IMAGE_ASPECT_COLOR_BIT;
        subresource.baseMipLevel    = 0;
        subresource.levelCount      = 1;
        subresource.baseArrayLayer  = 0;
        subresource.layerCount      = 1;

        VkImageViewCreateInfo imageViewInfo{};
        imageViewInfo.sType             = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        imageViewInfo.image             = bbImages_[i];
        imageViewInfo.viewType          = VK_IMAGE_VIEW_TYPE_2D;
        imageViewInfo.format            = swapChainFormat_;
        imageViewInfo.components        = VkComponentMapping { VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY };
        imageViewInfo.subresourceRange  = subresource;
        
        vkCreateImageView(vkDevice_, &imageViewInfo, nullptr, &bbViews_[i]);
    }


    //-----------------------
    // Create command pools and buffers
    VkCommandPoolCreateInfo directPoolInfo{};
    directPoolInfo.sType            = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    directPoolInfo.flags            = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    directPoolInfo.queueFamilyIndex = directQueueFamilyIdx_;

    if (VKR_FAILED(vkCreateCommandPool(vkDevice_, &directPoolInfo, nullptr, &directCmdPool_)))
        return R_FAIL;

    VkCommandBufferAllocateInfo cmdBufferInfo{};
    cmdBufferInfo.sType                 = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    cmdBufferInfo.commandPool           = directCmdPool_;
    cmdBufferInfo.level                 = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    cmdBufferInfo.commandBufferCount    = BB_IMG_COUNT;

    if (VKR_FAILED(vkAllocateCommandBuffers(vkDevice_, &cmdBufferInfo, directCmdBuffers_)))
        return R_FAIL;

    return R_OK;
}

//------------------------------------------------------------------------------
void Render::Update()
{
    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    VKR_CHECK(vkBeginCommandBuffer(directCmdBuffers_[currentBBIdx_], &beginInfo));

    //-------------------
    // Create render pass
    VkRenderPass renderPass{};
    {
        VkAttachmentDescription colorAttachment{};
        colorAttachment.format          = swapChainFormat_;
        colorAttachment.samples         = VK_SAMPLE_COUNT_1_BIT;
        colorAttachment.loadOp          = VK_ATTACHMENT_LOAD_OP_CLEAR;
        colorAttachment.storeOp         = VK_ATTACHMENT_STORE_OP_STORE;
        colorAttachment.stencilLoadOp   = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        colorAttachment.stencilStoreOp  = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        colorAttachment.initialLayout   = VK_IMAGE_LAYOUT_UNDEFINED;
        colorAttachment.finalLayout     = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

        VkAttachmentReference colorAttachmentRef{};
        colorAttachmentRef.attachment   = 0;
        colorAttachmentRef.layout       = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

        /*VkAttachmentDescription depthAttachment{};
        depthAttachment.format          = VK_FORMAT_D16_UNORM;
        depthAttachment.samples         = VK_SAMPLE_COUNT_1_BIT;
        depthAttachment.loadOp          = VK_ATTACHMENT_LOAD_OP_CLEAR;
        depthAttachment.storeOp         = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        depthAttachment.stencilLoadOp   = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        depthAttachment.stencilStoreOp  = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        depthAttachment.initialLayout   = VK_IMAGE_LAYOUT_UNDEFINED;
        depthAttachment.finalLayout     = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

        VkAttachmentReference depthAttachmentRef{};
        depthAttachmentRef.attachment   = 1;
        depthAttachmentRef.layout       = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;*/

        VkSubpassDescription subpass{};
        subpass.pipelineBindPoint       = VK_PIPELINE_BIND_POINT_GRAPHICS;
        subpass.colorAttachmentCount    = 1;
        subpass.pColorAttachments       = &colorAttachmentRef;
        //subpass.pDepthStencilAttachment = &depthAttachmentRef;

        /*VkSubpassDependency dependency{};
        dependency.srcSubpass       = VK_SUBPASS_EXTERNAL;
        dependency.dstSubpass       = 0;
        dependency.srcStageMask     = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dependency.srcAccessMask    = 0;
        dependency.dstStageMask     = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dependency.dstAccessMask    = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;*/

        VkAttachmentDescription attachments[1] = { colorAttachment };
        VkRenderPassCreateInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        renderPassInfo.attachmentCount = VKR_ARR_LEN(attachments);
        renderPassInfo.pAttachments = attachments;
        renderPassInfo.subpassCount = 1;
        renderPassInfo.pSubpasses = &subpass;
        //renderPassInfo.dependencyCount = 1;
        //renderPassInfo.pDependencies = &dependency;
    
        VKR_CHECK(vkCreateRenderPass(vkDevice_, &renderPassInfo, nullptr, &renderPass));
    }


    //-------------------
    // Create framebuffer
    VkFramebuffer frameBuffer{};
    {
        VkFramebufferCreateInfo framebufferInfo{};
        framebufferInfo.sType           = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        framebufferInfo.renderPass      = renderPass;
        framebufferInfo.attachmentCount = 1;
        framebufferInfo.pAttachments    = &bbViews_[currentBBIdx_];
        framebufferInfo.width           = width_;
        framebufferInfo.height          = height_;
        framebufferInfo.layers          = 1;
        
        VKR_CHECK(vkCreateFramebuffer(vkDevice_, &framebufferInfo, nullptr, &frameBuffer));
    }

    VkClearValue clearVal{ VkClearColorValue { 0.2f, 0.6f, 0.2f, 1.0f } };

    VkRenderPassBeginInfo renderPassBeginInfo{};
    renderPassBeginInfo.sType           = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassBeginInfo.renderPass      = renderPass;
    renderPassBeginInfo.framebuffer     = frameBuffer;
    renderPassBeginInfo.renderArea      = VkRect2D { VkOffset2D { 0, 0 }, VkExtent2D { width_, height_ } };
    renderPassBeginInfo.clearValueCount = 1;
    renderPassBeginInfo.pClearValues    = &clearVal;

    vkCmdBeginRenderPass(directCmdBuffers_[currentBBIdx_], &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

    VkClearAttachment clearAtt{};
    clearAtt.aspectMask         = VK_IMAGE_ASPECT_COLOR_BIT;
    clearAtt.colorAttachment    = 0;
    clearAtt.clearValue         = clearVal;

    VkClearRect clearRect{ VkRect2D { VkOffset2D { 0, 0 }, VkExtent2D { width_, height_ } }, 0, 1 };

    vkCmdClearAttachments(directCmdBuffers_[currentBBIdx_], 1, &clearAtt, 1, &clearRect);

    vkCmdEndRenderPass(directCmdBuffers_[currentBBIdx_]);

    vkEndCommandBuffer(directCmdBuffers_[currentBBIdx_]);

    //-------------------
    // Submit
    #if defined(VKR_USE_TIMELINE_SEMAPHORES)
        uint64 signalValue = semaphoreValues[currentBBIdx_];

        VkTimelineSemaphoreSubmitInfo timelineInfo{};
        timelineInfo.sType                      = VK_STRUCTURE_TYPE_TIMELINE_SEMAPHORE_SUBMIT_INFO;
        timelineInfo.signalSemaphoreValueCount  = 1;
        timelineInfo.pSignalSemaphoreValues     = &signalValue;

        VkSubmitInfo submit{};
        submit.sType                = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submit.pNext                = &timelineInfo;
        submit.waitSemaphoreCount   = 1;
        submit.pWaitSemaphores      = &directQueueSemaphore_;
        submit.commandBufferCount   = 1;
        submit.pCommandBuffers      = &directCmdBuffers_[currentBBIdx_];

        vkQueueSubmit(vkDirectQueue_, 1, &submit, VK_NULL_HANDLE);

        VKR_CHECK(vkAcquireNextImageKHR(vkDevice_, vkSwapchain_, (uint64)-1, nullptr, nullptr, &currentBBIdx_));

        uint64 waitValue = semaphoreValues[currentBBIdx_];
        semaphoreValues[currentBBIdx_] = signalValue + 1;

        uint64_t value{};
        VKR_CHECK(vkGetSemaphoreCounterValue(vkDevice_, directQueueSemaphore_, &value));
        if (value < waitValue)
        {
            VkSemaphoreWaitInfo waitInfo;
            waitInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_WAIT_INFO;
            waitInfo.semaphoreCount = 1;
            waitInfo.pSemaphores = &directQueueSemaphore_;
            waitInfo.pValues = &waitValue;

            VKR_CHECK(vkWaitSemaphores(vkDevice_, &waitInfo, (uint64)-1));
        }
    #else
        VkSubmitInfo submit{};
        submit.sType                = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submit.commandBufferCount   = 1;
        submit.pCommandBuffers      = &directCmdBuffers_[currentBBIdx_];

        VKR_CHECK(vkQueueSubmit(vkDirectQueue_, 1, &submit, directQueueFences_[currentBBIdx_]));
        
        VkPresentInfoKHR presentInfo{};
        presentInfo.sType           = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        presentInfo.swapchainCount  = 1;
        presentInfo.pSwapchains     = &vkSwapchain_;
        presentInfo.pImageIndices   = &currentBBIdx_;

        VKR_CHECK(vkQueuePresentKHR(vkDirectQueue_, &presentInfo));

        VKR_CHECK(vkAcquireNextImageKHR(vkDevice_, vkSwapchain_, (uint64)-1, nullptr, nullptr, &currentBBIdx_));

        VkResult fenceVal = vkGetFenceStatus(vkDevice_, directQueueFences_[currentBBIdx_]);
        if (fenceVal == VK_ERROR_DEVICE_LOST)
            VKR_CHECK(fenceVal);

        if (fenceVal == VK_NOT_READY)
        {
            VKR_CHECK(vkWaitForFences(vkDevice_, 1, &directQueueFences_[currentBBIdx_], VK_TRUE, (uint64)-1));
        }

        VKR_CHECK(vkResetFences(vkDevice_, 1, &directQueueFences_[currentBBIdx_]));

    #endif
}

}
