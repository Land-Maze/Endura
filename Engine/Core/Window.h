#pragma once

#include <memory>
#include <vulkan/vulkan.h>
#define GLFW_INCLUDE_VULKAN
#include <optional>
#include <GLFW/glfw3.h>

#define WINDOW_16_10_RATIO 1.6f

namespace Core
{
    struct DestroyGLFWWindow{
        void operator()(GLFWwindow* ptr) const
        {
            if (ptr) glfwDestroyWindow(ptr);
        }
    };
    typedef std::unique_ptr<GLFWwindow, DestroyGLFWWindow> Smart_GLFWWindow;

    class Window
    {
    public:
        Window();
        ~Window();


        /**
         * @param width Width of the window (excluding DPI scaling)
         * @param height of the window (excluding DPI scaling)
         * @param title Title of the window
         */
        struct WindowParams
        {
            int width = 800;
            int height = 600;
            std::string title = "Window";

        };

        /**
         * Creates a GLFW window with the specified title and dimensions.
         *
         * @param window_params Struct that includes title, width, height
         */
        void create(const WindowParams& window_params);

        /**
         * Polls events from the windowing system.
         * Such as keyboard and mouse input, window resize, etc.
         */
        void pollEvents() const;

        /**
         * Toggles fullscreen mode for the window.
         * It will delete the current window and create a new one in fullscreen or windowed mode.
         */
        void toggleFullscreen();

        /**
         * Checks if the window should close.
         * For example, if the user has clicked the close button.
         * Or if the application has requested to close the window (e.g. via ctrl+c or alt+f4).
         *
         * @return True if the window should close, False otherwise.
         */
        [[nodiscard]] bool shouldClose() const;

        /**
         * This function could be used to either request the window to close (e.g. user pressed Exit button)
         * or to cancel a previous close request (e.g. user pressed Cancel button in a confirmation dialog).
         *
         * Still, it would not close the window immediately, but set a flag that would be checked in the main loop.
         *
         * @param value True to request the window to close, false to keep it open.
         */
        void setShouldClose(bool value);

        /**
         * Destroys the window and releases resources related to it.
         * After calling this function, the window object should not be used anymore.
         */
        void close();

        /**
         *  Checks if the window is still running.
         *
         *  @return True if window is still running, False otherwise.
         */
        [[nodiscard]] bool isRunning() const;

        /**
         * Sets the title of the window.
         *
         * @param title New title for the window.
         */
        void setTitle(const std::string& title);

        /**
         * Sets the dimensions of the window.
         * DPI scaling is taken into account internally, example:
         * setDimensions(800, 600) on a 2x DPI scaling monitor will create a window of 1600x1200 actual pixels.
         *
         * @param width New width for the window in pixels (excluding DPI scaling).
         * @param height New height for the window in pixels (excluding DPI scaling).
         */
        void setDimensions(int width, int height);

        /**
         *
         * @return Returns a pointer to the underlying GLFW window object.
         */
        [[nodiscard]] GLFWwindow* getGLFWWindow() const;

        /**
         * Gets the dimensions of the window.
         * DPI scaling is taken into account internally, example:
         * setDimensions(800, 600) on a 2x DPI scaling monitor will create a window of 1600x1200 actual pixels.
         *
         * @return Pair of width and height
         */
        [[nodiscard]] std::pair<int, int> getDimensions() const;

    private:
        Smart_GLFWWindow _window = nullptr;
        GLFWmonitor* _monitor = nullptr;

        WindowParams _window_params;

        float _monitorRatio = WINDOW_16_10_RATIO;

        bool _isFullscreen = false;
    };
}
