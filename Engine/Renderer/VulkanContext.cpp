#include "VulkanContext.h"

#include <iostream>
#include <ostream>

#include <Renderer/Shader.h>
#include <AssetManager.h>

namespace Renderer
{
	void VulkanContext::InitializeVulkan(GLFWwindow* window)
	{
		createInstance();
		setupDebugMessenger();

		pickPhysicalDevice();

		createSurface(window);
		findBestQueueFamilyIndexes();

		createLogicalDevice();
		createQueues();
		createSwapChain(window);

		createImageViews();

		createGraphicsPipeline();

		createCommandPool();
		createCommandBuffer();

		createSyncObjects();
	}

	void VulkanContext::createInstance()
	{
		constexpr vk::ApplicationInfo application_info(
			"Endura",
			VK_MAKE_VERSION(0, 0, 1), "No Engine",
			VK_MAKE_API_VERSION(0, 0, 0, 1), vk::ApiVersion13
		);

		std::vector<char const*> requiredLayers;
		if (enableValidationLayers)
		{
			requiredLayers.assign(validationLayers.begin(), validationLayers.end());
		}

		const auto layerProperties = _context.enumerateInstanceLayerProperties();
		bool areLayersSupported = true;
		for (const auto layer : requiredLayers)
		{
			bool isLayerSupported = false;
			for (auto supportedLayer : layerProperties)
			{
				if (strcmp(supportedLayer.layerName, layer) == 0)
				{
					isLayerSupported = true;
					break;
				}
			}
			if (!isLayerSupported)
			{
				areLayersSupported = false;
				break;
			}
		}
		if (!areLayersSupported)
			throw std::runtime_error(
				"One or more required layers are not supported: areLayersSupported is false.");

		const auto extension = getGLFWRequiredExtension();

		const vk::InstanceCreateInfo create_info(
			{
			},
			&application_info,
			requiredLayers.size(),
			requiredLayers.data(),
			extension.size()
			, extension.data(),
			nullptr
		);

		_instance = vk::raii::Instance(_context, create_info);
	}

	std::vector<const char*> VulkanContext::getGLFWRequiredExtension() const
	{
		u_int32_t glfwExtensionCount = 0;
		auto glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

		auto extensionProperties = _context.enumerateInstanceExtensionProperties();
		bool areExtensionsSupported = true;
		for (uint32_t i = 0; i < glfwExtensionCount; i++)
		{
			bool isExtensionSupported = false;
			for (auto supportedExtension : extensionProperties)
			{
				if (strcmp(supportedExtension.extensionName, glfwExtensions[i]) == 0)
				{
					isExtensionSupported = true;
					break;
				}
			}
			if (!isExtensionSupported)
			{
				areExtensionsSupported = false;
				break;
			}
		}
		if (!areExtensionsSupported)
			throw std::runtime_error(
				"One or more required extensions are not supported: areExtensionsSupported is false.");

		std::vector extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

		if (enableValidationLayers) extensions.push_back(vk::EXTDebugUtilsExtensionName);

		return extensions;
	}

	void VulkanContext::setupDebugMessenger()
	{
		if constexpr (!enableValidationLayers) return;

		constexpr vk::DebugUtilsMessageSeverityFlagsEXT severity_flags(
			vk::DebugUtilsMessageSeverityFlagBitsEXT::eError | vk::DebugUtilsMessageSeverityFlagBitsEXT::eInfo |
			vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning);
		constexpr vk::DebugUtilsMessageTypeFlagsEXT message_type_flags(
			vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance |
			vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation);
		constexpr vk::DebugUtilsMessengerCreateInfoEXT debug_utils_messenger_create_info(
			{},
			severity_flags,
			message_type_flags,
			&debugCallback
		);

		debug_messenger = _instance.createDebugUtilsMessengerEXT(debug_utils_messenger_create_info);
	}

	vk::Bool32 VulkanContext::debugCallback(const vk::DebugUtilsMessageSeverityFlagBitsEXT severity,
	                                        const vk::DebugUtilsMessageTypeFlagsEXT type,
	                                        const vk::DebugUtilsMessengerCallbackDataEXT* pCallbackData, void*)
	{
		// FIXME: Implement Logger and put this there
		std::cerr << "validation layer: type " << to_string(type) << " msg: " << pCallbackData->pMessage <<
			std::endl;

		return vk::False;
	}

