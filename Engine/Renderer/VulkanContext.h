#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <vulkan/vulkan_raii.hpp>
#include <glm/glm.hpp>

#include <Renderer/VulkanInstance.h>
#include <Renderer/VulkanDevice.h>
#include <Renderer/VulkanSwapchain.h>

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

	struct UniformBufferObject
	{
		glm::mat4 model;
		glm::mat4 view;
		glm::mat4 proj;
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
		std::unique_ptr<VulkanInstance> m_instance = nullptr;
		std::unique_ptr<VulkanDevice> m_device = nullptr;
		std::unique_ptr<VulkanSwapchain> m_swapchain = nullptr;

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

		vk::raii::DescriptorSetLayout _descriptorSetLayout = VK_NULL_HANDLE;

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

		std::vector<vk::raii::Buffer> _uniformBuffers;
		std::vector<vk::raii::DeviceMemory> _uniformBuffersMemory;
		std::vector<void*> _uniformBuffersMapped;

		vk::raii::DescriptorPool _descriptorPool = nullptr;
		std::vector<vk::raii::DescriptorSet> _descriptorSets;

		std::vector<Vertex> _vertices;
		std::vector<uint16_t> _vertexIndicies;

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

		/**
		 *
		 */
		void createDescriptorSetLayout();

		/**
		 *
		 */
		void createUniformBuffers();

		/**
		 *
		 * @param currentImage
		 */
		void updateUniformBuffer(uint32_t currentImage) const;

		/**
		 *
		 */
		void createDescriptorPool();

		/**
		 *
		 */
		void createDescriptorSets();
	};
}
