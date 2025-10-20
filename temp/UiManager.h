#pragma once
#include <../../temp/VulkanContext.h>
#include "Core/window.h"


namespace UI
{
	class UIManager
	{
	public:
		UIManager() = default;
		~UIManager() = default;

		void initImGUI(const Renderer::VulkanContext& vkContext);

		void setWindow(const std::shared_ptr<Application::Window>& window);

	private:
		std::shared_ptr<Application::Window> m_window;
		std::shared_ptr<Renderer::VulkanContext> m_vkContext;
	};
}
