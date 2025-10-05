#include <iostream>
#include <cmath>
#include <Core/Window.h>
#include <Renderer/VulkanContext.h>

#include "UI/UiManager.h"

int main()
{
	uint32_t frames = 0;
	float timer = 0.0f;

	const std::vector<Renderer::Vertex> vertices = {
		{{-0.9f, -0.9f}, {1.0f, 0.0f, 0.0f}},
		{{0.9f, -0.9f}, {0.0f, 1.0f, 0.0f}},
		{{0.9f, 0.9f}, {0.0f, 0.0f, 1.0f}},
		{{-0.9f, 0.9f}, {1.0f, 1.0f, 1.0f}}
	};

	const std::vector<uint16_t> indices = {
		0, 1, 2, 2, 3, 0
	};

	const auto window = std::make_shared<Core::Window>();
	const auto vkContext = std::make_shared<Renderer::VulkanContext>();

	const auto uiManager = std::make_shared<UI::UIManager>();

	window->create({800, 600, "Endura"});

	uiManager->setWindow(window);
	uiManager->initImGUI(vkContext);


	vkContext->fillVertices(vertices, indices);
	vkContext->InitializeVulkan(window->getGLFWWindow());


	while(true)
	{
		while(!window->shouldClose())
		{
			const double timeStart = glfwGetTime();

			window->pollEvents();
			vkContext->drawFrame();
			frames++;

			timer += glfwGetTime() - timeStart;

			if(timer >= 1.0f)
			{
				printf("FPS: %f\n", frames / timer);
				timer = 0.0f;
				frames = 0;
			}
		}
		// We can have the exit logic here
		break;
	}

	vkContext->Cleanup();
}
