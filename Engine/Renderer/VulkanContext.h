#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <vulkan/vulkan_raii.hpp>
#include <glm/glm.hpp>

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
	vk::KHRCreateRenderpass2ExtensionName,
	vk::KHRShaderDrawParametersExtensionName,
	vk::KHRDynamicRenderingExtensionName
};

namespace Renderer
{
	struct Vertex
	{
		glm::vec2 pos;
		glm::vec3 color;

		static vk::VertexInputBindingDescription getBindingDescription();
		static std::array<vk::VertexInputAttributeDescription, 2> getAttributeDescriptions();
	};

	struct TimeUBO
	{
		float time;
	};

	class VulkanContext
	{
	public:
		VulkanContext() = default;
		~VulkanContext() = default;

		/**
		 * Initializes resources such as: instances, devices, queues, buffers, etc.
		 */
		void InitializeVulkan(GLFWwindow* window);

		/**
		 * Cleans every resource that requires manual destruction for successful exit
		 */
		void Cleanup();

		/**
		 * Draws a frame (it will be deleted after making decisions)
		 */
		void drawFrame();

		void fillVertices(const std::vector<Vertex>& inVert, const std::vector<uint16_t>& indicies);

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
		std::vector<vk::Image> _swapChainImages;
		std::vector<vk::raii::ImageView> _swapChainImageViews;
		vk::Format _swapChainImageFormat = vk::Format::eUndefined;

		vk::raii::PipelineLayout _pipelineLayout = VK_NULL_HANDLE;
		vk::raii::Pipeline _graphicsPipeline = VK_NULL_HANDLE;

		vk::raii::CommandPool _commandPool = VK_NULL_HANDLE;
		std::vector<vk::raii::CommandBuffer> _commandBuffers;

		std::vector<vk::raii::Semaphore> _presentCompleteSemaphores;
		std::vector<vk::raii::Semaphore> _renderFinishedSemaphores;
		std::vector<vk::raii::Fence> _inFlightFences;

		uint32_t _currentFrame = 0;
		uint32_t _semaphoreIndex = 0;

		GLFWwindow* _window = nullptr; // I hate this, but whatever

		bool _frameBufferResized = false;

		vk::raii::Buffer _vertexBuffer = VK_NULL_HANDLE;
		vk::raii::DeviceMemory _vertexBufferMemory = VK_NULL_HANDLE;
		vk::raii::Buffer _indexBuffer = VK_NULL_HANDLE;
		vk::raii::DeviceMemory _indexBufferMemory = VK_NULL_HANDLE;

		std::vector<Vertex> _vertices;
		std::vector<uint16_t> _vertexIndicies;

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

		static VKAPI_ATTR vk::Bool32 VKAPI_CALL debugCallback(
			vk::DebugUtilsMessageSeverityFlagBitsEXT severity,
			vk::DebugUtilsMessageTypeFlagsEXT type,
			const vk::DebugUtilsMessengerCallbackDataEXT*
			pCallbackData,
			void* a
		);

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
		static vk::Extent2D chooseSwapExtent(
			const vk::SurfaceCapabilitiesKHR& surface_capabilities,
			GLFWwindow* window
		);

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

		/**
		 * Records a command buffer (it will be deleted after making decisions)
		 */
		void recordCommandBuffer(const vk::raii::CommandBuffer& commandBuffer, uint32_t imageIndex) const;

		/**
		 *
		 */
		void transition_image_layout(
			const uint32_t imageIndex,
			const vk::ImageLayout oldLayout,
			const vk::ImageLayout newLayout,
			const vk::AccessFlags2 srcAccessMask,
			const vk::AccessFlags2 dstAccessMask,
			const vk::PipelineStageFlags2 srcStageMask,
			const vk::PipelineStageFlags2 dstStageMask
		) const;

		/**
		 *
		 */
		void recreateSwapChain();

		/**
		 *
		 * @param typeFilter
		 * @param properties
		 * @return
		 */
		uint32_t findMemoryType(uint32_t typeFilter, vk::MemoryPropertyFlags properties) const;

		/**
		 *
		 * @param window
		 * @param width
		 * @param height
		 */
		static void framebufferResizeCallback(GLFWwindow* window, int width, int height);

		/**
		 *
		 */
		void createVertexBuffer();

		/**
		 *
		 */
		void createIndexBuffer();

		/**
		 *
		 * @param size
		 * @param usage
		 * @param properties
		 * @param buffer
		 * @param bufferMemory
		 */
		void createBuffer(
			vk::DeviceSize size, vk::BufferUsageFlags usage, vk::MemoryPropertyFlags properties,
			vk::raii::Buffer& buffer,
			vk::raii::DeviceMemory& bufferMemory
		) const;

		/**
		 *
		 * @param srcBuffer
		 * @param dstBuffer
		 * @param size
		 */
		void copyBuffer(vk::raii::Buffer& srcBuffer, vk::raii::Buffer& dstBuffer, vk::DeviceSize size) const;
	};
}
