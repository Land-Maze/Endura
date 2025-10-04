#include "VulkanContext.h"

#include <iostream>
#include <ostream>

#include <Renderer/Shader.h>
#include <AssetManager.h>

namespace Renderer
{
	void VulkanContext::InitializeVulkan(GLFWwindow* window)
	{
		_window = window;

		createInstance();
		setupDebugMessenger();

		pickPhysicalDevice();

		createSurface(_window);
		findBestQueueFamilyIndexes();

		createLogicalDevice();
		createQueues();
		createSwapChain(_window);

		createImageViews();

		createGraphicsPipeline();

		createCommandPool();
		createCommandBuffer();

		createSyncObjects();
	}

	void VulkanContext::Cleanup()
	{
		_device.waitIdle();

		_swapChainImageViews.clear();
		_swapChain = VK_NULL_HANDLE;
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
			_graphics_family_index,
			1,
			&queuePriority
		);

		vk::DeviceCreateInfo deviceCreateInfo(
			{},
			1,
			&deviceQueueCreateInfo,
			static_cast<uint32_t>(validationLayers.size()),
			validationLayers.data(),
			static_cast<uint32_t>(deviceExtensions.size()),
			deviceExtensions.data(),
			{},
			features
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
			std::clamp<uint32_t>(height, surface_capabilities.minImageExtent.height,
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

		const std::vector<uint32_t> queueFamilyIndices = {_graphics_family_index, _present_family_index};

		uint32_t queueFamilyIndexCount;
		auto queueIndices = queueFamilyIndices.data();
		vk::SharingMode imageSharingMode;

		if (_graphics_family_index == _present_family_index)
		{
			queueFamilyIndexCount = 0;
			queueIndices = nullptr;
			imageSharingMode = vk::SharingMode::eExclusive;
		}
		else
		{
			queueFamilyIndexCount = queueFamilyIndices.size();
			imageSharingMode = vk::SharingMode::eConcurrent;
		}

		_swapChainImageFormat = swapChainSurfaceFormat.format;

		const vk::SwapchainCreateInfoKHR swapchainCreateInfo(
			{},
			_surface,
			imageCount,
			swapChainSurfaceFormat.format,
			swapChainSurfaceFormat.colorSpace,
			_swapChainExtent,
			IMAGE_ARRAY_LAYERS,
			vk::ImageUsageFlagBits::eColorAttachment,
			imageSharingMode,
			queueFamilyIndexCount,
			queueIndices,
			surfaceCapabilities.currentTransform,
			vk::CompositeAlphaFlagBitsKHR::eOpaque,
			chooseSwapPresentMode(
				_physical_device.getSurfacePresentModesKHR(_surface)),
			VK_TRUE,
			VK_NULL_HANDLE
		);

		_swapChain = vk::raii::SwapchainKHR(_device, swapchainCreateInfo);
		_swapChainImages = _swapChain.getImages();
	}

	void VulkanContext::createImageViews()
	{
		_swapChainImageViews.clear();
		_swapChainImageViews.reserve(_swapChainImages.size());

		constexpr vk::ImageSubresourceRange subresourceRange(
			vk::ImageAspectFlagBits::eColor,
			0,
			1,
			0,
			1
		);

		for (const auto image : _swapChainImages)
		{
			vk::ImageViewCreateInfo imageViewCreateInfo(
				{},
				image,
				vk::ImageViewType::e2D,
				_swapChainImageFormat,
				{},
				subresourceRange
			);

			_swapChainImageViews.emplace_back(_device, imageViewCreateInfo);
		}
	}

	void VulkanContext::createGraphicsPipeline()
	{
		auto shaderSpirV = Assets::AssetManager::load<Assets::AssetType::Shader>("shader")->spirV;

		auto vertShader = Shader(_device, vk::ShaderStageFlagBits::eVertex, "vertMain", shaderSpirV);
		auto fragShader = Shader(_device, vk::ShaderStageFlagBits::eFragment, "fragMain", shaderSpirV);

		const vk::PipelineShaderStageCreateInfo vertShaderStageInfo = vertShader.getStageInfo();
		const vk::PipelineShaderStageCreateInfo fragShaderStageInfo = fragShader.getStageInfo();

		vk::PipelineShaderStageCreateInfo shaderStages[] = {vertShaderStageInfo, fragShaderStageInfo};

		vk::PipelineVertexInputStateCreateInfo vertexInputInfo;

		vk::PipelineInputAssemblyStateCreateInfo inputAssemblyInfo(
			{},
			vk::PrimitiveTopology::eTriangleList
		);

		const std::vector dynamicStates = {
			vk::DynamicState::eViewport,
			vk::DynamicState::eScissor
		};

		vk::PipelineViewportStateCreateInfo viewportStateInfo(
			{},
			1,
			{},
			1,
			{}
		);

		vk::PipelineDynamicStateCreateInfo pipelineDynamicStateInfo(
			{},
			dynamicStates.size(),
			dynamicStates.data()
		);

		vk::PipelineRasterizationStateCreateInfo rasterizationStateInfo(
			{},
			vk::False,
			vk::False,
			vk::PolygonMode::eFill,
			vk::CullModeFlagBits::eBack,
			vk::FrontFace::eClockwise,
			vk::False,
			{},
			{},
			1.0f,
			1.0f);

		vk::PipelineColorBlendAttachmentState colorBlendAttachmentState(
			vk::True,
			vk::BlendFactor::eSrcAlpha,
			vk::BlendFactor::eOneMinusSrcAlpha,
			vk::BlendOp::eAdd,
			vk::BlendFactor::eOne,
			vk::BlendFactor::eZero,
			vk::BlendOp::eAdd,
			vk::ColorComponentFlagBits::eR |
					   vk::ColorComponentFlagBits::eG |
					   vk::ColorComponentFlagBits::eB |
					   vk::ColorComponentFlagBits::eA
		);

		vk::PipelineColorBlendStateCreateInfo colorBlendingInfo(
			{},
			vk::False,
			vk::LogicOp::eCopy,
			1,
			&colorBlendAttachmentState
		);

		vk::PipelineLayoutCreateInfo pipelineLayoutInfo(
			{},
			0,
			{},
			0
		);

		_pipelineLayout = vk::raii::PipelineLayout(_device, pipelineLayoutInfo);

		vk::PipelineMultisampleStateCreateInfo pipelineMultisampleStateInfo(
			{},
			vk::SampleCountFlagBits::e1,
			vk::True
		);

		vk::PipelineRenderingCreateInfo pipelineRenderingInfo(
			{},
			1,
			&_swapChainImageFormat
		);

		vk::GraphicsPipelineCreateInfo pipelineInfo(
			{},
			2,
			shaderStages,
			&vertexInputInfo,
			&inputAssemblyInfo,
			{},
			&viewportStateInfo,
			&rasterizationStateInfo,
			&pipelineMultisampleStateInfo,
			{},
			&colorBlendingInfo,
			&pipelineDynamicStateInfo,
			_pipelineLayout,
			VK_NULL_HANDLE,
			{},
			VK_NULL_HANDLE,
			-1,
			&pipelineRenderingInfo
		);

		_graphicsPipeline = vk::raii::Pipeline(_device, VK_NULL_HANDLE, pipelineInfo);
	}

	void VulkanContext::createCommandPool()
	{
		const vk::CommandPoolCreateInfo commandPoolInfo(
			vk::CommandPoolCreateFlagBits::eResetCommandBuffer,
			_graphics_family_index
		);
		_commandPool = vk::raii::CommandPool(_device, commandPoolInfo);
	}

	void VulkanContext::createCommandBuffer()
	{
		const vk::CommandBufferAllocateInfo allocateInfo(
			_commandPool,
			vk::CommandBufferLevel::ePrimary,
			MAX_FRAMES_IN_FLIGHT
		);

		_commandBuffers = vk::raii::CommandBuffers(_device, allocateInfo);
	}

	void VulkanContext::createSyncObjects()
	{
		_presentCompleteSemaphores.clear();
		_renderFinishedSemaphores.clear();
		// FIXME: This should be a Timeline Semaphore
		_inFlightFences.clear();

		for (size_t i = 0; i < _swapChainImages.size(); i++) {
			_presentCompleteSemaphores.emplace_back(_device, vk::SemaphoreCreateInfo());
			_renderFinishedSemaphores.emplace_back(_device, vk::SemaphoreCreateInfo());
		}

		for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
		{
			_inFlightFences.emplace_back(_device, vk::FenceCreateInfo(vk::FenceCreateFlagBits::eSignaled));
		}
	}

	void VulkanContext::recordCommandBuffer(const vk::raii::CommandBuffer& commandBuffer, const uint32_t imageIndex) const
	{
		constexpr vk::CommandBufferBeginInfo commandBufferBeginInfo({}, {});
		commandBuffer.begin(commandBufferBeginInfo);

		transition_image_layout(
			imageIndex,
			vk::ImageLayout::eUndefined,
			vk::ImageLayout::eColorAttachmentOptimal,
			{},
			vk::AccessFlagBits2::eColorAttachmentWrite,
			vk::PipelineStageFlagBits2::eTopOfPipe,
			vk::PipelineStageFlagBits2::eColorAttachmentOutput
		);

		constexpr vk::ClearValue clearColor = vk::ClearColorValue(0.1f, 0.3f, 0.1f, 1.0f);

		const vk::RenderingAttachmentInfo attachmentInfo(
			_swapChainImageViews[imageIndex],
			vk::ImageLayout::eColorAttachmentOptimal,
			{},
			{},
			{},
			vk::AttachmentLoadOp::eClear,
			vk::AttachmentStoreOp::eStore,
			clearColor
		);

		const vk::Rect2D renderArea({0, 0}, _swapChainExtent);

		const vk::RenderingInfo renderingInfo(
			{},
			renderArea,
			1,
			{},
			1,
			&attachmentInfo
		);

		const vk::Viewport viewport(
			0.0f,
			0.0f,
			static_cast<float>(_swapChainExtent.width),
			static_cast<float>(_swapChainExtent.height),
			0.0f,
			1.0f
		);

		const vk::Rect2D scissors(
			vk::Offset2D(0, 0),
			_swapChainExtent
		);

		commandBuffer.beginRendering(renderingInfo);
		commandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, _graphicsPipeline);
		commandBuffer.setViewport(0, viewport);
		commandBuffer.setScissor(0, scissors);

		commandBuffer.draw(6, 1, 0, 0);

		commandBuffer.endRendering();

		transition_image_layout(
			imageIndex,
			vk::ImageLayout::eColorAttachmentOptimal,
			vk::ImageLayout::ePresentSrcKHR,
			vk::AccessFlagBits2::eColorAttachmentWrite,
			{},
			vk::PipelineStageFlagBits2::eColorAttachmentOutput,
			vk::PipelineStageFlagBits2::eBottomOfPipe
		);

		commandBuffer.end();
	}

