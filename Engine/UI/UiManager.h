#pragma once
#include <Renderer/VulkanContext.h>
#include "Core/Window.h"


namespace UI
{
	class UIManager
	{
	public:
		UIManager() = default;
		~UIManager() = default;

		void initImGUI(const std::shared_ptr<Renderer::VulkanContext>& vkContext);

		void setWindow(const std::shared_ptr<Core::Window>& window);

	private:
		std::shared_ptr<Core::Window> m_window;
		std::shared_ptr<Renderer::VulkanContext> m_vkContext;
	};
}
