#include "Window.h"

#include <GLFW/glfw3.h>
#include <imgui_impl_glfw.h>

namespace sb
{

    void Window::OnKeyEvent(GLFWwindow* in_window, int in_key, int in_scancode, int in_action, int in_mods)
    {
        ImGui_ImplGlfw_KeyCallback(in_window, in_key, in_scancode, in_action, in_mods);

        // 원래 입력은 Queue에 모아서 처리해야 한다.
        // 지금은 여기서 바로 처리하도록 함.
        switch (in_action)
        {
            case GLFW_PRESS:
            {
                if (in_key == GLFW_KEY_ESCAPE)
                {
                    glfwSetWindowShouldClose(in_window, true);
                }
            }
        }
    }
    
    void Window::OnCharEvent(GLFWwindow* in_window, uint32 in_ch)
    {
        ImGui_ImplGlfw_CharCallback(in_window, in_ch);
    }

    void Window::OnCursorPos(GLFWwindow* in_window, double in_x, double in_y)
    {
        Window* WindowPtr = (Window*)glfwGetWindowUserPointer(in_window);
        WindowPtr->MouseMove(in_x, in_y);
    }

    void Window::OnMouseButton(GLFWwindow* in_window, int32 in_button, int32 in_action, int32 in_modifier)
    {
        ImGui_ImplGlfw_MouseButtonCallback(in_window, in_button, in_action, in_modifier);
        Window* WindowPtr = (Window*)glfwGetWindowUserPointer(in_window);
        double x, y;
        glfwGetCursorPos(in_window, &x, &y);
        WindowPtr->MouseButton(in_button, in_action, x, y);
    }

    void Window::OnScroll(GLFWwindow* in_window, double in_x_offset, double in_y_offset)
    {
        ImGui_ImplGlfw_ScrollCallback(in_window, in_x_offset, in_y_offset);
    }

    // UPtr<Window> Window::Create(const WindowContext& arg_context)
    // {
    //     return UPtr<Window>();
    // }
}