	void VulkanContext::drawFrame()
	{
		while ( vk::Result::eTimeout == _device.waitForFences( *_inFlightFences[_currentFrame], vk::True, UINT64_MAX ) ){}

		auto [result, imageIndex] = _swapChain.acquireNextImage(UINT64_MAX, *_presentCompleteSemaphores[_semaphoreIndex], VK_NULL_HANDLE);

		if (result == vk::Result::eErrorOutOfDateKHR)
		{
			recreateSwapChain();
			return;
		}


		if (result != vk::Result::eSuccess && result != vk::Result::eSuboptimalKHR)
			throw std::runtime_error(
				"Failed to acquire swap chain image: result has value other than eSuccess or eSuboptimalKHR.");

		_device.resetFences(*_inFlightFences[_currentFrame]);

		_commandBuffers[_currentFrame].reset();
		recordCommandBuffer(_commandBuffers[_currentFrame], imageIndex);

		constexpr vk::PipelineStageFlags waitDestinationStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput);

		const vk::SubmitInfo submitInfo(
			1,
			&*_presentCompleteSemaphores[_semaphoreIndex],
			&waitDestinationStageMask,
			1,
			&*_commandBuffers[_currentFrame],
			1,
			&*_renderFinishedSemaphores[imageIndex]
		);

