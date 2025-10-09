#include "VulkanBuffer.h"

namespace Renderer
{
	VulkanBuffer::VulkanBuffer(
		VulkanDevice& device, const vk::DeviceSize size, const vk::BufferUsageFlags usage, const VmaMemoryUsage memUsage
	):m_device(device), m_allocator(device.getAllocator()), m_size(size)
	{
		vk::BufferCreateInfo bufferCreateInfo(
			{},
			size,
			usage,
			vk::SharingMode::eExclusive
		);

		VmaAllocationCreateInfo allocInfo{};
		allocInfo.usage = memUsage;

		VkBuffer rawBuffer;
		if (vmaCreateBuffer(m_allocator, &*bufferCreateInfo, &allocInfo, &rawBuffer, &m_allocation, nullptr) != VK_SUCCESS)
			throw std::runtime_error("Failed to create VulkanBuffer with VMA");

		m_buffer = vk::raii::Buffer(device.getDevice(), rawBuffer);
	}

	VulkanBuffer::~VulkanBuffer()
	{
		if (*m_buffer)
		{
			vmaDestroyBuffer(m_allocator, *m_buffer, m_allocation);
		}
	}

	void VulkanBuffer::upload(const void* data, const size_t size)
	{
		void* mappedData;
		vmaMapMemory(m_allocator, m_allocation, &mappedData);
		std::memcpy(mappedData, data, size);
		vmaUnmapMemory(m_allocator, m_allocation);
	}

	void VulkanBuffer::copy(VulkanDevice& device, const VulkanBuffer& src, const VulkanBuffer& dst, const vk::DeviceSize size)
	{
		vk::raii::CommandBuffer cmd = device.beginSingleTimeCommands();

		VkBufferCopy copyRegion{};
		copyRegion.srcOffset = 0;
		copyRegion.dstOffset = 0;
		copyRegion.size = size;

		vkCmdCopyBuffer(*cmd, src.get(), dst.get(), 1, &copyRegion);

		device.endSingleTimeCommands(std::move(cmd));
	}
}