	void VulkanContext::pickPhysicalDevice()
	{
		const auto devices = _instance.enumeratePhysicalDevices();

		if (devices.empty())
			throw std::runtime_error(
				"No suitable device with Vulkan support was found: enumeratePhysicalDevices() returned empty vector. Is the GPU enabled?");

		uint64_t deviceScore = 0;

		for (auto device : devices)
		{
			const auto deviceProperties = device.getProperties();
			const auto deviceFeatures = device.getFeatures();
			uint64_t score = 0;

			if (deviceProperties.deviceType == vk::PhysicalDeviceType::eDiscreteGpu)
			{
				score += 1000;
			}

			score += deviceProperties.limits.maxImageDimension2D;


			if (score > deviceScore)
			{
				_device_features = deviceFeatures;
				_physical_device = std::move(device);
				deviceScore = score;
			}
		}

		if (_physical_device == VK_NULL_HANDLE)
			throw std::runtime_error(
				"No suitable device with required features was found. Is the GPU enabled?");

		std::printf("Device -> Name: %s, API v%u\n", _physical_device.getProperties().deviceName.data(),
		            _physical_device.getProperties().apiVersion);
	}

	void VulkanContext::findBestQueueFamilyIndexes()
	{
		const std::vector<vk::QueueFamilyProperties> queue_families = _physical_device.getQueueFamilyProperties();

		for (uint32_t i = 0; i < queue_families.size(); i++)
		{
			if (const auto family = queue_families[i]; family.queueFlags & vk::QueueFlagBits::eGraphics &&
				_physical_device.getSurfaceSupportKHR(i, _surface))
			{
				_graphics_family_index = i;
				_present_family_index = i;
				break;
			}
		}

		if (_graphics_family_index == UINT32_MAX)
			for (uint32_t i = 0; i < queue_families.size(); i++)
			{
				const auto family = queue_families[i];
				if ((family.queueFlags & vk::QueueFlagBits::eGraphics) && (_graphics_family_index != UINT32_MAX))
				{
					_graphics_family_index = i;
				}

				if (_physical_device.getSurfaceSupportKHR(
					static_cast<uint32_t>(family.queueFlags & vk::QueueFlagBits::eGraphics), _surface))
				{
					_present_family_index = _graphics_family_index;
					break;
				}
			}

		if ((_graphics_family_index == UINT32_MAX) || (_present_family_index == UINT32_MAX))
			throw std::runtime_error(
				"Could not find a queue for graphics or present: neither of _graphics_family_index nor _present_family_index is set.");
	}

	void VulkanContext::createSurface(GLFWwindow* window)
	{
		VkSurfaceKHR surface;

		if (glfwCreateWindowSurface(*_instance, window, nullptr, &surface) != VK_SUCCESS)
			throw std::runtime_error(
				"Failed to create window surface: glfwCreateWindowSurface returned non-zero value.");

		_surface = vk::raii::SurfaceKHR(_instance, surface);
	}

	void VulkanContext::createQueues()
	{
		_graphics_queue = vk::raii::Queue(_device, _graphics_family_index, 0);
		_present_queue = vk::raii::Queue(_device, _present_family_index, 0);
	}

	void VulkanContext::createLogicalDevice()
	{
		auto features = _physical_device.getFeatures2();

		vk::PhysicalDeviceExtendedDynamicStateFeaturesEXT extendedDynamicStateFeatures;
		extendedDynamicStateFeatures.extendedDynamicState = vk::True;

		features.setPNext(extendedDynamicStateFeatures);

		constexpr float queuePriority = 0.0f;

		const vk::DeviceQueueCreateInfo deviceQueueCreateInfo(
			{},
			_graphics_family_index, 1, &queuePriority
		);

		vk::DeviceCreateInfo deviceCreateInfo(
			{}, 1,
			&deviceQueueCreateInfo, static_cast<uint32_t>(validationLayers.size()),
			validationLayers.data(), static_cast<uint32_t>(deviceExtensions.size()),
			deviceExtensions.data()
		);

		_device = vk::raii::Device(_physical_device, deviceCreateInfo);
	}

	vk::SurfaceFormatKHR VulkanContext::chooseSwapSurfaceFormat(const std::vector<vk::SurfaceFormatKHR>& surfaceFormats)
	{
		vk::SurfaceFormatKHR result;
		for (const auto& surfaceFormat : surfaceFormats)
		{
			if (surfaceFormat.format == vk::Format::eB8G8R8A8Srgb && surfaceFormat.colorSpace ==
				vk::ColorSpaceKHR::eSrgbNonlinear)
				result = surfaceFormat;
		}
		return result;
	}

