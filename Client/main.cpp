#include <iostream>
#include <cmath>
#include <Core/Window.h>
#include <Renderer/VulkanContext.h>

int main()
{
	uint32_t frames = 0;
	float timer = 0.0f;

	std::vector<Renderer::Vertex> vertices;
	for(int i = 0; i < 6; ++i)
	{
		constexpr float radius = 0.6f;
		const float angle0 = glm::radians(60.0f * i);
		const float angle1 = glm::radians(60.0f * (i + 1));
		float x0 = cos(angle0) * radius;
		float y0 = sin(angle0) * radius;
		float x1 = cos(angle1) * radius;
		float y1 = sin(angle1) * radius;

		vertices.push_back({{0.0f, 0.0f}, {0.3f, 0.3f, 0.3f}});
		vertices.push_back({{x0, y0}, {fabs(x0), fabs(y0), 1.0f - fabs(y0)}});
		vertices.push_back({{x1, y1}, {fabs(x1), fabs(y1), 1.0f - fabs(x1)}});
	}

	Core::Window window;
	Renderer::VulkanContext vkContext;
	window.create({});
	vkContext.fillVertices(vertices);
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
