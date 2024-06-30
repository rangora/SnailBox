#include "FrontWindow.h"
#include "imgui.h"

namespace sb
{
    FrontWindow::FrontWindow(const WindowContext& in_windowContext) : WinsWindow(in_windowContext)
    {
    }

    FrontWindow::FrontWindow(const WindowContext& in_windowContext, Application* in_app)
        : WinsWindow(in_windowContext, in_app)
    {
    }

    bool FrontWindow::InitializeFrontWindow()
    {
        m_wc = {sizeof(m_wc), CS_CLASSDC,         WndProc, 0L, 0L, GetModuleHandle(nullptr), nullptr, nullptr, nullptr,
                nullptr,      L"FrontMenuWindow", nullptr};
        ::RegisterClassExW(&m_wc);

        m_hwnd = ::CreateWindowW(m_wc.lpszClassName, L"FrontWindow", WS_OVERLAPPEDWINDOW, 100, 100, 1280, 800, nullptr,
                                 nullptr, m_wc.hInstance, nullptr);

        return true;
    }

    bool FrontWindow::BindDirect3dDriver()
    {
        
        return false;
    }

    void FrontWindow::ImGuiUpdate()
    {
        ImGui::Begin("FrontWindow", 0);
        ImGui::Text("Hello, world!");
        ImGui::RadioButton("DirectX12", &m_driverType, 0);
        ImGui::SameLine();
        ImGui::RadioButton("OpenGL", &m_driverType, 1);
        if (ImGui::Button("Next"))
        {
            m_bNext = true;
        }
        ImGui::End();
    }
} // namespace sb