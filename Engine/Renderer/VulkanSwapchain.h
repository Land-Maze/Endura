#pragma once
#include <vector>
#include <vulkan/vulkan_raii.hpp>
#include <GLFW/glfw3.h>

namespace Renderer
{
	class VulkanSwapchain
	{
	public:
		VulkanSwapchain() = default;
		~VulkanSwapchain() = default;

		VulkanSwapchain(const VulkanSwapchain&) = delete;
		VulkanSwapchain& operator=(const VulkanSwapchain&) = delete;

		/**
		 * Creates swapchain
		 *
		 * @param window GLFW window
		 * @param device Vulkan device
		 * @param surface Vulkan surface
		 * @param physicalDevice Vulkan physical device
		 * @param graphicsFamilyIndex Graphical queue family index
		 * @param presentFamilyIndex Present queue family index
		 */
		void create(
			GLFWwindow* window, const vk::raii::Device& device, vk::SurfaceKHR surface, const vk::raii::PhysicalDevice&
			physicalDevice, uint32_t graphicsFamilyIndex, uint32_t presentFamilyIndex
		);

		vk::raii::SwapchainKHR& getSwapchain() noexcept { return m_swapChain; }

		vk::Extent2D& getExtent() noexcept { return m_swapChainExtent; }

		std::vector<vk::Image>& getSwapchainImages() noexcept { return m_swapChainImages; }

		std::vector<vk::raii::ImageView>& getSwapchainImageViews() noexcept { return m_swapChainImageViews; }

		vk::Format& getSwapchainImageFormat() noexcept { return m_swapChainImageFormat; }

	private:
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
		void createSwapChain(
			GLFWwindow* window, const vk::raii::Device& device, vk::SurfaceKHR surface, const vk::raii::PhysicalDevice&
			physicalDevice, uint32_t graphicsFamilyIndex, uint32_t presentFamilyIndex
		);

		/**
		 * Create image views
		 */
		void createImageViews(const vk::raii::Device& device);

		vk::raii::SwapchainKHR m_swapChain = VK_NULL_HANDLE;
		vk::Extent2D m_swapChainExtent;
		std::vector<vk::Image> m_swapChainImages;
		std::vector<vk::raii::ImageView> m_swapChainImageViews;
		vk::Format m_swapChainImageFormat = vk::Format::eUndefined;
	};
}