	vk::PresentModeKHR VulkanContext::chooseSwapPresentMode(const std::vector<vk::PresentModeKHR>& presentModes)
	{
		for (const auto& presentMode : presentModes)
		{
			if (presentMode == vk::PresentModeKHR::eMailbox)
			{
				return presentMode;
			}
		}
		return vk::PresentModeKHR::eFifo;
	}

	vk::Extent2D VulkanContext::chooseSwapExtent(const vk::SurfaceCapabilitiesKHR& surface_capabilities,
	                                             GLFWwindow* window)
	{
		// When vulkan sets both width and height to uint32_t maximum value, it means that we have freedom in choosing extent
		if (surface_capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
			return surface_capabilities.currentExtent;

		int width, height;
		glfwGetFramebufferSize(window, &width, &height);

		return {
			std::clamp<uint32_t>(width, surface_capabilities.minImageExtent.width,
			                     surface_capabilities.maxImageExtent.width),
			std::clamp<uint32_t>(width, surface_capabilities.minImageExtent.height,
			                     surface_capabilities.maxImageExtent.height)
		};
	}


	void VulkanContext::createSwapChain(GLFWwindow* window)
	{
		const auto surfaceCapabilities = _physical_device.getSurfaceCapabilitiesKHR(_surface);
		const auto swapChainSurfaceFormat = chooseSwapSurfaceFormat(_physical_device.getSurfaceFormatsKHR(_surface));

		_swapChainExtent = chooseSwapExtent(surfaceCapabilities, window);

		const auto minImageCount = std::max(3u, surfaceCapabilities.minImageCount);

		// This ternary operator checks if the minImageCount is bigger that the limit,
		// and if the limit is NOT endless (value of greater than 0),
		// it will return the limit count, otherwise returns required image count
		const auto imageCount = (
			minImageCount > surfaceCapabilities.maxImageCount
			&& surfaceCapabilities.maxImageCount > 0
				? surfaceCapabilities.maxImageCount
				: minImageCount);

		std::vector<uint32_t> queueFamilyIndices = {_graphics_family_index, _present_family_index};

		uint32_t queueFamilyIndexCount;
		vk::SharingMode imageSharingMode;

		if (_graphics_family_index == _present_family_index)
		{
			queueFamilyIndexCount = 0;
			imageSharingMode = vk::SharingMode::eExclusive;
		}
		else
		{
			queueFamilyIndexCount = queueFamilyIndices.size();
			imageSharingMode = vk::SharingMode::eExclusive;
		}

		const vk::SwapchainCreateInfoKHR swapchainCreateInfo({}, _surface,
		                                                     imageCount,
		                                                     swapChainSurfaceFormat.format,
		                                                     swapChainSurfaceFormat.colorSpace, _swapChainExtent,
		                                                     IMAGE_ARRAY_LAYERS,
		                                                     vk::ImageUsageFlagBits::eColorAttachment,
		                                                     vk::SharingMode::eExclusive, queueFamilyIndexCount,
		                                                     queueFamilyIndices.data(),
		                                                     surfaceCapabilities.currentTransform,
		                                                     vk::CompositeAlphaFlagBitsKHR::eOpaque,
		                                                     chooseSwapPresentMode(
			                                                     _physical_device.getSurfacePresentModesKHR(_surface)),
		                                                     VK_TRUE,
		                                                     VK_NULL_HANDLE);

		_swapChain = vk::raii::SwapchainKHR(_device, swapchainCreateInfo);
		swapChainImages = _swapChain.getImages();
	}

	void VulkanContext::createImageViews()
	{
		swapChainImages.clear();
		swapChainImageViews.reserve(swapChainImages.size());

		for (const auto image : swapChainImages)
		{
			vk::ImageViewCreateInfo imageViewCreateInfo({}, image, vk::ImageViewType::e2D, swapChainImageFormat, {},
			                                            {vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1});
			swapChainImageViews.emplace_back(_device, imageViewCreateInfo);
		}
	}

	void VulkanContext::createGraphicsPipeline()
	{
	}

	void VulkanContext::createCommandPool()
	{
	}

	void VulkanContext::createCommandBuffer()
	{
	}

	void VulkanContext::createSyncObjects()
	{
	}
}
