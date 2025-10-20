#pragma once

#include <string>
#include <vulkan/vulkan_raii.hpp>

#include <Core/Window.h>

#define VULKAN_API_VERSION vk::ApiVersion13

#ifdef NDEBUG
constexpr bool ENABLE_VALIDATION_LAYERS = false;
#else
constexpr bool ENABLE_VALIDATION_LAYERS = true;
#endif

namespace Renderer
{
	/**
	 * Vulkan Instance Wrapper
	 */
	class VulkanInstance
	{
	public:
		VulkanInstance() = default;
		~VulkanInstance() = default;

		VulkanInstance(const VulkanInstance&) = delete;
		VulkanInstance& operator=(const VulkanInstance&) = delete;

		/**
		 * Initializes Vulkan instance, Vulkan surface and sets layers if it's the debug build.
		 *
		 * @param appName Application name
		 * @param appVersion Application version
		 */
		void initialize(const char* appName, uint32_t appVersion, GLFWwindow* window);

		/**
		 * Cleans up the resources.
		 * It's empty since RAII handles it.
		 */
		void cleanup() = delete;

		/**
		 * @return Vulkan Instance
		 */
		vk::raii::Instance& getInstance() noexcept { return m_instance; }

		/**
		 * @return Vulkan Surface
		 */
		vk::raii::SurfaceKHR& getSurface() noexcept { return m_surface; }

	private:
		/**
		 * Creates a Vulkan instance.
		 *
		 * @param appName Application name
		 * @param appVersion Application version
		 */
		void createInstance(const char* appName, uint32_t appVersion);

		/**
		 * This will check if layers are supported.
		 * If one of them isn't supported, this will throw a runtime_error.
		 *
		 * @param requiredLayers Required instance layers
		 */
		void checkLayers(const std::vector<char const*>& requiredLayers) const;

		/**
		 * This both checks and returns GLFW required extensions.
		 * If one of them isn't supported, this will throw a runtime_error.
		 *
		 * @return GLFW required extensions
		 */
		[[nodiscard]]
		std::vector<const char*> getGLFWRequiredExtensions() const;

		/**
		 * This sets debug callback for the VK_LAYER_KHRONOS_validation layer.
		 */
		void setupDebugMessenger();

		static vk::Bool32 debugCallback(
			vk::DebugUtilsMessageSeverityFlagBitsEXT severity, vk::DebugUtilsMessageTypeFlagsEXT type,
			const vk::DebugUtilsMessengerCallbackDataEXT* pCallbackData, void* data
		);

		void createSurface(GLFWwindow* window);

		vk::raii::Context m_context;
		vk::raii::Instance m_instance = VK_NULL_HANDLE;

		vk::raii::SurfaceKHR m_surface = VK_NULL_HANDLE;

		vk::raii::DebugUtilsMessengerEXT m_debug_messenger = VK_NULL_HANDLE;

		const std::vector<const char*> m_validationLayers = {
			"VK_LAYER_KHRONOS_validation"
		};
	};
}
