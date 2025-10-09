#include "Application.h"

namespace Core
{
	void Application::run()
	{
		init();
		mainLoop();
		shutdown();
	}

	void Application::init()
	{
		const std::vector<Renderer::Vertex> vertices = {
			{{-0.9f, -0.9f}, {1.0f, 0.0f, 0.0f}},
			{{0.9f, -0.9f}, {0.0f, 1.0f, 0.0f}},
			{{0.9f, 0.9f}, {0.0f, 0.0f, 1.0f}},
			{{-0.9f, 0.9f}, {1.0f, 1.0f, 1.0f}}
		};

		const std::vector<uint16_t> indices = {
			0, 1, 2, 2, 3, 0
		};

		m_window = std::make_shared<Core::Window>();
		m_renderer = std::make_unique<Renderer::VulkanContext>();

		m_uiManager = std::make_unique<UI::UIManager>();

		m_window->create({800, 600, "Endura"});

		m_uiManager->setWindow(m_window);
		m_uiManager->initImGUI(*m_renderer);

		m_renderer->fillVertices(vertices, indices);
		m_renderer->InitializeVulkan(m_window->getGLFWWindow());
	}

	void Application::mainLoop()
	{
		while(true)
		{
			while(!m_window->shouldClose())
			{

				m_window->pollEvents();
				m_renderer->drawFrame();

				m_frameCounter++;
			}
			// We can have the exit logic here
			printf("Average FPS: %f\n", m_frameCounter / glfwGetTime());
			break;
		}
	}

	void Application::shutdown()
	{
		m_renderer->Cleanup();
	}
}
