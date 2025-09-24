#pragma once

#include "vulkan/vulkan_raii.hpp"

#ifdef NDEBUG
constexpr bool enableValidationLayers = false;
#else
constexpr bool enableValidationLayers = true;
#endif

constexpr int MAX_FRAMES_IN_FLIGHT = 2;

const std::vector validationLayers = {
	"VK_LAYER_KHRONOS_validation"
};

const std::vector deviceExtensions = {
	vk::KHRSwapchainExtensionName,
	vk::KHRSpirv14ExtensionName,
	vk::KHRSynchronization2ExtensionName,
	vk::KHRCreateRenderpass2ExtensionName,
};

namespace Renderer
{
    class VulkanContext
    {
    public:

        VulkanContext() = default;

        /**
         * Initializes resources such as: instances, devices, queues, buffers, etc.
         */
        void InitializeVulkan();

    private:
        vk::raii::Context _context;
        vk::raii::Instance _instance = nullptr;

        /**
         * Creates Vulkan instance
         */
        void createInstance();

        /**
         * Sets custom callback to the debug messanger
         */
        void setupDebugMessanger();

        /**
         * Checks if the GLFW required extensions are available, and returns them.
         *
         * @return Extension vector
         */
        std::vector<const char*> getGLFWRequiredExtension() const;
    };
}
