#include <algorithm>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <exception>
#include <format>
#include <iostream>
#include <limits>
#include <optional>
#include <set>
#include <stdexcept>
#include <vector>

#include <vulkan/vulkan.h>
#include <vulkan/vulkan_core.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_vulkan.h>

#include "shader.h"

#define ARRAY_SIZE(a) (sizeof(a) / sizeof(a[0]))
#ifndef UINT32_MAX
#define UINT32_MAX 0xffffffff
#endif

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;

const std::vector<const char*> validationLayers = {
    "VK_LAYER_KHRONOS_validation"
};

const std::vector<const char*> deviceExtensions = {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

struct QueueFamilyIndices {
    std::optional<uint32_t> graphicsFamily;
    std::optional<uint32_t> presentFamily;

    bool isComplete()
    {
        return graphicsFamily.has_value();
    }
};

struct SwapChainSupportDetails {
    VkSurfaceCapabilitiesKHR capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> presentModes;
};

#ifndef NDEBUG
const bool enableValidationLayers = true;
const bool release = false;
#else
const bool enableValidationLayers = false;
const bool debug = false;
#endif // !DEBUG

static void check_vk_result(VkResult err)
{
    if (err == 0)
        return;
    fprintf(stderr, "[vulkan] Error: VkResult = %d\n", err);
    if (err < 0)
        abort();
}

VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger)
{
    auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
    if (func != nullptr) {
        return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
    } else {
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    }
}

auto DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator) -> void
{
    auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
    if (func != nullptr) {
        func(instance, debugMessenger, pAllocator);
    }
}

class Engine {
public:
    auto run() -> void
    {
        initWindow();
        initVulkan();
        // initImgui();
        mainLoop();
        cleanup();
    }

private:
    bool running = true;

    SDL_Window* window = nullptr;

    VkInstance instance;

    VkDebugUtilsMessengerEXT debugMessenger;

    VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;

    VkDevice device;

    VkSurfaceKHR surface;

    VkQueue graphicsQueue;

    VkQueue presentQueue;

    VkSwapchainKHR swapChain;

    std::vector<VkImage> swapChainImages;

    std::vector<VkImageView> swapChainImageViews;

    VkFormat swapChainImageFormat;

    VkExtent2D swapChainExtent;

    auto initWindow() -> void
    {
        if (SDL_Init(SDL_INIT_VIDEO) != 0) {
            throw std::runtime_error(std::format("SDL have not been initialized! Error: {}\n", SDL_GetError()));
            return;
        }

        if (SDL_Vulkan_LoadLibrary(nullptr) != 0) {
            throw std::runtime_error(std::format("SDL could not loaded vulkan! Error: {}\n", SDL_GetError()));
            SDL_Quit();
            return;
        }

        window = SDL_CreateWindow("VKSetup", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_VULKAN);

        if (window == nullptr) {
            throw std::runtime_error(std::format("SDL could not create a window! Error: {}\n", SDL_GetError()));
            SDL_Quit();
        }
    }

    auto initVulkan() -> void
    {
        createInstance();
        setupDebugMessenger();
        createSurface();
        pickPhysicalDevice();
        createLogicalDevice();
        createSwapChain();
        createImageViews();
        createGraphicsPipelines();
    }

    auto initImgui() -> void
    {

#ifdef IMGUISUPER
        // Setup Dear ImGui context
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad; // Enable Gamepad Controls

        // Setup Dear ImGui style
        ImGui::StyleColorsDark();

        // Setup Platform/Renderer backends
        ImGui_ImplSDL2_InitForVulkan(window);
        ImGui_ImplVulkan_InitInfo init_info = {};
        init_info.Instance = instance;
        init_info.PhysicalDevice = physicalDevice;
        init_info.Device = device;
        init_info.QueueFamily = findQueueFamilies(physicalDevice).presentFamily.value();
        init_info.Queue = presentQueue;
        init_info.PipelineCache = VK_NULL_HANDLE;
        init_info.DescriptorPool = VK_NULL_HANDLE;
        init_info.Subpass = 0;
        init_info.MinImageCount = 2;
        init_info.ImageCount = 2;
        init_info.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
        init_info.Allocator = nullptr;
        init_info.CheckVkResultFn = check_vk_result;
        ImGui_ImplVulkan_Init(&init_info);
#endif
    }

