#pragma once

#include <vulkan/vulkan_raii.hpp>
#include <vk_mem_alloc.h>

namespace Renderer
{
	class VulkanDevice
	{
		struct sQueue
		{
			vk::raii::Queue graphicsQueue = VK_NULL_HANDLE;
			vk::raii::Queue presentQueue = VK_NULL_HANDLE;
		};

	public:
		VulkanDevice() = default;
		~VulkanDevice();

		VulkanDevice(const VulkanDevice&) = delete;
		VulkanDevice& operator=(const VulkanDevice&) = delete;

		/**
		 * This will create physical device, logical device, queues.
		 * Also, it will find queue family
		 *
		 * @param instance Vulkan instance
		 * @param surface Vulkan surface
		 * @param framesInFlight Frames in flight counter
		 */
		void create(const vk::raii::Instance& instance, const vk::SurfaceKHR& surface, uint32_t framesInFlight);

		vk::raii::CommandBuffer beginSingleTimeCommands();

		void endSingleTimeCommands(vk::raii::CommandBuffer&& cmd);

		/**
		 *
		 * @return Vulkan logical device
		 */
		vk::raii::Device& getDevice() { return m_device; }

		/**
		 *
		 * @return Struct with queues
		 */
		sQueue& getQueues() { return m_queues; }

		/**
		 *
		 * @return Queue family index
		 */
		uint16_t& getQueueFamilyIndex() { return m_queueFamilyIndex; }

		/**
		 *
		 * @return Vulkan physical device
		 */
		vk::raii::PhysicalDevice& getPhysicalDevice() { return m_physicalDevice; }

		/**
		 *
		 * @return Vulkan physical device features
		 */
		vk::PhysicalDeviceFeatures& getPhysicalDeviceFeatures() { return m_physicalDeviceFeatures; }

		VmaAllocator getAllocator() const { return m_allocator; }

		vk::raii::CommandBuffer& getFrameCommandBuffer(uint32_t frameIndex);

	private:
		/**
		 * Picks the best GPU with point system
		 */
		void pickPhysicalDevice(const vk::raii::Instance& instance);

		/**
		 * Finds the best queue family (with both present and graphics queue)
		 */
		void findBestQueueFamilyIndices(const vk::SurfaceKHR& surface);

		/**
		 * Creates Vulkan logical device
		 */
		void createLogicalDevice();

		/**
		 * Creates graphical and present queue
		 */
		void createQueues();

		void createCommandPools();

		void createAllocator(const vk::raii::Instance& instance);

		void destroyAllocator();

		vk::raii::PhysicalDevice m_physicalDevice = VK_NULL_HANDLE;
		vk::PhysicalDeviceFeatures m_physicalDeviceFeatures;

		vk::raii::Device m_device = VK_NULL_HANDLE;

		uint32_t m_maxFramesInFlight = 1;

		VmaAllocator m_allocator;

		vk::raii::CommandPool m_uploadCommandPool = VK_NULL_HANDLE;
		std::vector<vk::raii::CommandPool> m_frameCommandPools;

		std::vector<vk::raii::CommandBuffer> m_frameCommandBuffers;

		sQueue m_queues;

		uint16_t m_queueFamilyIndex = UINT16_MAX;

		const std::vector<const char*> m_validationLayers = {
			"VK_LAYER_KHRONOS_validation"
		};

		const std::vector<const char*> m_deviceExtensions = {
			vk::KHRSwapchainExtensionName,
			vk::KHRSpirv14ExtensionName,
			vk::KHRSynchronization2ExtensionName,
			vk::KHRCreateRenderpass2ExtensionName,
			vk::KHRShaderDrawParametersExtensionName,
			vk::KHRDynamicRenderingExtensionName
		};
	};
}
