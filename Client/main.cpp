#include <iostream>
#include <cmath>
#include <Core/Window.h>
#include <Renderer/VulkanContext.h>

int main()
{
	uint32_t frames = 0;
	float timer = 0.0f;

	const std::vector<Renderer::Vertex> vertices = {
		{{-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}},
		{{0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}},
		{{0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},
		{{-0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}}
	};

	const std::vector<uint16_t> indices = {
		0, 1, 2, 2, 3, 0
	};

	Core::Window window;
	Renderer::VulkanContext vkContext;
	window.create({});
	vkContext.fillVertices(vertices, indices);
	vkContext.InitializeVulkan(window.getGLFWWindow());


	while(true)
	{
		while(!window.shouldClose())
		{
			const double timeStart = glfwGetTime();

			window.pollEvents();
			vkContext.drawFrame(timeStart);
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

	vkContext.Cleanup();
}
