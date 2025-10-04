#include "Window.h"

#include <iostream>
#include <ostream>

namespace Core
{
    Window::Window()
    {
        if (!glfwInit())
        {
            throw std::runtime_error(
                "Failed to initialize GLFW: glfwInit returned false. Is GLFW (for correct display manager) installed correctly?");
        }

        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
        glfwWindowHint(GLFW_SCALE_TO_MONITOR, GLFW_TRUE);

        _monitor = glfwGetPrimaryMonitor();
        if (!_monitor)
        {
            throw std::runtime_error(
                "Failed to get primary monitor: glfwGetPrimaryMonitor returned nullptr. Is monitor connected?");
        }
    }

    Window::~Window()
    {
        close();
        glfwTerminate();
    }

    void Window::close()
    {
        if (!_window) return;

        glfwDestroyWindow(_window.release());
    }

    void Window::create(const WindowParams& window_params)
    {
        if (_window)
        {
            std::cout << "Window already created. Skipping create()" << std::endl;
            return;
        }

        const auto mode = glfwGetVideoMode(_monitor);


        if (_isFullscreen)
            _window = Smart_GLFWWindow(glfwCreateWindow(mode->width, mode->height, window_params.title.c_str(),
                                                        _monitor, nullptr));
        else
            _window = Smart_GLFWWindow(glfwCreateWindow(window_params.width, window_params.height,
                                                        window_params.title.c_str(),
                                                        nullptr, nullptr));

        _monitorRatio = static_cast<float>(mode->width) / static_cast<float>(mode->height);

        if (!_window)
            throw std::runtime_error(
                "Unknown reason occurred during the window creation: glfwCreateWindow returned nullptr.");

        _window_params = window_params;
        setTitle(_window_params.title + " [DEBUG]");
    }

    void Window::pollEvents() const
    {
        if (!_window) return;

        glfwPollEvents();
    }

    void Window::toggleFullscreen()
    {
        this->close();
        _isFullscreen = !_isFullscreen;
        this->create(_window_params);
    }

    bool Window::shouldClose() const
    {
        if (!_window) return false;

        return glfwWindowShouldClose(_window.get());
    }

    void Window::setShouldClose(const bool value) const
    {
        if (!_window) return;

        glfwSetWindowShouldClose(_window.get(), value);
    }

    void Window::setTitle(const std::string& title)
    {
        _window_params.title = title;
        if (!_window)
            glfwSetWindowTitle(_window.get(), title.c_str());
    }

    bool Window::isRunning() const
    {
        return _window != nullptr;
    }

    void Window::setDimensions(const int width, const int height)
    {
        if (!_window) return;

        glfwSetWindowSize(_window.get(), width, height);
        _window_params.width = width;
        _window_params.height = height;
    }

    std::pair<int, int> Window::getDimensions() const
    {
        return std::pair<int, int>{_window_params.width, _window_params.height};
    }

    GLFWwindow* Window::getGLFWWindow() const
    {
        return _window.get();
    }
}
