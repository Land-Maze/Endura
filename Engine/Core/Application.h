#pragma once
#include <memory>

#include <Renderer/VulkanContext.h>
#include <UI/UiManager.h>

namespace Core
{
	class Application
	{
	public:
		/**
		 * Highest level of abstraction class to run the application
		 */
		void run();
	private:
		void init();
		void mainLoop();
		void shutdown();
		std::shared_ptr<Core::Window> m_window;
		std::unique_ptr<Renderer::VulkanContext> m_renderer;
		std::unique_ptr<UI::UIManager> m_uiManager;

		uint32_t m_frameCounter = 0;
	};
}
