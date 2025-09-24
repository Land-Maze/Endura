// #include <vulkan/vulkan.hpp>
// #define GLFW_INCLUDE_VULKAN
// #include <GLFW/glfw3.h>
//
// #include <fstream>
// #include <iostream>
// #include <stdexcept>
// #include <cstdlib>
// #include <map>
// #include <print>
// #include <limits>
// #include <algorithm>
// #include <cstdint>
//
// #include "vulkan/vulkan_raii.hpp"
// #include <vulkan/vk_platform.h>
//
// constexpr uint32_t SCREEN_WIDTH = 800;
// constexpr uint32_t SCREEN_HEIGHT = 600;
//
// constexpr int MAX_FRAMES_IN_FLIGHT = 2;
//
// const std::vector validationLayers = {
// 	"VK_LAYER_KHRONOS_validation"
// };
//
// const std::vector deviceExtensions = {
// 	vk::KHRSwapchainExtensionName,
// 	vk::KHRSpirv14ExtensionName,
// 	vk::KHRSynchronization2ExtensionName,
// 	vk::KHRCreateRenderpass2ExtensionName,
// };
//
// #ifdef NDEBUG
// constexpr bool enableValidationLayers = false;
// #else
// constexpr bool enableValidationLayers = true;
// #endif
//
// class HelloTriangleApplication
// {
// public:
// 	HelloTriangleApplication()
// 	= default;
//
// 	~HelloTriangleApplication() = default;
//
// 	void run()
// 	{
// 		initWindow();
// 		initVulkan();
// 		mainLoop();
// 		cleanup();
// 	}
//
// private:
//
// 	void cleanupSwapChain() {
// 		swapChainImageViews.clear();
// 		swapChain = nullptr;
// 	}
//
//
// 	void recreateSwapChain() {
// 		device.waitIdle();
//
// 		cleanupSwapChain();
//
// 		createSwapChain();
// 		createImageViews();
// 	}
//
// 	void createSyncObjects()
// 	{
// 		presentCompleteSemaphore = vk::raii::Semaphore(device, vk::SemaphoreCreateInfo());
// 		// signaled when swapchain image is ready
// 		renderFinishedSemaphore = vk::raii::Semaphore(device, vk::SemaphoreCreateInfo());
// 		// signaled when rendering finished (why?). Ah, after render to move image to the screen
// 		drawFence = vk::raii::Fence(device, {.flags = vk::FenceCreateFlagBits::eSignaled});
// 		// signaled when command buffer finished
// 	}
//
// 	void drawFrame()
// 	{
// 		graphicsQueue.waitIdle();
//
// 		auto [result, imageIndex] = swapChain.acquireNextImage( UINT64_MAX, *presentCompleteSemaphore, nullptr );
//
// 		if (result == vk::Result::eErrorOutOfDateKHR) {
// 			recreateSwapChain();
// 			return;
// 		}
// 		if (result != vk::Result::eSuccess && result != vk::Result::eSuboptimalKHR) {
// 			throw std::runtime_error("failed to acquire swap chain image!");
// 		}
// 		recordCommandBuffer(imageIndex);
// 		device.resetFences(*drawFence);
//
// 		vk::PipelineStageFlags waitDestinationStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput);
// 		const vk::SubmitInfo submitInfo{
// 			.waitSemaphoreCount = 1, .pWaitSemaphores = &*presentCompleteSemaphore,
// 			.pWaitDstStageMask = &waitDestinationStageMask, .commandBufferCount = 1, .pCommandBuffers = &*commandBuffer,
// 			.signalSemaphoreCount = 1, .pSignalSemaphores = &*renderFinishedSemaphore
// 		};
//
// 		graphicsQueue.submit(submitInfo, *drawFence);
//
// 		while (vk::Result::eTimeout == device.waitForFences(*drawFence, vk::True, UINT64_MAX));
//
// 		const vk::PresentInfoKHR presentInfoKHR({
// 			.waitSemaphoreCount = 1, .pWaitSemaphores = &*renderFinishedSemaphore,
// 			.swapchainCount = 1, .pSwapchains = &*swapChain, .pImageIndices = &imageIndex
// 		});
//
// 		result = presentQueue.presentKHR( presentInfoKHR );
// 	}
//
// 	void transition_image_layout(
// 		uint32_t imageIndex,
// 		vk::ImageLayout oldLayout,
// 		vk::ImageLayout newLayout,
// 		vk::AccessFlags2 srcAccessMask,
// 		vk::AccessFlags2 dstAccessMask,
// 		vk::PipelineStageFlags2 srcStageMask,
// 		vk::PipelineStageFlags2 dstStageMask
// 	)
// 	{
// 		vk::ImageMemoryBarrier2 barrier = {
// 			.srcStageMask = srcStageMask,
// 			.srcAccessMask = srcAccessMask,
// 			.dstStageMask = dstStageMask,
// 			.dstAccessMask = dstAccessMask,
// 			.oldLayout = oldLayout,
// 			.newLayout = newLayout,
// 			.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
// 			.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
// 			.image = swapChainImages[imageIndex],
// 			.subresourceRange = {
// 				.aspectMask = vk::ImageAspectFlagBits::eColor,
// 				.baseMipLevel = 0,
// 				.levelCount = 1,
// 				.baseArrayLayer = 0,
// 				.layerCount = 1
// 			}
// 		};
// 		vk::DependencyInfo dependencyInfo = {
// 			.dependencyFlags = {},
// 			.imageMemoryBarrierCount = 1,
// 			.pImageMemoryBarriers = &barrier
// 		};
// 		commandBuffer.pipelineBarrier2(dependencyInfo);
// 	}
//
// 	void recordCommandBuffer(uint32_t imageIndex)
// 	{
// 		commandBuffer.begin({});
//
// 		// Before starting rendering, transition the swapchain image to COLOR_ATTACHMENT_OPTIMAL
// 		transition_image_layout(
// 			imageIndex,
// 			vk::ImageLayout::eUndefined,
// 			vk::ImageLayout::eColorAttachmentOptimal,
// 			{}, // srcAccessMask (no need to wait for previous operations)
// 			vk::AccessFlagBits2::eColorAttachmentWrite, // dstAccessMask
// 			vk::PipelineStageFlagBits2::eTopOfPipe, // srcStage
// 			vk::PipelineStageFlagBits2::eColorAttachmentOutput // dstStage
// 		);
//
// 		vk::ClearValue clearColor = vk::ClearColorValue(0.0f, 0.0f, 0.0f, 1.0f);
// 		vk::RenderingAttachmentInfo attachmentInfo = {
// 			.imageView = swapChainImageViews[imageIndex],
// 			.imageLayout = vk::ImageLayout::eColorAttachmentOptimal,
// 			.loadOp = vk::AttachmentLoadOp::eClear,
// 			.storeOp = vk::AttachmentStoreOp::eStore,
// 			.clearValue = clearColor
// 		};
//
// 		vk::RenderingInfo renderingInfo = {
// 			.renderArea = {.offset = {0, 0}, .extent = swapChainExtent},
// 			.layerCount = 1,
// 			.colorAttachmentCount = 1,
// 			.pColorAttachments = &attachmentInfo
// 		};
//
// 		commandBuffer.beginRendering(renderingInfo);
//
// 		commandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, graphicsPipeline);
//
// 		commandBuffer.setViewport(0, vk::Viewport(0.0f, 0.0f, static_cast<float>(swapChainExtent.width),
// 		                                          static_cast<float>(swapChainExtent.height), 0.0f, 1.0f));
// 		commandBuffer.setScissor(0, vk::Rect2D(vk::Offset2D(0, 0), swapChainExtent));
//
// 		commandBuffer.draw(3, 1, 0, 0);
//
// 		commandBuffer.endRendering();
//
// 		// After rendering, transition the swapchain image to PRESENT_SRC
// 		transition_image_layout(
// 			imageIndex,
// 			vk::ImageLayout::eColorAttachmentOptimal,
// 			vk::ImageLayout::ePresentSrcKHR,
// 			vk::AccessFlagBits2::eColorAttachmentWrite, // srcAccessMask
// 			{}, // dstAccessMask
// 			vk::PipelineStageFlagBits2::eColorAttachmentOutput, // srcStage
// 			vk::PipelineStageFlagBits2::eBottomOfPipe // dstStage
// 		);
//
// 		commandBuffer.end();
// 	}
//
// 	void createCommandBuffer()
// 	{
// 		vk::CommandBufferAllocateInfo allocInfo{
// 			.commandPool = commandPool, .level = vk::CommandBufferLevel::ePrimary, .commandBufferCount = 1
// 		};
//
// 		commandBuffer = std::move(vk::raii::CommandBuffers(device, allocInfo).front());
// 	}
//
// 	void createCommandPool()
// 	{
// 		vk::CommandPoolCreateInfo poolInfo{
// 			.flags = vk::CommandPoolCreateFlagBits::eResetCommandBuffer, .queueFamilyIndex = graphicsFamilyIndex
// 		};
// 		commandPool = vk::raii::CommandPool(device, poolInfo);
// 	}
//
// 	static std::vector<char> readFile(const std::string& filename)
// 	{
// 		std::ifstream file(filename, std::ios::ate | std::ios::binary);
//
// 		if (!file.is_open())
// 		{
// 			throw std::runtime_error("failed to open file!");
// 		}
//
// 		std::vector<char> buffer(file.tellg());
//
// 		file.seekg(0, std::ios::beg);
// 		file.read(buffer.data(), static_cast<std::streamsize>(buffer.size()));
//
// 		file.close();
//
// 		return buffer;
// 	}
//
// 	[[nodiscard]]
// 	vk::raii::ShaderModule createShaderModule(const std::vector<char>& code) const
// 	{
// 		vk::ShaderModuleCreateInfo createInfo{
// 			.codeSize = code.size() * sizeof(char), .pCode = reinterpret_cast<const uint32_t*>(code.data())
// 		};
//
// 		vk::raii::ShaderModule shaderModule{device, createInfo};
//
// 		return shaderModule;
// 	}
//
// 	void createGraphicsPipeline()
// 	{
// 		auto shaderCode = readFile("Assets/Shaders/slang.spv");
// 		auto shaderModule = createShaderModule(shaderCode);
//
// 		vk::PipelineShaderStageCreateInfo vertShaderStageInfo{
// 			.stage = vk::ShaderStageFlagBits::eVertex, .module = shaderModule, .pName = "vertMain"
// 		};
// 		vk::PipelineShaderStageCreateInfo fragShaderStageInfo{
// 			.stage = vk::ShaderStageFlagBits::eFragment, .module = shaderModule, .pName = "fragMain"
// 		};
//
// 		vk::PipelineShaderStageCreateInfo shaderStages[] = {vertShaderStageInfo, fragShaderStageInfo};
//
// 		vk::PipelineVertexInputStateCreateInfo vertexInputInfo;
//
// 		vk::PipelineInputAssemblyStateCreateInfo inputAssembly{.topology = vk::PrimitiveTopology::eTriangleList};
//
// 		std::vector dynamicStates = {
// 			vk::DynamicState::eViewport,
// 			vk::DynamicState::eScissor
// 		};
//
// 		vk::PipelineViewportStateCreateInfo viewportState{.viewportCount = 1, .scissorCount = 1};
//
// 		vk::PipelineDynamicStateCreateInfo dynamicState{
// 			.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size()), .pDynamicStates = dynamicStates.data()
// 		};
//
// 		vk::PipelineRasterizationStateCreateInfo rasterizer{
// 			.depthClampEnable = vk::False, .rasterizerDiscardEnable = vk::False,
// 			.polygonMode = vk::PolygonMode::eFill, .cullMode = vk::CullModeFlagBits::eBack,
// 			.frontFace = vk::FrontFace::eClockwise, .depthBiasEnable = vk::False,
// 			.depthBiasSlopeFactor = 1.0f, .lineWidth = 1.0f
// 		};
//
// 		vk::PipelineColorBlendAttachmentState colorBlendAttachment;
// 		colorBlendAttachment.colorWriteMask = vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG |
// 			vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA;
// 		colorBlendAttachment.blendEnable = vk::True;
// 		colorBlendAttachment.srcColorBlendFactor = vk::BlendFactor::eSrcAlpha;
// 		colorBlendAttachment.dstColorBlendFactor = vk::BlendFactor::eOneMinusSrcAlpha;
// 		colorBlendAttachment.colorBlendOp = vk::BlendOp::eAdd;
// 		colorBlendAttachment.srcAlphaBlendFactor = vk::BlendFactor::eOne;
// 		colorBlendAttachment.dstAlphaBlendFactor = vk::BlendFactor::eZero;
// 		colorBlendAttachment.alphaBlendOp = vk::BlendOp::eAdd;
//
// 		vk::PipelineColorBlendStateCreateInfo colorBlending{
// 			.logicOpEnable = vk::False, .logicOp = vk::LogicOp::eCopy, .attachmentCount = 1,
// 			.pAttachments = &colorBlendAttachment
// 		};
//
// 		vk::PipelineLayoutCreateInfo pipelineLayoutInfo{.setLayoutCount = 0, .pushConstantRangeCount = 0};
//
// 		pipelineLayout = vk::raii::PipelineLayout(device, pipelineLayoutInfo);
// 		vk::PipelineMultisampleStateCreateInfo multisampling{
// 			.rasterizationSamples = vk::SampleCountFlagBits::e1, .sampleShadingEnable = vk::False
// 		};
//
// 		vk::PipelineRenderingCreateInfo pipelineRenderingCreateInfo{
// 			.colorAttachmentCount = 1, .pColorAttachmentFormats = &swapChainImageFormat
// 		};
// 		vk::GraphicsPipelineCreateInfo pipelineInfo{
// 			.pNext = &pipelineRenderingCreateInfo,
// 			.stageCount = 2, .pStages = shaderStages,
// 			.pVertexInputState = &vertexInputInfo, .pInputAssemblyState = &inputAssembly,
// 			.pViewportState = &viewportState, .pRasterizationState = &rasterizer,
// 			.pMultisampleState = &multisampling, .pColorBlendState = &colorBlending,
// 			.pDynamicState = &dynamicState, .layout = pipelineLayout, .renderPass = nullptr
// 		};
//
// 		pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
// 		pipelineInfo.basePipelineIndex = -1;
//
// 		graphicsPipeline = vk::raii::Pipeline(device, VK_NULL_HANDLE, pipelineInfo);
// 	}
//
// 	void createImageViews()
// 	{
// 		swapChainImageViews.clear();
//
// 		vk::ImageViewCreateInfo imageViewCreateInfo{
// 			.viewType = vk::ImageViewType::e2D, .format = swapChainImageFormat,
// 			.subresourceRange = {vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1}
// 		};
//
// 		swapChainImageViews.reserve(swapChainImageViews.size());
//
// 		for (auto image : swapChainImages)
// 		{
// 			imageViewCreateInfo.image = image;
// 			swapChainImageViews.emplace_back(device, imageViewCreateInfo);
// 		}
// 	}
//
// 	void createSwapChain()
// 	{
// 		auto surfaceCapabilities = physicalDevice.getSurfaceCapabilitiesKHR(surface);
// 		auto swapChainSurfaceFormat = chooseSwapSurfaceFormat(physicalDevice.getSurfaceFormatsKHR(surface));
// 		swapChainExtent = chooseSwapExtent(surfaceCapabilities);
// 		auto minImageCount = std::max(3u, surfaceCapabilities.minImageCount);
// 		minImageCount = (surfaceCapabilities.maxImageCount > 0 && minImageCount > surfaceCapabilities.maxImageCount)
// 			                ? surfaceCapabilities.maxImageCount
// 			                : minImageCount;
//
// 		uint32_t imageCount = surfaceCapabilities.minImageCount + 1;
// 		if (surfaceCapabilities.maxImageCount > 0 && imageCount > surfaceCapabilities.maxImageCount)
// 		{
// 			imageCount = surfaceCapabilities.maxImageCount;
// 		}
//
// 		swapChainImageFormat = swapChainSurfaceFormat.format;
//
// 		vk::SwapchainCreateInfoKHR swapChainCreateInfo{
// 			.flags = vk::SwapchainCreateFlagsKHR(),
// 			.surface = surface,
// 			.minImageCount = minImageCount,
// 			.imageFormat = swapChainSurfaceFormat.format,
// 			.imageColorSpace = swapChainSurfaceFormat.colorSpace,
// 			.imageExtent = swapChainExtent,
// 			.imageArrayLayers = 1,
// 			.imageUsage = vk::ImageUsageFlagBits::eColorAttachment, .imageSharingMode = vk::SharingMode::eExclusive,
// 			.preTransform = surfaceCapabilities.currentTransform,
// 			.compositeAlpha = vk::CompositeAlphaFlagBitsKHR::eOpaque,
// 			.presentMode = chooseSwapPresentMode(physicalDevice.getSurfacePresentModesKHR(surface)),
// 			.clipped = true, .oldSwapchain = VK_NULL_HANDLE
// 		};
//
// 		uint32_t queueFamilyIndices[] = {graphicsFamilyIndex, presentFamilyIndex};
//
// 		if (graphicsFamilyIndex != presentFamilyIndex)
// 		{
// 			swapChainCreateInfo.imageSharingMode = vk::SharingMode::eConcurrent;
// 			swapChainCreateInfo.queueFamilyIndexCount = 2;
// 			swapChainCreateInfo.pQueueFamilyIndices = queueFamilyIndices;
// 		}
// 		else
// 		{
// 			swapChainCreateInfo.imageSharingMode = vk::SharingMode::eExclusive;
// 			swapChainCreateInfo.queueFamilyIndexCount = 0; // Optional
// 			swapChainCreateInfo.pQueueFamilyIndices = nullptr; // Optional
// 		}
//
// 		swapChain = vk::raii::SwapchainKHR(device, swapChainCreateInfo);
// 		swapChainImages = swapChain.getImages();
// 	}
//
// 	vk::PresentModeKHR chooseSwapPresentMode(const std::vector<vk::PresentModeKHR>& availablePresentModes)
// 	{
// 		for (const auto& availablePresentMode : availablePresentModes)
// 		{
// 			if (availablePresentMode == vk::PresentModeKHR::eMailbox)
// 			{
// 				return availablePresentMode;
// 			}
// 		}
// 		return vk::PresentModeKHR::eFifo;
// 	}
//
// 	vk::SurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<vk::SurfaceFormatKHR>& availableFormats)
// 	{
// 		for (const auto& availableFormat : availableFormats)
// 		{
// 			std::print("Found available format: {} {}\n", to_string(availableFormat.format),
// 			           to_string(availableFormat.colorSpace));
// 			if (availableFormat.format == vk::Format::eB8G8R8A8Srgb && availableFormat.colorSpace ==
// 				vk::ColorSpaceKHR::eSrgbNonlinear)
// 			{
// 				return availableFormat;
// 			}
// 		}
//
// 		return availableFormats[0];
// 	}
//
// 	vk::Extent2D chooseSwapExtent(const vk::SurfaceCapabilitiesKHR& capabilities)
// 	{
// 		if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
// 		{
// 			return capabilities.currentExtent;
// 		}
// 		int width, height;
// 		glfwGetFramebufferSize(window, &width, &height);
//
// 		return {
// 			std::clamp<uint32_t>(width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width),
// 			std::clamp<uint32_t>(height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height)
// 		};
// 	}
//
// 	void createSurface()
// 	{
// 		VkSurfaceKHR _surface; // Surface handle Vulkan C API
// 		if (glfwCreateWindowSurface(*instance, window, nullptr, &_surface) != 0)
// 		{
// 			throw std::runtime_error("failed to create window surface!");
// 		}
// 		surface = vk::raii::SurfaceKHR(instance, _surface);
// 	}
//
//
// 	void createLogicalDevice()
// 	{
// 		// find the index of the first queue family that supports graphics
// 		std::vector<vk::QueueFamilyProperties> queueFamilyProperties = physicalDevice.getQueueFamilyProperties();
//
// 		// get the first index into queueFamilyProperties which supports graphics
// 		auto graphicsQueueFamilyProperty = std::ranges::find_if(queueFamilyProperties, [](auto const& qfp)
// 		{
// 			return (qfp.queueFlags &
// 				vk::QueueFlagBits::eGraphics) != static_cast<
// 				vk::QueueFlags>(0);
// 		});
//
// 		graphicsFamilyIndex = static_cast<uint32_t>(std::distance(queueFamilyProperties.begin(),
// 		                                                          graphicsQueueFamilyProperty));
//
// 		// determine a queueFamilyIndex that supports present
// 		// first check if the graphicsIndex is good enough
// 		presentFamilyIndex = physicalDevice.getSurfaceSupportKHR(graphicsFamilyIndex, *surface)
// 			                     ? graphicsFamilyIndex
// 			                     : static_cast<uint32_t>(queueFamilyProperties.size());
// 		if (presentFamilyIndex == queueFamilyProperties.size())
// 		{
// 			// the graphicsIndex doesn't support present -> look for another family index that supports both
// 			// graphics and present
// 			for (size_t i = 0; i < queueFamilyProperties.size(); i++)
// 			{
// 				if ((queueFamilyProperties[i].queueFlags & vk::QueueFlagBits::eGraphics) &&
// 					physicalDevice.getSurfaceSupportKHR(static_cast<uint32_t>(i), *surface))
// 				{
// 					graphicsFamilyIndex = static_cast<uint32_t>(i);
// 					presentFamilyIndex = graphicsFamilyIndex;
// 					break;
// 				}
// 			}
// 			if (presentFamilyIndex == queueFamilyProperties.size())
// 			{
// 				// there's nothing like a single family index that supports both graphics and present -> look for another
// 				// family index that supports present
// 				for (size_t i = 0; i < queueFamilyProperties.size(); i++)
// 				{
// 					if (physicalDevice.getSurfaceSupportKHR(static_cast<uint32_t>(i), *surface))
// 					{
// 						presentFamilyIndex = static_cast<uint32_t>(i);
// 						break;
// 					}
// 				}
// 			}
// 		}
//
// 		if ((graphicsFamilyIndex == queueFamilyProperties.size()) || (presentFamilyIndex == queueFamilyProperties.
// 			size()))
// 		{
// 			throw std::runtime_error("Could not find a queue for graphics or present -> terminating");
// 		}
//
// 		// query for Vulkan 1.3 features
// 		auto features = physicalDevice.getFeatures2();
// 		vk::PhysicalDeviceVulkan11Features vulkan11Features;
// 		vulkan11Features.shaderDrawParameters = vk::True; // Enable it if SPIR-V complains about enabling this
// 		vk::PhysicalDeviceVulkan13Features vulkan13Features;
// 		vk::PhysicalDeviceExtendedDynamicStateFeaturesEXT extendedDynamicStateFeatures;
// 		vulkan13Features.dynamicRendering = vk::True;
// 		vulkan13Features.synchronization2 = vk::True;
// 		extendedDynamicStateFeatures.extendedDynamicState = vk::True;
// 		vulkan13Features.pNext = &extendedDynamicStateFeatures;
// 		vulkan11Features.pNext = &vulkan13Features;
// 		features.pNext = vulkan11Features;
//
//
// 		// create a Device
// 		float queuePriority = 0.0f;
// 		vk::DeviceQueueCreateInfo deviceQueueCreateInfo{
// 			.queueFamilyIndex = graphicsFamilyIndex, .queueCount = 1, .pQueuePriorities = &queuePriority
// 		};
// 		vk::DeviceCreateInfo deviceCreateInfo{
// 			.pNext = &features, .queueCreateInfoCount = 1, .pQueueCreateInfos = &deviceQueueCreateInfo,
// 			.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size()),
// 			.ppEnabledExtensionNames = deviceExtensions.data()
// 		};
// 		deviceCreateInfo.enabledExtensionCount = deviceExtensions.size();
// 		deviceCreateInfo.ppEnabledExtensionNames = deviceExtensions.data();
//
// 		device = vk::raii::Device(physicalDevice, deviceCreateInfo);
// 		graphicsQueue = vk::raii::Queue(device, graphicsFamilyIndex, 0);
// 		presentQueue = vk::raii::Queue(device, presentFamilyIndex, 0);
// 	}
//
// 	void pickPhysicalDevice()
// 	{
// 		auto devices = instance.enumeratePhysicalDevices();
//
// 		if (devices.empty())
// 		{
// 			throw std::runtime_error("failed to find GPUs with Vulkan support!");
// 		}
//
// 		long long chosenDeviceScore = -1;
//
// 		for (const auto& device : devices)
// 		{
// 			auto deviceProperties = device.getProperties();
// 			auto deviceFeatures = device.getFeatures();
// 			long long score = 0;
//
// 			if (deviceProperties.deviceType == vk::PhysicalDeviceType::eDiscreteGpu)
// 			{
// 				score += 1000;
// 			}
//
// 			score += deviceProperties.limits.maxImageDimension2D;
//
// 			if (!deviceFeatures.geometryShader)
// 			{
// 				continue;
// 			}
//
// 			if (score > chosenDeviceScore)
// 			{
// 				this->deviceFeatures = deviceFeatures;
// 				physicalDevice = std::move(device);
// 				chosenDeviceScore = score;
// 			}
// 		}
//
// 		if (!*physicalDevice && chosenDeviceScore == -1)
// 		{
// 			throw std::runtime_error("failed to find a suitable GPU!");
// 		}
//
// 		std::print("The chosen device is: {}\n", std::string_view(physicalDevice.getProperties().deviceName));
// 	}
//
// 	uint32_t findQueueFamilies(vk::raii::PhysicalDevice* physicalDevice)
// 	{
// 		std::vector<vk::QueueFamilyProperties> queueFamilyProperties = physicalDevice->getQueueFamilyProperties();
//
// 		auto graphicsQueueFamilyProperty =
// 			std::find_if(queueFamilyProperties.begin(),
// 			             queueFamilyProperties.end(),
// 			             [](vk::QueueFamilyProperties const& qfp)
// 			             {
// 				             return qfp.queueFlags & vk::QueueFlagBits::eGraphics;
// 			             });
//
// 		return static_cast<uint32_t>(std::distance(queueFamilyProperties.begin(), graphicsQueueFamilyProperty));
// 	}
//
// 	void setupDebugMessenger()
// 	{
// 		if (!enableValidationLayers) return;
//
// 		vk::DebugUtilsMessageSeverityFlagsEXT severityFlags(
// 			vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose | vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning |
// 			vk::DebugUtilsMessageSeverityFlagBitsEXT::eError);
// 		vk::DebugUtilsMessageTypeFlagsEXT messageTypeFlags(
// 			vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral | vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance |
// 			vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation);
// 		vk::DebugUtilsMessengerCreateInfoEXT debugUtilsMessengerCreateInfoEXT{
// 			.messageSeverity = severityFlags,
// 			.messageType = messageTypeFlags,
// 			.pfnUserCallback = &debugCallback
// 		};
// 		debugMessenger = instance.createDebugUtilsMessengerEXT(debugUtilsMessengerCreateInfoEXT);
// 	}
//
// 	static VKAPI_ATTR vk::Bool32 VKAPI_CALL debugCallback(vk::DebugUtilsMessageSeverityFlagBitsEXT severity,
// 	                                                      vk::DebugUtilsMessageTypeFlagsEXT type,
// 	                                                      const vk::DebugUtilsMessengerCallbackDataEXT* pCallbackData,
// 	                                                      void*)
// 	{
// 		std::cerr << "validation layer: type " << to_string(type) << " msg: " << pCallbackData->pMessage << std::endl;
//
// 		return vk::False;
// 	}
//
// 	std::vector<const char*> getRequiredExtensions()
// 	{
// 		uint32_t glfwExtensionCount = 0;
// 		auto glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
//
// 		auto extensionProperties = context.enumerateInstanceExtensionProperties();
// 		for (uint32_t i = 0; i < glfwExtensionCount; ++i)
// 		{
// 			if (std::ranges::none_of(extensionProperties,
// 			                         [glfwExtension = glfwExtensions[i]](auto const& extensionProperty)
// 			                         {
// 				                         return strcmp(extensionProperty.extensionName, glfwExtension) == 0;
// 			                         }))
// 			{
// 				throw std::runtime_error("Required GLFW extension not supported: " + std::string(glfwExtensions[i]));
// 			}
// 		}
//
// 		std::vector extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);
// 		if (enableValidationLayers)
// 		{
// 			extensions.push_back(vk::EXTDebugUtilsExtensionName);
// 		}
//
// 		return extensions;
// 	}
//
// 	void initWindow()
// 	{
// 		glfwInit();
//
// 		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
// 		glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
//
// 		window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Vulkan", nullptr, nullptr);
// 	}
//
// 	void initVulkan()
// 	{
// 		createInstance();
// 		setupDebugMessenger();
// 		createSurface();
// 		pickPhysicalDevice();
// 		createLogicalDevice();
// 		createSwapChain();
// 		createImageViews();
// 		createGraphicsPipeline();
// 		createCommandPool();
// 		createCommandBuffer();
// 		createSyncObjects();
// 	}
//
// 	void createInstance()
// 	{
// 		constexpr vk::ApplicationInfo appInfo{
// 			.pApplicationName = "Hello Triangle",
// 			.applicationVersion = VK_MAKE_VERSION(1, 0, 0),
// 			.pEngineName = "No Engine",
// 			.engineVersion = VK_MAKE_VERSION(1, 0, 0),
// 			.apiVersion = vk::ApiVersion13
// 		};
//
// 		std::vector<char const*> requiredLayers;
// 		if (enableValidationLayers)
// 		{
// 			requiredLayers.assign(validationLayers.begin(), validationLayers.end());
// 		}
//
// 		auto layerProperties = context.enumerateInstanceLayerProperties();
// 		if (std::ranges::any_of(requiredLayers, [&layerProperties](auto const& requiredLayer)
// 		{
// 			return std::ranges::none_of(layerProperties,
// 			                            [requiredLayer](auto const& layerProperty)
// 			                            {
// 				                            return strcmp(layerProperty.layerName, requiredLayer) == 0;
// 			                            });
// 		}))
// 		{
// 			throw std::runtime_error("One or more required layers are not supported!");
// 		}
//
// 		auto extensions = getRequiredExtensions();
// 		vk::InstanceCreateInfo createInfo{
// 			.pApplicationInfo = &appInfo,
// 			.enabledLayerCount = static_cast<uint32_t>(requiredLayers.size()),
// 			.ppEnabledLayerNames = requiredLayers.data(),
// 			.enabledExtensionCount = static_cast<uint32_t>(extensions.size()),
// 			.ppEnabledExtensionNames = extensions.data()
// 		};
//
// 		instance = vk::raii::Instance(context, createInfo);
// 	}
//
// 	void mainLoop()
// 	{
// 		while (!glfwWindowShouldClose(window))
// 		{
// 			glfwPollEvents();
// 			drawFrame();
// 		}
//
// 		device.waitIdle();
// 	}
//
// 	void cleanup() {
// 		cleanupSwapChain();
//
// 		glfwDestroyWindow(window);
// 		glfwTerminate();
// 	}
//
// 	GLFWwindow* window = nullptr;
// 	vk::raii::Context context;
// 	vk::raii::Instance instance = nullptr;
// 	vk::raii::DebugUtilsMessengerEXT debugMessenger = nullptr;
// 	vk::raii::PhysicalDevice physicalDevice = nullptr;
// 	vk::PhysicalDeviceFeatures deviceFeatures;
// 	vk::raii::Device device = nullptr;
// 	vk::raii::Queue graphicsQueue = nullptr;
// 	vk::raii::SurfaceKHR surface = nullptr;
// 	vk::raii::Queue presentQueue = nullptr;
// 	uint32_t graphicsFamilyIndex = 0;
// 	uint32_t presentFamilyIndex = 0;
// 	vk::raii::SwapchainKHR swapChain = nullptr;
// 	std::vector<vk::Image> swapChainImages;
// 	vk::Format swapChainImageFormat = vk::Format::eUndefined;
// 	vk::Extent2D swapChainExtent;
// 	std::vector<vk::raii::ImageView> swapChainImageViews;
// 	vk::raii::PipelineLayout pipelineLayout = nullptr;
// 	vk::raii::Pipeline graphicsPipeline = nullptr;
//
// 	vk::raii::CommandPool commandPool = nullptr;
// 	vk::raii::CommandBuffer commandBuffer = nullptr;
//
// 	vk::raii::Semaphore presentCompleteSemaphore = nullptr;
// 	vk::raii::Semaphore renderFinishedSemaphore = nullptr;
// 	vk::raii::Fence drawFence = nullptr;
// };
//
// int main()
// {
// 	HelloTriangleApplication app;
//
// 	std::print("Are validation layers enabled?: {}\n", static_cast<int>(enableValidationLayers));
//
// 	try
// 	{
// 		app.run();
// 	}
// 	catch (const std::exception& e)
// 	{
// 		std::cerr << e.what() << std::endl;
// 		return EXIT_FAILURE;
// 	}
//
// 	return EXIT_SUCCESS;
// }


#include <Core/Window.h>
#include <Renderer/VulkanContext.h>

int main() {
    Core::Window window;
    Renderer::VulkanContext vkContext;

    window.create({});
}