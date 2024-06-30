#define GLFW_INCLUDE_NONE
#include "WinWindow.h"
#include "Application.h"
#include "Direct3dDriver.h"
#include "Input.h"
#include "OpenglDriver.h"
#include "Render/OpenGL/OpenGLContext.h"
#include "imgui_impl_dx12.h"
#include "imgui_impl_win32.h"
#include "spdlog/spdlog.h"
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <tchar.h>
#include <vector>

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

                auto swapChain = sg_d3dDriver->GetSwapChain();
                if (swapChain == nullptr)
                    return 0;

                auto swapChain3 = swapChain->GetSwapChain3();
                if (swapChain3 == nullptr)
                    return 0;

                HWND pHwnd;
                swapChain3->GetHwnd(&pHwnd);

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
            ::PostQuitMessage(0);
            return 0;
        }
    }

    return ::DefWindowProcW(hWnd, msg, wParam, lParam);
}

void framebuffer_size_callback(GLFWwindow* InWindow, int InWidth, int InHeight)
{
    glViewport(0, 0, InWidth, InHeight);
}

namespace sb
{
    WinsWindow::WinsWindow(const WindowContext& in_windowContext)
    {
        m_windowData.height = in_windowContext.height;
        m_windowData.width = in_windowContext.width;
        m_windowData.title = in_windowContext.title;
    }

    WinsWindow::WinsWindow(const WindowContext& in_windowContext, Application* in_app)
    {
        m_windowData.height = in_windowContext.height;
        m_windowData.width = in_windowContext.width;
        m_windowData.title = in_windowContext.title;
        m_app = in_app;
    }

    WinsWindow::~WinsWindow()
    {
        ShutDown();
    }

