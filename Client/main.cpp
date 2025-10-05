#include <iostream>
#include <cmath>
#include <Core/Window.h>
#include <Renderer/VulkanContext.h>

int main() {
    Core::Window window;
    Renderer::VulkanContext vkContext;
    window.create({});
    vkContext.InitializeVulkan(window.getGLFWWindow());

    uint32_t frames = 0;
    float timer = 0.0f;

    while (true)
    {
        while (!window.shouldClose())
        {
            const double timeStart = glfwGetTime();

            window.pollEvents();
            vkContext.drawFrame(timeStart);
            frames++;

            timer += glfwGetTime() - timeStart;

            if (timer >= 1.0f)
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