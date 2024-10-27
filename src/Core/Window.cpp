#include "Window.h"
#include "Application.h"
#include <imgui.h>

namespace sb
{
    bool Window::IsShutdownReserved()
    {
        return m_isWindowShutDownKeyPressed || m_bForceShutDown;
    }

    void Window::ImGuiUpdate()
    {
        Application& App = Application::Get();

        if (ImGui::BeginMainMenuBar())
        {
            if (ImGui::BeginMenu("Home"))
            {
                if (ImGui::MenuItem("DirectX12"))
                {
                    // null
                }
                if (ImGui::MenuItem("OpenGL"))
                {
                    // null
                }

                ImGui::EndMenu();
            }
        }

        ImGui::EndMainMenuBar();
    }

    void Window::OnWindowSizeChanged(int32 in_width, int32 in_height)
    {
        m_windowData.width = in_width;
        m_windowData.height = in_height;
    }
} // namespace sb