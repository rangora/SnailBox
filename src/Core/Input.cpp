#include "Input.h"
#include "Application.h"
#include "Core/Canvas.h"
#include "Core/Window.h"
#include <GLFW/glfw3.h>

namespace sb
{
    std::pair<double, double> Input::GetMousePosition()
    {
        Window& window = Application::Get().GetFocusWindow();

        if (window.IsOpenglWindow())
        {
            auto canvas = window.m_canvas.get();
            GLFWwindow* glfwWindow = static_cast<GLFWwindow*>(canvas->GetNativeWindow());
            double x = 0.0, y = 0.0;
            glfwGetCursorPos(glfwWindow, &x, &y);
            return {(double)x, (double)y};
        }

        return std::pair<double, double>(0.0, 0.0);
    }

    double Input::GetMouseX()
    {
        auto [x, y] = GetMousePosition();
        return x;
    }

    double Input::GetMouseY()
    {
        auto [x, y] = GetMousePosition();
        return y;
    }
} // namespace sb