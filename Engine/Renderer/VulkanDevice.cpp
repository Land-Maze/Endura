#include "VulkanDevice.h"

namespace Renderer
{
	void VulkanDevice::create(const vk::raii::Instance& instance, const vk::SurfaceKHR& surface)
	{
		pickPhysicalDevice(instance);
		findBestQueueFamilyIndices(surface);
		createLogicalDevice();
		createQueues();
	}

	void VulkanDevice::pickPhysicalDevice(const vk::raii::Instance& instance)
	{
		const auto devices = instance.enumeratePhysicalDevices();

		if(devices.empty())
		{
			throw std::runtime_error(
				"No suitable device with Vulkan support was found: enumeratePhysicalDevices() returned empty vector. Is the GPU enabled?"
			);
		}

		uint64_t deviceScore = 0;

		for(auto device : devices)
		{
			const auto deviceProperties = device.getProperties();
			const auto deviceFeatures = device.getFeatures();
			uint64_t score = 0;

			if(deviceProperties.deviceType == vk::PhysicalDeviceType::eDiscreteGpu)
			{
				score += 1000;
			}

			score += deviceProperties.limits.maxImageDimension2D;


			if(score > deviceScore)
			{
				m_physicalDeviceFeatures = deviceFeatures;
				m_physicalDevice = std::move(device);
				deviceScore = score;
			}
		}

		if(m_physicalDevice == VK_NULL_HANDLE)
		{
			throw std::runtime_error(
				"No suitable device with required features was found. Is the GPU enabled?"
			);
		}
	}

	void VulkanDevice::findBestQueueFamilyIndices(const vk::SurfaceKHR& surface)
	{
		const auto queueFamilies = m_physicalDevice.getQueueFamilyProperties();

		for(uint32_t i = 0; i < queueFamilies.size(); ++i)
		{
			if(
				(queueFamilies[i].queueFlags & vk::QueueFlagBits::eGraphics)
				&&
				m_physicalDevice.getSurfaceSupportKHR(i, surface)
			)
			{
				m_queueFamilyIndex = i;
				break;
			}
		}

		if(m_queueFamilyIndex == UINT16_MAX)
		{
			throw std::runtime_error(
				"Could not find a queue for graphics and present: m_queueFamilyIndex=UINT16_MAX."
			);
		}
	}

	void VulkanDevice::createLogicalDevice()
	{
		auto features = m_physicalDevice.getFeatures2();
		features.features.sampleRateShading = vk::True;

		vk::PhysicalDeviceExtendedDynamicStateFeaturesEXT extendedDynamicStateFeatures;
		extendedDynamicStateFeatures.extendedDynamicState = vk::True;

		vk::PhysicalDeviceVulkan11Features deviceVulkan11Features;
		deviceVulkan11Features.pNext = extendedDynamicStateFeatures;
		deviceVulkan11Features.shaderDrawParameters = vk::True;


		vk::PhysicalDeviceVulkan13Features deviceVulkan13Features;
		deviceVulkan13Features.dynamicRendering = vk::True;
		deviceVulkan13Features.synchronization2 = vk::True;

		deviceVulkan13Features.pNext = deviceVulkan11Features;

		features.setPNext(deviceVulkan13Features);

		constexpr float queuePriority = 0.0f;
		const vk::DeviceQueueCreateInfo deviceQueueCreateInfo(
			{},
			m_queueFamilyIndex,
			1,
			&queuePriority
		);

		vk::DeviceCreateInfo deviceCreateInfo(
			{},
			1,
			&deviceQueueCreateInfo,
			static_cast<uint32_t>(m_validationLayers.size()),
			m_validationLayers.data(),
			static_cast<uint32_t>(m_deviceExtensions.size()),
			m_deviceExtensions.data(),
			{},
			features
		);

		m_device = vk::raii::Device(m_physicalDevice, deviceCreateInfo);
	}

	void VulkanDevice::createQueues()
	{
		m_queues.graphicsQueue = vk::raii::Queue(m_device, m_queueFamilyIndex, 0);
		m_queues.presentQueue = vk::raii::Queue(m_device, m_queueFamilyIndex, 0);
	}
}
