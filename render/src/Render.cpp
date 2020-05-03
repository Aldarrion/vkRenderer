#include "Render.h"

#include "Logging.h"
#include "vkr_Assert.h"

#include "vulkan/vulkan_win32.h"
#include "vkr_Shaderc.h"

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

//------------------------------------------------------------------------------
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
    Log(LogLevel::Error, "VALIDATION %s: %s", layerPrefix, msg);

    // Return true only when we want to test the VL themselves
    return VK_FALSE;
}

RESULT Render::CompileShader(const char* file, ShaderType type, Shader& shader)
{
    Log(LogLevel::Info, "---- Compiling shader %s ----", file);

    FILE* f = fopen(file, "r");
    if (!f)
    {
        Log(LogLevel::Error, "Failed to open the file");
        return R_FAIL;
    }

    fseek(f , 0 , SEEK_END);
    auto size = ftell(f);
    rewind(f);

    char* buffer = (char*)malloc(size);
    if (!buffer)
    {
        free(buffer);
        Log(LogLevel::Error, "Failed to alloc space for shader file");
        return R_FAIL;
    }

    auto readRes = fread(buffer, 1, size, f);
    auto eof = feof(f);
    if (readRes != size && !eof)
    {
        Log(LogLevel::Error, "Failed to read the shader file, error %d", ferror(f));
        free(buffer);
        fclose(f);
        return R_FAIL;
    }
    fclose(f);

    shaderc_shader_kind kind = type == ShaderType::Vertex ? shaderc_glsl_vertex_shader : shaderc_glsl_fragment_shader;

    shaderc_compilation_result_t result = shaderc_compile_into_spv(shadercCompiler_, buffer, readRes, kind, file, "main", nullptr);
    free(buffer);

    shaderc_compilation_status status = shaderc_result_get_compilation_status(result);
    
    const char* msg = shaderc_result_get_error_message(result);
    auto warningCount = shaderc_result_get_num_warnings(result);
    auto errorCount = shaderc_result_get_num_errors(result);
    LogLevel resultLevel = LogLevel::Info;
    if (errorCount > 0)
        resultLevel = LogLevel::Error;
    else if (warningCount > 0)
        resultLevel = LogLevel::Warning;

    if (msg && *msg)
        Log(resultLevel, msg);
    Log(resultLevel, "Done with %d errors, %d warnings", errorCount, warningCount);


    if (status != shaderc_compilation_status_success)
    {
        shaderc_result_release(result);
        return R_FAIL;
    }

    shader.Code = (uint*)shaderc_result_get_bytes(result);
    shader.Length = shaderc_result_get_length(result);

    //shaderc_result_release(result);

    return R_OK;
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

    //-----------------------
    // Compile shaders

    shadercCompiler_ = shaderc_compiler_initialize();
    if (!shadercCompiler_)
    {
        Log(LogLevel::Error, "Could not create shaderc compiler");
        return R_FAIL;
    }

    if (CompileShader("../shaders/triangle.vert", ShaderType::Vertex, triangleVert_) != R_OK)
        return R_FAIL;
    if (CompileShader("../shaders/triangle.frag", ShaderType::Fragment, triangleFrag_) != R_OK)
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
    // Before frame start
    //-------------------

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
    // Pipeline layout
    // Pipeline layout is empty for now
    VkPipelineLayout pipelineLayout{};
    VkPipelineLayoutCreateInfo plLayoutInfo{};
    plLayoutInfo.sType          = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    VKR_CHECK(vkCreatePipelineLayout(vkDevice_, &plLayoutInfo, nullptr, &pipelineLayout));

    //-------------------
    // Create Pipeline
    
    VkPipelineShaderStageCreateInfo stages[2]{};
    {
        VkShaderModule vertexShader;
        VkShaderModuleCreateInfo vertInfo{};
        vertInfo.sType    = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        vertInfo.codeSize = triangleVert_.Length;
        vertInfo.pCode    = triangleVert_.Code;

        VKR_CHECK(vkCreateShaderModule(vkDevice_, &vertInfo, nullptr, &vertexShader));

        VkShaderModule fragmentShader;
        VkShaderModuleCreateInfo fragInfo{};
        fragInfo.sType    = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        fragInfo.codeSize = triangleFrag_.Length;
        fragInfo.pCode    = triangleFrag_.Code;

        VKR_CHECK(vkCreateShaderModule(vkDevice_, &fragInfo, nullptr, &fragmentShader));

        stages[0].sType     = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        stages[0].stage     = VK_SHADER_STAGE_VERTEX_BIT;
        stages[0].module    = vertexShader;
        stages[0].pName     = "main";
    
        stages[1].sType     = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        stages[1].stage     = VK_SHADER_STAGE_FRAGMENT_BIT;
        stages[1].module    = fragmentShader;
        stages[1].pName     = "main";
    }

    VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
    {
        vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    }

    VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
    {
        inputAssembly.sType                     = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        inputAssembly.topology                  = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        inputAssembly.primitiveRestartEnable    = VK_FALSE;
    }

    VkPipelineViewportStateCreateInfo viewportState{};
    VkViewport viewport{};
    VkRect2D scissor{};
    {
        viewport.x          = 0.0f;
        viewport.y          = 0.0f;
        viewport.width      = (float)width_;
        viewport.height     = (float)height_;
        viewport.minDepth   = 0.0f;
        viewport.maxDepth   = 1.0f;

        scissor.offset = { 0, 0 };
        scissor.extent = VkExtent2D{ width_, height_ };

        viewportState.sType         = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        viewportState.viewportCount = 1;
        viewportState.pViewports    = &viewport;
        viewportState.scissorCount  = 1;
        viewportState.pScissors     = &scissor;
    }

    VkPipelineRasterizationStateCreateInfo rasterizer{};
    {
        rasterizer.sType                    = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        rasterizer.depthClampEnable         = VK_FALSE; // Is geometry beyond near-far clamped to near far? (if not, it's discarded)
        rasterizer.rasterizerDiscardEnable  = VK_FALSE; // If true everything is discarded.
        rasterizer.polygonMode              = VK_POLYGON_MODE_FILL; // Using any other mode requires enabling GPU feature
        //rasterizer.polygonMode              = VK_POLYGON_MODE_LINE;
        rasterizer.lineWidth                = 1.0f;
        rasterizer.cullMode                 = VK_CULL_MODE_NONE;
        rasterizer.frontFace                = VK_FRONT_FACE_CLOCKWISE;
    }

    VkPipelineMultisampleStateCreateInfo multisampling{};
    {
        multisampling.sType                 = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        multisampling.rasterizationSamples  = VK_SAMPLE_COUNT_1_BIT;
    }

    VkPipelineDepthStencilStateCreateInfo depthStencil{};
    {
        depthStencil.sType              = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
        depthStencil.depthTestEnable    = VK_TRUE;
        depthStencil.depthWriteEnable   = VK_TRUE;
        depthStencil.depthCompareOp     = VK_COMPARE_OP_LESS;
    }

    VkPipelineColorBlendAttachmentState colorBlendAttachment{};
    VkPipelineColorBlendStateCreateInfo colorBlending{};
    {
        colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;

        colorBlending.sType             = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        colorBlending.attachmentCount   = 1;
        colorBlending.pAttachments      = &colorBlendAttachment;
    }

    VkPipeline pipeline{};
    VkGraphicsPipelineCreateInfo plInfo{};
    plInfo.sType                = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    plInfo.stageCount           = 2;
    plInfo.pStages              = stages;
    plInfo.pVertexInputState    = &vertexInputInfo;
    plInfo.pInputAssemblyState  = &inputAssembly;
    plInfo.pViewportState       = &viewportState;
    plInfo.pRasterizationState  = &rasterizer;
    plInfo.pMultisampleState    = &multisampling;
    plInfo.pDepthStencilState   = &depthStencil;
    plInfo.pColorBlendState     = &colorBlending;
    plInfo.layout               = pipelineLayout;
    plInfo.renderPass           = renderPass;

    VKR_CHECK(vkCreateGraphicsPipelines(vkDevice_, VK_NULL_HANDLE, 1, &plInfo, nullptr, &pipeline));

    // End Create Pipeline
    //-------------------

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

    //-------------------
    // Render pass commands
    
    vkCmdBindPipeline(directCmdBuffers_[currentBBIdx_], VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);

    vkCmdDraw(directCmdBuffers_[currentBBIdx_], 3, 1, 0, 0);

    // End render pass
    //-------------------
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
