#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <vulkan/vulkan_raii.hpp>

#ifdef NDEBUG
constexpr bool enableValidationLayers = false;
#else
constexpr bool enableValidationLayers = true;
#endif

constexpr int MAX_FRAMES_IN_FLIGHT = 2;

constexpr int IMAGE_ARRAY_LAYERS = 1;

inline std::vector validationLayers = {
    "VK_LAYER_KHRONOS_validation",

};

inline std::vector deviceExtensions = {
    vk::KHRSwapchainExtensionName,
    vk::KHRSpirv14ExtensionName,
    vk::KHRSynchronization2ExtensionName,
    vk::KHRCreateRenderpass2ExtensionName
};

namespace Renderer
{
    class VulkanContext
    {
    public:
        VulkanContext() = default;
        ~VulkanContext() = default;

        /**
         * Initializes resources such as: instances, devices, queues, buffers, etc.
         */
        void InitializeVulkan(GLFWwindow* window);

    private:
        vk::raii::Context _context;
        vk::raii::Instance _instance = VK_NULL_HANDLE;

        vk::raii::DebugUtilsMessengerEXT debug_messenger = VK_NULL_HANDLE;

        vk::raii::PhysicalDevice _physical_device = VK_NULL_HANDLE;
        vk::PhysicalDeviceFeatures _device_features;

        vk::raii::Device _device = VK_NULL_HANDLE;

        vk::raii::SurfaceKHR _surface = VK_NULL_HANDLE;

        vk::raii::Queue _graphics_queue = VK_NULL_HANDLE;
        vk::raii::Queue _present_queue = VK_NULL_HANDLE;

        uint32_t _graphics_family_index = UINT32_MAX;
        uint32_t _present_family_index = UINT32_MAX;

        vk::raii::SwapchainKHR _swapChain = VK_NULL_HANDLE;
        vk::Extent2D _swapChainExtent;
        std::vector<vk::Image> swapChainImages;
        std::vector<vk::raii::ImageView> swapChainImageViews;
        vk::Format swapChainImageFormat = vk::Format::eUndefined;

        /**
         * Creates Vulkan instance
         */
        void createInstance();

        /**
         * Sets custom callback to the debug messenger
         */
        void setupDebugMessenger();

        /**
         * Checks if the GLFW required extensions are available, and returns them.
         *
         * @return Extensions vector
         */
        [[nodiscard]] std::vector<const char*> getGLFWRequiredExtension() const;

        static VKAPI_ATTR vk::Bool32 VKAPI_CALL debugCallback(vk::DebugUtilsMessageSeverityFlagBitsEXT severity,
                                                              vk::DebugUtilsMessageTypeFlagsEXT type,
                                                              const vk::DebugUtilsMessengerCallbackDataEXT*
                                                              pCallbackData,
                                                              void*);

        /**
         * Picks the best GPU with point system
         */
        void pickPhysicalDevice();

        /**
         * Finds the best queue family (with both present and graphics queue)
         */
        void findBestQueueFamilyIndexes();

        /**
         * This will create a Vulkan Surface
         * @param window GLFW handle of the window
         */
        void createSurface(GLFWwindow* window);

        /**
         * Creates logical device
         */
        void createLogicalDevice();

        /**
         * Creates graphical and present queue
         */
        void createQueues();

        /**
         *  This is a helper function
         *  Picks the swap surface format for the swap chain
         *
         * @param surfaceFormats Available surface formats from physical device
         * @return Chosen swap surface format
         */
        static vk::SurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<vk::SurfaceFormatKHR>& surfaceFormats);

        /**
         * This is a helper function
         * Picks the present mode
         *
         * @param presentModes Available present modes
         * @return Chosen present mode
         */
        static vk::PresentModeKHR chooseSwapPresentMode(const std::vector<vk::PresentModeKHR>& presentModes);
        /**
         * This is a helper function
         * Returns swap extend in 2 dimension (width, height)
         *
         * @param surface_capabilities Surface capabilities
         * @param window
         * @return Chosen swap extent
         */
        static vk::Extent2D chooseSwapExtent(const vk::SurfaceCapabilitiesKHR& surface_capabilities, GLFWwindow* window);

        /**
         * Creates swap chain
         */
        void createSwapChain(GLFWwindow* window);

        /**
         * Create image views
         */
        void createImageViews();

        /**
         * Creates graphical pipeline
         */
        void createGraphicsPipeline();

        /**
         * Creates command pools
         */
        void createCommandPool();

        /**
         * Creates Command buffers
         */
        void createCommandBuffer();

        /**
         * Creates sync objects such as: semaphores, and fences (only for now)
         */
        void createSyncObjects();
    };
}
