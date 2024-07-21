#include "FrontWindow.h"
#include "imgui.h"

namespace sb
{
    FrontWindow::FrontWindow(const WindowContext& in_windowContext) : WinsWindow(in_windowContext)
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
        return true;
    }

    void FrontWindow::ImGuiUpdate()
    {
        const int32 OpenGL = static_cast<int32>(GraphicsDevice::OpenGL);
        const int32 DirectX12 = static_cast<int32>(GraphicsDevice::DirectX12);

        ImGui::Begin("FrontWindow", 0);
        ImGui::Text("Hello, world!");
        ImGui::RadioButton("DirectX12", &m_driverType, DirectX12);
        ImGui::SameLine();
        ImGui::RadioButton("OpenGL", &m_driverType, OpenGL);
        if (ImGui::Button("Next"))
        {
            m_Next = true;
            m_bForceShutDown = true;
        }
        ImGui::End();
    }
} // namespace sb