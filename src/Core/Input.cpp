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

    bool Input::IsMouseButtonDown(MouseButton in_button)
    {
        Window& window = Application::Get().GetFocusWindow();

        if (window.IsOpenglWindow())
        {
            auto canvas = window.m_canvas.get();
            GLFWwindow* glfwWindow = static_cast<GLFWwindow*>(canvas->GetNativeWindow());
            int state = glfwGetMouseButton(glfwWindow, static_cast<int32>(in_button));
            return state == GLFW_PRESS;
        }

        return false;
    }

    void Input::TransitionPressedButtons()
    {
        for (const auto& [button, buttonData] : s_mouseData)
        {
            if (buttonData.m_state == KeyState::Pressed)
            {
                UpdateButtonState(button, KeyState::Held);
            }
        }
    }

    void Input::UpdateButtonState(MouseButton in_button, KeyState in_state)
    {
        auto& mouseData = s_mouseData[in_button];
        mouseData.m_button = in_button;
        mouseData.m_oldState = mouseData.m_state;
        mouseData.m_state = in_state;
    }

    void Input::ClearReleasedKeys()
    {
        for (const auto& [button, buttonData] : s_mouseData)
        {
            if (buttonData.m_state == KeyState::Released)
            {
                UpdateButtonState(button, KeyState::None);
            }
        }
    }
} // namespace sb