		_graphics_queue.submit(submitInfo, *_inFlightFences[_currentFrame]);

		const vk::PresentInfoKHR presentInfo(
			1,
			&*_renderFinishedSemaphores[imageIndex],
			1,
			&*_swapChain,
			&imageIndex
		);

		result = _present_queue.presentKHR(presentInfo);

		_semaphoreIndex = (_semaphoreIndex + 1) % _presentCompleteSemaphores.size();
		_currentFrame = (_currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
	}

	void VulkanContext::transition_image_layout(const uint32_t imageIndex, const vk::ImageLayout oldLayout,
	                                            const vk::ImageLayout newLayout, const vk::AccessFlags2 srcAccessMask,
	                                            const vk::AccessFlags2 dstAccessMask,
	                                            const vk::PipelineStageFlags2 srcStageMask,
	                                            const vk::PipelineStageFlags2 dstStageMask) const
	{
		constexpr vk::ImageSubresourceRange subresourceRange(
			vk::ImageAspectFlagBits::eColor,
			0,
			1,
			0,
			1
		);

		vk::ImageMemoryBarrier2 imageMemoryBarrier(
			srcStageMask,
			srcAccessMask,
			dstStageMask,
			dstAccessMask,
			oldLayout,
			newLayout,
			VK_QUEUE_FAMILY_IGNORED,
			VK_QUEUE_FAMILY_IGNORED,
			_swapChainImages[imageIndex],
			subresourceRange
		);

		vk::DependencyInfo dependencyInfo(
			{},
			{},
			{},
			{},
			{},
			1,
			&imageMemoryBarrier
		);
		_commandBuffers[_currentFrame].pipelineBarrier2(dependencyInfo);
	}

	void VulkanContext::recreateSwapChain() {
		_device.waitIdle();

		createSwapChain(_window);
		createImageViews();
	}
}
