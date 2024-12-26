#include "Window.h"
#include "Application.h"
#include <imgui.h>
#include "winuser.h"

namespace sb
{
    bool Window::IsShutdownReserved()
    {
        return m_isWindowShutDownKeyPressed || m_bForceShutDown;
    }

    void Window::UpdateMousePosition(int mouseX, int mouseY)
    {
        if (_bConsumeMouseInput)
        {
            _preMousePos.X = _MousePos.X;
            _preMousePos.Y = _MousePos.Y;

            _MousePos.X = std::max(0, mouseX);
            _MousePos.Y = std::max(0, mouseY);

            _mouseDiff.X = _preMousePos.X - _MousePos.X;
            _mouseDiff.Y = _preMousePos.Y - _MousePos.Y;

            _bConsumeMouseInput = false;
        }
    }

    void Window::Update()
    {
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

    void Window::PostUpdate()
    {
        _bConsumeMouseInput = true;
    }

    void Window::OnWindowSizeChanged(int32 in_width, int32 in_height)
    {
        m_windowData.width = in_width;
        m_windowData.height = in_height;
    }
} // namespace sb