#include "WinWindow.h"
#include "Application.h"
#include "Direct3dDriver.h"
#include "Input.h"
#include "imgui_impl_dx12.h"
#include "imgui_impl_win32.h"
#include "spdlog/spdlog.h"
#include <tchar.h>
#include <vector>

sb::Window* destroy_marked_window = nullptr;

// forward declare
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
    {
        return true;
    }

    switch (msg)
    {
        case WM_SIZE:
        {
            if (sg_d3dDevice && wParam != SIZE_MINIMIZED)
            {
                sg_d3dDriver->WaitForLastSubmittedFrame();
                sg_d3dDriver->CleanUpRenderTarget();

                HRESULT result = sg_d3dDriver->GetSwapChain()->GetSwapChain3()->ResizeBuffers(
                    0, (UINT)LOWORD(lParam), (UINT)HIWORD(lParam), DXGI_FORMAT_UNKNOWN,
                    DXGI_SWAP_CHAIN_FLAG_FRAME_LATENCY_WAITABLE_OBJECT);

                assert(SUCCEEDED(result) && "Failed to reszie swapchain.");
                sg_d3dDriver->CreateRenderTarget();
            }
            return 0;
        }
        case WM_SYSCOMMAND:
        {
            // Disable ALT application menu
            if ((wParam & 0xfff0) == SC_KEYMENU)
            {
                return 0;
            }
            break;
        }

        case WM_KEYDOWN:
        {
            if (wParam == VK_ESCAPE)
            {
                if (sb::Input::IsKeyButtonDown(sb::KeyButton::Esc))
                {
                    break;
                }

                sb::Input::UpdateKeyState(sb::KeyButton::Esc, sb::KeyState::Pressed);
            }
            break;
        }

        case WM_KEYUP:
        {
            if (wParam == VK_ESCAPE)
            {
                if (sb::Input::IsKeyButtonReleased(sb::KeyButton::Esc))
                {
                    break;
                }

                sb::Input::UpdateKeyState(sb::KeyButton::Esc, sb::KeyState::Released);
            }
            break;
        }
        case WM_DESTROY:
        {
            destroy_marked_window = &sb::Application::Get().GetFocusWindow();
            ::PostQuitMessage(0);
            return 0;
        }
    }

    return ::DefWindowProcW(hWnd, msg, wParam, lParam);
}

namespace sb
{
    WinsWindow::WinsWindow(const WindowContext& in_windowContext)
    : Window(in_windowContext)
    {
        m_windowData.height = in_windowContext.height;
        m_windowData.width = in_windowContext.width;
        m_windowData.title = in_windowContext.title;
    }

    WinsWindow::~WinsWindow()
    {
    }

    void WinsWindow::Update()
    {
        if (!IsShutdownReserved())
        {
            ProcessWinInput();

            MSG msg;
            while (::PeekMessage(&msg, nullptr, 0U, 0U, PM_REMOVE))
            {
                ::TranslateMessage(&msg);
                ::DispatchMessage(&msg);
                if (msg.message == WM_QUIT)
                {
                    if (destroy_marked_window == this)
                    {
                        // 종료
                        break;
                    }
                }

                // Imgui update
                ImGui_ImplDX12_NewFrame();
                ImGui_ImplWin32_NewFrame();
                ImGui::NewFrame();
                ImGuiUpdate();
                ImGui::Render();

                // Driver update
                m_driver->Update();
                m_driver->SwapBuffers();
            }

            }
        else
        {
            ReadyWindowShutdown();
        }
    }

    void WinsWindow::ImGuiUpdate()
    {
        static ImVec4 clearColor;

        ImGui::Begin("Hello, world!");

        ImGui::Text("This is some useful text.");
        ImGui::SliderFloat("float", &m_f, 0.0f, 1.0f);
        ImGui::ColorEdit3("clear color", (float*)&clearColor);
        if (ImGui::Button("Button"))
            m_counter++;
        ImGui::SameLine();
        ImGui::Text("counter = %d", m_counter);

        ImGui::End();

        m_clearColor = {clearColor.x, clearColor.y, clearColor.z};

        sg_d3dDriver->EnqueueImGuiProperty({m_clearColor});
    }

    bool WinsWindow::InitializeWindows(const std::string& in_menuName, const std::string& in_className)
    {
        std::wstring menuWidStr(in_menuName.begin(), in_menuName.end());
        std::wstring classWidStr(in_className.begin(), in_className.end());

        m_wc = {sizeof(m_wc), CS_CLASSDC, WndProc, 0L,      0L,         GetModuleHandle(nullptr),
                nullptr,      nullptr,    nullptr, nullptr, menuWidStr.c_str(), nullptr};
        ::RegisterClassExW(&m_wc);

        m_hwnd = ::CreateWindowW(m_wc.lpszClassName, classWidStr.c_str(), WS_OVERLAPPEDWINDOW, 100, 100, 1280, 800, nullptr,
                                 nullptr, m_wc.hInstance, nullptr);

        SetupImGuiContext();

        return true;
    }

    bool WinsWindow::BindGraphicsDriver()
    {
        if (!InitializeWindows("dirct3d menu", "direct3d window"))
        {
            spdlog::error("Failed to init winswindows.");
            return false;
        }

        m_driver = sg_d3dDriver;
        if (!m_driver->BindWinWindow(m_hwnd))
        {
            spdlog::error("Failed init Direct3dDriver.");
            return false;
        }

        AttachLayout(m_driver);

        return true;
    }

    bool WinsWindow::InitializeDriver()
    {
        return false;
    }

    void WinsWindow::OnWindowSizeChanged(int32 in_width, int32 in_height)
    {
        m_windowData.width = in_width;
        m_windowData.height = in_height;
        glViewport(0.f, 0.f, m_windowData.width, m_windowData.height);
    }

    void WinsWindow::ProcessWinInput()
    {
        auto pressedAction = [this]() { m_isWindowShutDownKeyPressed = true; };
        auto heldAction = []() {};
        auto releasedAction = []() {};

        Input::KeyInputAction(pressedAction, KeyState::Pressed, KeyButton::Esc);
        Input::KeyInputAction(heldAction, KeyState::Held, KeyButton::Esc);
        Input::KeyInputAction(releasedAction, KeyState::Released, KeyButton::Esc);
    }

    void WinsWindow::AttachLayout(Layout* in_layout)
    {
        m_layouts.emplace_back(in_layout);
    }

    void WinsWindow::SetupImGuiContext()
    {
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
        io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

        ImGui::StyleColorsDark();
    }

    void WinsWindow::ReadyWindowShutdown()
    {
        m_driver->CleanDriver();

        ImGui::DestroyContext();
        ::DestroyWindow(m_hwnd);
        ::UnregisterClassW(m_wc.lpszClassName, m_wc.hInstance);

        Window::ReadyWindowShutdown();
    }
} // namespace sb
