#pragma once

#include <Renderer/VulkanDevice.h>
#include <vulkan/vulkan_raii.hpp>

#define VMA_IMPLEMENTATION
#include <vk_mem_alloc.h>

namespace Renderer
{
	class VulkanBuffer
	{
	public:

		VulkanBuffer(VulkanDevice& device, VkDeviceSize size, vk::BufferUsageFlags usage, VmaMemoryUsage memUsage);
		~VulkanBuffer();

		void upload(const void* data, size_t size);
		VkBuffer get() const { return *m_buffer; }
		vk::DeviceSize size() const { return m_size; }

		static void copy(VulkanDevice& device, const VulkanBuffer& src, const VulkanBuffer& dst, vk::DeviceSize size);

	private:
		VulkanDevice& m_device;
		VmaAllocator m_allocator;
		VmaAllocation m_allocation;
		vk::raii::Buffer m_buffer = VK_NULL_HANDLE;
		VkDeviceSize m_size;
	};
}
