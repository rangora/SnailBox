#define GLFW_INCLUDE_NONE
#include "Application.h"

#include "Direct3dDriver.h"
#include "Enums.h"
#include "GlfwWindow.h"
#include "Input.h"
#include "Render/ShaderCompiler.h"
#include "WinWindow.h"
#include "spdlog/spdlog.h"

namespace sb
{
    int32_t Application::m_runningWindowCount = 0;
    Application* Application::s_instance = nullptr;
    ShaderArchive Application::s_staticShaderArchive(GraphicsDevice::OpenGL);
    GraphicsDevice Application::s_currentGraphicsDevice = GraphicsDevice::None;

    Application::Application(const ApplicationSpec& spec)
    {
        if (spec._pix)
        {
            HMODULE hModule =
                LoadLibraryA("C:/Program Files/Microsoft PIX/2405.15.002-OneBranch_release/WinPixGpuCapturer.dll");
            if (hModule == nullptr)
            {
                spdlog::warn("Failed to load pix library.");
            }
        }

        // s_instance에 assert 필요!
        s_instance = this;

        InitializeDirect3dDriver();

        // ShaderPreCompiler list
        s_staticShaderArchive.Add("../resources/shader/simple.vs");
        s_staticShaderArchive.Add("../resources/shader/simple.fs");

        // 1) 쉐이더 컴파일
        ShaderCompiler::Compile(s_staticShaderArchive);

        WindowContext RandomWindowContext;
        OpenFrontWindow(RandomWindowContext);
        CreateAppWindow(RandomWindowContext);
    }

    void Application::CreateAppWindow(const WindowContext& in_windowContext)
    {
        if (in_windowContext.graphicsDevice == GraphicsDevice::None)
        {
            spdlog::info("FrontWindow closed.");
            return;
        }

        Application::s_currentGraphicsDevice = in_windowContext.graphicsDevice;

        spdlog::info("Add new window:{}, height:{}, width:{}", in_windowContext.title, in_windowContext.height,
                     in_windowContext.width);

        if (in_windowContext.graphicsDevice == GraphicsDevice::OpenGL)
        {
            auto glfwWindow = CreateUPtr<GlfwWindow>(in_windowContext);
            m_windows.insert({in_windowContext.title, std::move(glfwWindow)});
            if (!m_windows[in_windowContext.title]->BindGraphicsDriver())
            {
                spdlog::error("Failed to createWindow during initOpenglDevice.");
                return;
            }
        }
        else if (in_windowContext.graphicsDevice == GraphicsDevice::DirectX12)
        {
            m_isRunningDirectXWindow = true;
            InitializeDirect3dDriver(); // stage 01
            auto winsWindow = CreateUPtr<WinsWindow>(in_windowContext);
            if (!winsWindow->BindGraphicsDriver())
            {
                spdlog::error("Failed to createWindow during initDirectXDevice.");
                return;
            }

            m_windows.insert({in_windowContext.title, std::move(winsWindow)});
        }

        m_runningWindowCount++;
    }

    ComPtr<ID3D12Device> Application::GetD3Device()
    {
        Application& app = Get();
        return app.m_d3dDriver->GetDevice();
    }

    Direct3dDriver* Application::GetDirect3dDriver()
    {
        Application& app = Get();
        return app.m_d3dDriver.get();
    }

    void Application::OpenFrontWindow(WindowContext& OutWindowContext)
    {
        m_isRunningFrontWindow = true;
        WindowContext FrontWindowContext;
        FrontWindowContext.title = directXWindowTitle;
        FrontWindowContext.graphicsDevice = GraphicsDevice::DirectX12;
        m_frontWindow = CreateUPtr<FrontWindow>(FrontWindowContext);
        m_frontWindow->BindGraphicsDriver();
        while (!m_frontWindow->IsReadyWindowShutdown())
        {
            m_frontWindow->Update();
        }

        if (m_frontWindow->m_Next)
        {
            OutWindowContext.graphicsDevice = static_cast<GraphicsDevice>(m_frontWindow->m_driverType);
            OutWindowContext.title =
                OutWindowContext.graphicsDevice == GraphicsDevice::DirectX12 ? directXWindowTitle : openglWindowTitle;
        }
        m_isRunningFrontWindow = false;
    }

    void Application::InitializeDirect3dDriver()
    {
        m_d3dDriver = CreateUPtr<Direct3dDriver>();
        m_d3dDriver->InitD3dDevice();
    }

    void Application::CleanGraphicsDriver()
    {
        m_d3dDriver.reset();
    }

    void Application::PreProcessOnFrame()
    {
        Input::ClearReleasedKeys();
        Input::TransitionPressedButtons();
    }

    Application::~Application()
    {
        CleanGraphicsDriver();
    }

    void Application::Run()
    {
        while (m_runningWindowCount)
        {
            PreProcessOnFrame();

            std::queue<std::string> deadWindows;

            for (auto& [name, window] : m_windows)
            {
                if (window->IsReadyWindowShutdown())
                {
                    deadWindows.emplace(name);
                    continue;
                }

                window->Update();
            }

            while (!deadWindows.empty())
            {
                m_windows.erase(deadWindows.front());
                deadWindows.pop();
                m_runningWindowCount--;
            }
        }
    }

    Window& Application::GetDirectXWindow()
    {
        if (m_windows.size())
        {
            return *(m_windows[directXWindowTitle]);
        }

        return *m_frontWindow.get();
    }

    Window& Application::GetFocusWindow()
    {
        if (m_isRunningFrontWindow)
        {
            return *m_frontWindow.get();
        }

        const std::string key = m_isRunningDirectXWindow ? directXWindowTitle : openglWindowTitle;
        return *m_windows.find(key).get_ptr()->second;
    }
} // namespace sb