    auto mainLoop() -> void
    {
        // Our state for demo IMGUI, not used and will be extracted into a bigger class
        bool show_demo_window = true;

        while (running) {

            SDL_Event event;

            while (SDL_PollEvent(&event)) {

                if (event.type == SDL_QUIT) {
                    running = false;
                }
            }
            /*
            // Start the Dear ImGui frame
            ImGui_ImplVulkan_NewFrame();
            ImGui_ImplSDL2_NewFrame();
            ImGui::NewFrame();

            if (show_demo_window)
            ImGui::ShowDemoWindow(&show_demo_window);
            */
        }
    }

    auto cleanup() -> void
    {

        for (auto imageView : swapChainImageViews) {
            vkDestroyImageView(device, imageView, nullptr);
        }

        if (enableValidationLayers) {
            DestroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr);
        }

        vkDestroySwapchainKHR(device, swapChain, nullptr);

        vkDestroyDevice(device, nullptr);

        vkDestroySurfaceKHR(instance, surface, nullptr);

        vkDestroyInstance(instance, nullptr);

        SDL_DestroyWindow(window);

        SDL_Vulkan_UnloadLibrary();

        SDL_Quit();
    }

    auto createInstance() -> void
    {
        if (enableValidationLayers && !checkValidationLayersSupport()) {
            throw std::runtime_error("Validation layers requested, but not available!");
        }

        VkApplicationInfo appInfo {};

        appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        appInfo.pApplicationName = "VKSetup";
        appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.pEngineName = "No Engine";
        appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.apiVersion = VK_API_VERSION_1_0;

        VkInstanceCreateInfo createInfo {};

        createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        createInfo.pApplicationInfo = &appInfo;

        unsigned int extensionCount = 0;

        SDL_Vulkan_GetInstanceExtensions(window, &extensionCount, nullptr);

        std::vector<const char*> extensionNames(extensionCount);
        SDL_Vulkan_GetInstanceExtensions(window, &extensionCount, extensionNames.data());

        createInfo.enabledExtensionCount = extensionNames.size();
        createInfo.ppEnabledExtensionNames = extensionNames.data();

        VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo {};

        if (enableValidationLayers) {
            createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
            createInfo.ppEnabledLayerNames = validationLayers.data();

            populateDebugMessengerCreateInfo(debugCreateInfo);
            createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debugCreateInfo;
        } else {
            createInfo.enabledLayerCount = 0;

            createInfo.pNext = nullptr;
        }

        if (vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create Vulkan instance!");
        }
    }

    auto createGraphicsPipelines() -> void
    {
        std::string vertSource, fragSource;

        vertSource = Shader::getFileContent("../shaders/vert_test.glsl");
        fragSource = Shader::getFileContent("../shaders/frag_test.glsl");

        auto vertShaderCode = Shader::compileShader("vert_test", shaderc_glsl_vertex_shader, vertSource, release);
        auto fragShaderCode = Shader::compileShader("frag_test", shaderc_glsl_fragment_shader, fragSource, release);

        VkShaderModule vertShaderModule = Shader::createShaderModule(device, vertShaderCode);
        VkShaderModule fragShaderModule = Shader::createShaderModule(device, fragShaderCode);

        vkDestroyShaderModule(device, vertShaderModule, nullptr);
        vkDestroyShaderModule(device, fragShaderModule, nullptr);

        SDL_Log("%lu", sizeof(VkDevice));
    }

    auto createSurface() -> void
    {
        // Check is initializion is correct, these statements may not be needed
        assert(window != nullptr);
        assert(instance != nullptr);

        if (!SDL_Vulkan_CreateSurface(window, instance, &surface)) {
            throw std::runtime_error("failed to create window surface!");
        }
    }

    auto createImageViews() -> void
    {
        swapChainImageViews.resize(swapChainImages.size());

        for (size_t i = 0; i < swapChainImages.size(); i++) {
            VkImageViewCreateInfo createInfo {};

            createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
            createInfo.image = swapChainImages[i];
            createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
            createInfo.format = swapChainImageFormat;

            createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

            createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            createInfo.subresourceRange.baseMipLevel = 0;
            createInfo.subresourceRange.levelCount = 1;
            createInfo.subresourceRange.baseArrayLayer = 0;
            createInfo.subresourceRange.layerCount = 1;

            if (vkCreateImageView(device, &createInfo, nullptr, &swapChainImageViews[i]) != VK_SUCCESS) {
                throw std::runtime_error("failed to create image views!");
            }
        }
    }

    auto querySwapChainSupport(VkPhysicalDevice device) -> SwapChainSupportDetails
    {
        SwapChainSupportDetails details;

        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &details.capabilities);

        uint32_t formatCount;
        vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, nullptr);

        if (formatCount != 0) {
            details.formats.resize(formatCount);
            vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, details.formats.data());
        }

        uint32_t presentModeCount;
        vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, nullptr);

        if (presentModeCount != 0) {
            details.presentModes.resize(presentModeCount);
            vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, details.presentModes.data());
        }

        return details;
    }

    auto chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats) -> VkSurfaceFormatKHR
    {
        // try to find the ideal choice
        for (const auto& availableFormat : availableFormats) {
            if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLORSPACE_SRGB_NONLINEAR_KHR) {
                return availableFormat;
            }
        }

        // else the first one should be good enough
        return availableFormats[0];
    }

    auto chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes) -> VkPresentModeKHR
    {
        for (const auto& availablePresentMode : availablePresentModes) {
            if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
                return availablePresentMode;
            }
        }

        // default option
        return VK_PRESENT_MODE_FIFO_KHR;
    }

    auto chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities) -> VkExtent2D
    {
        if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
            return capabilities.currentExtent;
        } else {
            int width, height;

            SDL_GetWindowSize(window, &width, &height);

            VkExtent2D actualExtent = {
                static_cast<uint32_t>(width),
                static_cast<uint32_t>(height)
            };

            actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
            actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

            return actualExtent;
        }
    }

    auto findQueueFamilies(VkPhysicalDevice device) -> QueueFamilyIndices
    {
        QueueFamilyIndices indices;
        uint32_t queueFamilyCount = 0;

        vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

        std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

        int i = 0;

        for (const auto& queueFamily : queueFamilies) {
            if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
                indices.graphicsFamily = i;
            }

            VkBool32 presentSupport = false;
            vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentSupport);

            if (presentSupport) {
                indices.presentFamily = i;
            }

            if (indices.isComplete()) {
                break;
            }

            i++;
        }
        return indices;
    }

    auto checkDeviceExtensionSupport(VkPhysicalDevice device) -> bool
    {
        uint32_t extensionCount;
        vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

        std::vector<VkExtensionProperties> availableExtensions(extensionCount);
        vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

        std::set<std::string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());

        for (const auto& extension : availableExtensions) {
            requiredExtensions.erase(extension.extensionName);
        }

        return requiredExtensions.empty();
    }

    auto checkValidationLayersSupport() -> bool
    {
        uint32_t layerCount;
        vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

        std::vector<VkLayerProperties> availableLayers(layerCount);
        vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

        for (const char* layerName : validationLayers) {
            bool layerFound = false;

            for (const auto& layerProperties : availableLayers) {
                if (strcmp(layerName, layerProperties.layerName) == 0) {
                    layerFound = true;
                    break;
                }
            }

            if (!layerFound) {
                return false;
            }
        }

        return true;
    }

    auto populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo) -> void
    {
        createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
        createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
        createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
        createInfo.pfnUserCallback = debugCallback;
    }

    auto setupDebugMessenger() -> void
    {
        if (!enableValidationLayers)
            return;

        VkDebugUtilsMessengerCreateInfoEXT createInfo = {};

        populateDebugMessengerCreateInfo(createInfo);

        if (CreateDebugUtilsMessengerEXT(instance, &createInfo, nullptr, &debugMessenger) != VK_SUCCESS) {
            std::cerr << "failed to set up debug messenger!\n";
        }
    }

    auto createSwapChain() -> void
    {
        SwapChainSupportDetails swapChainSupport = querySwapChainSupport(physicalDevice);

        VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.formats);
        VkPresentModeKHR presentMode = chooseSwapPresentMode(swapChainSupport.presentModes);
        VkExtent2D extent = chooseSwapExtent(swapChainSupport.capabilities);

        uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;

        if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount) {
            imageCount = swapChainSupport.capabilities.maxImageCount;
        }

        VkSwapchainCreateInfoKHR createInfo {};
        createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        createInfo.surface = surface;

        createInfo.minImageCount = imageCount;
        createInfo.imageFormat = surfaceFormat.format;
        createInfo.imageColorSpace = surfaceFormat.colorSpace;
        createInfo.imageExtent = extent;
        createInfo.imageArrayLayers = 1;
        createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

        QueueFamilyIndices indices = findQueueFamilies(physicalDevice);
        uint32_t queueFamilyIndices[] = { indices.graphicsFamily.value(), indices.presentFamily.value() };

        if (indices.graphicsFamily != indices.presentFamily) {
            createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
            createInfo.queueFamilyIndexCount = 2;
            createInfo.pQueueFamilyIndices = queueFamilyIndices;
        } else {
            createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        }

        createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
        createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        createInfo.presentMode = presentMode;
        createInfo.clipped = VK_TRUE;

        createInfo.oldSwapchain = VK_NULL_HANDLE;

        if (vkCreateSwapchainKHR(device, &createInfo, nullptr, &swapChain) != VK_SUCCESS) {
            throw std::runtime_error("failed to create swap chain!");
        }

        vkGetSwapchainImagesKHR(device, swapChain, &imageCount, nullptr);
        swapChainImages.resize(imageCount);
        vkGetSwapchainImagesKHR(device, swapChain, &imageCount, swapChainImages.data());

        swapChainImageFormat = surfaceFormat.format;
        swapChainExtent = extent;
    }

    auto createLogicalDevice() -> void
    {
        QueueFamilyIndices indices = findQueueFamilies(physicalDevice);

        std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
        std::set<uint32_t> uniqueQueueFamilies = { indices.graphicsFamily.value(), indices.presentFamily.value() };

        float queuePriority = 1.0f;

        for (uint32_t queueFamily : uniqueQueueFamilies) {
            VkDeviceQueueCreateInfo queueCreateInfo {};

            queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            queueCreateInfo.queueFamilyIndex = queueFamily;
            queueCreateInfo.queueCount = 1;
            queueCreateInfo.pQueuePriorities = &queuePriority;

            queueCreateInfos.push_back(queueCreateInfo);
        }

        VkPhysicalDeviceFeatures deviceFeatures {};

        VkDeviceCreateInfo createInfo {};

        createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

        createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
        createInfo.pQueueCreateInfos = queueCreateInfos.data();

        createInfo.pEnabledFeatures = &deviceFeatures;

        createInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
        createInfo.ppEnabledExtensionNames = deviceExtensions.data();

        if (enableValidationLayers) {
            createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
            createInfo.ppEnabledLayerNames = validationLayers.data();
        } else {
            createInfo.enabledLayerCount = 0;
        }

        if (vkCreateDevice(physicalDevice, &createInfo, nullptr, &device) != VK_SUCCESS) {
            throw std::runtime_error("failed to create logical device!");
        }

        vkGetDeviceQueue(device, indices.graphicsFamily.value(), 0, &graphicsQueue);
        vkGetDeviceQueue(device, indices.presentFamily.value(), 0, &graphicsQueue);
    }

    auto pickPhysicalDevice() -> void
    {
        uint32_t deviceCount = 0;

        vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);

        if (deviceCount == 0) {
            throw std::runtime_error("failed to find GPUs with Vulkan support!");
        }

        std::vector<VkPhysicalDevice> devices(deviceCount);
        vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());

        for (const auto& device : devices) {
            if (isDeviceSuitable(device)) {
                physicalDevice = device;

                VkPhysicalDeviceProperties deviceProperties;
                vkGetPhysicalDeviceProperties(device, &deviceProperties);

                std::cout << "Running:" << deviceProperties.deviceName << "\n";
                break;
            }
        }

        if (physicalDevice == VK_NULL_HANDLE) {
            throw std::runtime_error("failed to find a suitable GPU!");
        }
    }

    auto isDeviceSuitable(VkPhysicalDevice device) -> bool
    {
        /*
         *  Test based on GPU feature supporting geometic shader (take the first found)

        VkPhysicalDeviceProperties deviceProperties;
        VkPhysicalDeviceFeatures deviceFeatures;
        vkGetPhysicalDeviceProperties(device, &deviceProperties);
        vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

        return deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU && deviceFeatures.geometryShader;

        */

        QueueFamilyIndices indices = findQueueFamilies(device);

        bool extensionsSupported = checkDeviceExtensionSupport(device);

        bool swapChainAdequate = false;

        if (extensionsSupported) {
            SwapChainSupportDetails swapChainSupport = querySwapChainSupport(device);
            swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
        }

        return indices.isComplete() && extensionsSupported && swapChainAdequate;
    }

    // Special callback function used by vulkan for debug purposes
    static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
        VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
        VkDebugUtilsMessageTypeFlagsEXT messageType,
        const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
        void* pUserData)
    {

        std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;

        return VK_FALSE;
    }
};

int main(int argc, char* argv[])
{
    Engine engine;

    try {
        engine.run();
    } catch (const std::exception& e) {
        std::cerr << e.what() << "\n";
        return -1;
    }

    return 0;
}