    void WinsWindow::Update()
    {
        if(!m_driver->IsWindowShouldClosed())
        {
            ProcessInput();

            MSG msg;
            while (::PeekMessage(&msg, nullptr, 0U, 0U, PM_REMOVE))
            {
                ::TranslateMessage(&msg);
                ::DispatchMessage(&msg);
                if (msg.message == WM_QUIT)
                {
                    // 종료
                    break;
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
            // TEMP
            m_app->DestroyAppWindow();
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

    bool WinsWindow::InitializeWithOpenglDevice()
    {
        m_driver = new OpenglDriver(this);
        if (!m_driver->InitDriver(&m_windowData))
        {
            spdlog::error("Failed init openglDriver.");
            return false;
        }

        AttachLayout(m_driver);

        m_nativeWindow = static_cast<GLFWwindow*>(m_driver->GetNativeWindow());
        if (m_nativeWindow == nullptr)
        {
            return false;
        }

        m_imguiContext = ImGui::CreateContext();
        ImGui::SetCurrentContext(m_imguiContext);

        // config초기화는 Impl초기화 이전에 해줘야 한다.
        ImGuiIO& io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

        const char* glsl_version = "#version 460";
        ImGui_ImplGlfw_InitForOpenGL(m_nativeWindow, false);
        ImGui_ImplOpenGL3_Init(glsl_version);
        ImGui_ImplOpenGL3_CreateFontsTexture();
        ImGui_ImplOpenGL3_CreateDeviceObjects();

        glfwSetWindowUserPointer(m_nativeWindow, this);

        // Register callbacks
        glfwSetFramebufferSizeCallback(m_nativeWindow, OnFreamBufferSizeChanged);

        // input callback binding은 init()에서 해준다.
        // Key input
        glfwSetKeyCallback(m_nativeWindow, OnKeyEvent);

        // 문자가 입력됬을 때 호출되는 callback.
        glfwSetCharCallback(m_nativeWindow, OnCharEvent);

        // Mouse input
        glfwSetCursorPosCallback(m_nativeWindow, OnCursorPos);
        glfwSetMouseButtonCallback(m_nativeWindow, OnMouseButton);
        glfwSetScrollCallback(m_nativeWindow, OnScroll);

        m_isOpenglWindow = true;

        return true;
    }

    bool WinsWindow::InitializeWithDirectXDevice()
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

    void WinsWindow::ProcessInput()
    {
        if(!m_cameraTranslation)
        {
            return;
        }

        const float cameraSpeed = 0.05f;
        if (glfwGetKey(m_nativeWindow, GLFW_KEY_W) == GLFW_PRESS)
            m_cameraPos += cameraSpeed * m_cameraFront;
        if (glfwGetKey(m_nativeWindow, GLFW_KEY_S) == GLFW_PRESS)
            m_cameraPos -= cameraSpeed * m_cameraFront;

        auto cameraRight = glm::normalize(glm::cross(m_cameraUp, -m_cameraFront));
        if (glfwGetKey(m_nativeWindow, GLFW_KEY_D) == GLFW_PRESS)
            m_cameraPos += cameraSpeed * cameraRight;
        if (glfwGetKey(m_nativeWindow, GLFW_KEY_A) == GLFW_PRESS)
            m_cameraPos -= cameraSpeed * cameraRight;

        auto cameraUp = glm::normalize(glm::cross(-m_cameraFront, cameraRight));
        if (glfwGetKey(m_nativeWindow, GLFW_KEY_E) == GLFW_PRESS)
            m_cameraPos += cameraSpeed * cameraUp;
        if (glfwGetKey(m_nativeWindow, GLFW_KEY_Q) == GLFW_PRESS)
            m_cameraPos -= cameraSpeed * cameraUp;
    }

    void WinsWindow::MouseMove(double in_x, double in_y)
    {
        if (!m_cameraTranslation)
        {
            return;
        }

        auto pos = glm::vec2((float)in_x, (float)in_y);
        auto deltaPos = pos - m_preMousePos;

        const float cameraRotSpeed = 0.8f;
        m_cameraYaw -= deltaPos.x * cameraRotSpeed;
        m_cameraPitch -= deltaPos.y * cameraRotSpeed;

        if (m_cameraYaw < 0.0f)
            m_cameraYaw += 360.0f;
        if (m_cameraYaw > 360.0f)
            m_cameraYaw -= 360.0f;

        if (m_cameraPitch > 89.0f)
            m_cameraPitch = 89.0f;
        if (m_cameraPitch < -89.0f)
            m_cameraPitch = -89.0f;

        m_preMousePos = pos;
    }

    void WinsWindow::MouseButtonAction(int32 in_button, int32 in_action, double in_x, double in_y)
    {
        if (in_button == GLFW_MOUSE_BUTTON_RIGHT)
        {
            if (in_action == GLFW_PRESS)
            {
                m_cameraTranslation = true;
                m_preMousePos = glm::vec2((float)in_x, (float)in_y);
            }
            else if (in_action == GLFW_RELEASE)
            {
                m_cameraTranslation = false;
            }
        }

        // new
        switch (in_action)
        {
            case GLFW_PRESS:
            {
                Input::UpdateButtonState((MouseButton)in_button, KeyState::Pressed);
                break;
            }
            case GLFW_RELEASE:
            {
                Input::UpdateButtonState((MouseButton)in_button, KeyState::Released);
                break;
            }
        }
    }

    ComPtr<ID3D12Device> WinsWindow::GetD3dDevice()
    {
        Direct3dDriver* driver = static_cast<Direct3dDriver*>(m_driver);

        return driver == nullptr ? nullptr : driver->GetDevice();
    }

    void WinsWindow::GetMousePos(double& out_x, double& out_y)
    {
        if (IsOpenglWindow())
        {
            glfwGetCursorPos(m_nativeWindow, &out_x, &out_y);
        }
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

    void WinsWindow::ShutDown()
    {
        if (m_isOpenglWindow)
        {
            ImGui_ImplOpenGL3_DestroyFontsTexture();
            ImGui_ImplOpenGL3_DestroyDeviceObjects();
            ImGui_ImplOpenGL3_Shutdown();
            ImGui_ImplGlfw_Shutdown();
            ImGui::DestroyContext(m_imguiContext);
            glfwTerminate();
        }
    }
} // namespace sb
