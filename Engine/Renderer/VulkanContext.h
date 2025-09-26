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
        std::vector<const char*> getGLFWRequiredExtension() const;

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
    };
}
