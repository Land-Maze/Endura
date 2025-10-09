//
// Created by land_maze on 10/6/25.
//

#include "VulkanSwapchain.h"

namespace Renderer
{
	void VulkanSwapchain::create(
		GLFWwindow* window,
		const vk::raii::Device& device,
		const vk::SurfaceKHR surface,
		const vk::raii::PhysicalDevice& physicalDevice,
		const uint32_t graphicsFamilyIndex,
		const uint32_t presentFamilyIndex
	)
	{
		createSwapChain(window, device, surface, physicalDevice, graphicsFamilyIndex, presentFamilyIndex);
		createImageViews(device);
	}

	vk::SurfaceFormatKHR VulkanSwapchain::chooseSwapSurfaceFormat(
		const std::vector<vk::SurfaceFormatKHR>& surfaceFormats
	)
	{
		vk::SurfaceFormatKHR result;
		for(const auto& surfaceFormat : surfaceFormats)
		{
			if(surfaceFormat.format == vk::Format::eB8G8R8A8Srgb && surfaceFormat.colorSpace ==
				vk::ColorSpaceKHR::eSrgbNonlinear)
				result = surfaceFormat;
		}
		return result;
	}

	vk::PresentModeKHR VulkanSwapchain::chooseSwapPresentMode(const std::vector<vk::PresentModeKHR>& presentModes)
	{
		for(const auto& presentMode : presentModes)
		{
			if(presentMode == vk::PresentModeKHR::eMailbox)
			{
				return presentMode;
			}
		}
		return vk::PresentModeKHR::eFifo;
	}

	vk::Extent2D VulkanSwapchain::chooseSwapExtent(
		const vk::SurfaceCapabilitiesKHR& surface_capabilities,
		GLFWwindow* window
	)
	{
		// When vulkan sets both width and height to uint32_t maximum value, it means that we have freedom in choosing extent
		if(surface_capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
			return surface_capabilities.currentExtent;

		int width, height;
		glfwGetFramebufferSize(window, &width, &height);

		return {
			std::clamp<uint32_t>(
				width,
				surface_capabilities.minImageExtent.width,
				surface_capabilities.maxImageExtent.width
			),
			std::clamp<uint32_t>(
				height,
				surface_capabilities.minImageExtent.height,
				surface_capabilities.maxImageExtent.height
			)
		};
	}


	void VulkanSwapchain::createSwapChain(
		GLFWwindow* window,
		const vk::raii::Device& device,
		const vk::SurfaceKHR surface,
		const vk::raii::PhysicalDevice& physicalDevice,
		const uint32_t graphicsFamilyIndex,
		const uint32_t presentFamilyIndex
	)
	{
		const auto surfaceCapabilities = physicalDevice.getSurfaceCapabilitiesKHR(surface);
		const auto swapChainSurfaceFormat = chooseSwapSurfaceFormat(physicalDevice.getSurfaceFormatsKHR(surface));

		m_swapChainExtent = chooseSwapExtent(surfaceCapabilities, window);

		const auto minImageCount = std::max(3u, surfaceCapabilities.minImageCount);

		// This ternary operator checks if the minImageCount is bigger that the limit,
		// and if the limit is NOT endless (value of greater than 0),
		// it will return the limit count, otherwise returns required image count
		const auto imageCount = (
			minImageCount > surfaceCapabilities.maxImageCount
			&& surfaceCapabilities.maxImageCount > 0
				? surfaceCapabilities.maxImageCount
				: minImageCount);

		const std::vector<uint32_t> queueFamilyIndices = {graphicsFamilyIndex, presentFamilyIndex};

		uint32_t queueFamilyIndexCount;
		auto queueIndices = queueFamilyIndices.data();
		vk::SharingMode imageSharingMode;

		if(graphicsFamilyIndex == presentFamilyIndex)
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

		m_swapChainImageFormat = swapChainSurfaceFormat.format;

		const vk::SwapchainCreateInfoKHR swapchainCreateInfo(
			{},
			surface,
			imageCount,
			swapChainSurfaceFormat.format,
			swapChainSurfaceFormat.colorSpace,
			m_swapChainExtent,
			1,
			vk::ImageUsageFlagBits::eColorAttachment,
			imageSharingMode,
			queueFamilyIndexCount,
			queueIndices,
			surfaceCapabilities.currentTransform,
			vk::CompositeAlphaFlagBitsKHR::eOpaque,
			chooseSwapPresentMode(
				physicalDevice.getSurfacePresentModesKHR(surface)
			),
			VK_TRUE,
			VK_NULL_HANDLE
		);

		m_swapChain = vk::raii::SwapchainKHR(device, swapchainCreateInfo);
		m_swapChainImages = m_swapChain.getImages();
	}

	void VulkanSwapchain::createImageViews(const vk::raii::Device& device)
	{
		m_swapChainImageViews.clear();
		m_swapChainImageViews.reserve(m_swapChainImages.size());

		constexpr vk::ImageSubresourceRange subresourceRange(
			vk::ImageAspectFlagBits::eColor,
			0,
			1,
			0,
			1
		);

		for(const auto image : m_swapChainImages)
		{
			vk::ImageViewCreateInfo imageViewCreateInfo(
				{},
				image,
				vk::ImageViewType::e2D,
				m_swapChainImageFormat,
				{},
				subresourceRange
			);

			m_swapChainImageViews.emplace_back(device, imageViewCreateInfo);
		}
	}
}
