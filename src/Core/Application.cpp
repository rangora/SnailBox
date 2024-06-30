#define GLFW_INCLUDE_NONE
#include "Application.h"

#include "Direct3dDriver.h"
#include "Enums.h"
#include "Input.h"
#include "Render/ShaderCompiler.h"
#include "WinWindow.h"
#include "spdlog/spdlog.h"

namespace sb
{
    int32_t Application::m_runningWindowCount = 0;
    Application* Application::s_instance = nullptr;
    ShaderArchive Application::s_staticShaderArchive(GraphicsDevice::OpenGL);

    Application::Application()
    {
        // s_instance에 assert 필요!
        s_instance = this;

        InitializeDirect3dDriver();

        // ShaderPreCompiler list
        s_staticShaderArchive.Add("../resources/shader/simple.vs");
        s_staticShaderArchive.Add("../resources/shader/simple.fs");

        // 1) 쉐이더 컴파일
        ShaderCompiler::Compile(s_staticShaderArchive);

        // FrontWindow
        WindowContext FrontWindowContext;
        FrontWindowContext.title = directXWindowTitle;
        FrontWindowContext.graphicsDevice = GraphicsDevice::DirectX12;
        m_frontWindow = CreateUPtr<FrontWindow>(FrontWindowContext, this);
        m_frontWindow->InitializeWithDirectXDevice();
        while (!m_frontWindow->m_bNext && !m_frontWindow->IswindowShutDown())
        {
            m_frontWindow->Update();
        }

        // opengl window
        // WindowContext openglWindowContext;
        // openglWindowContext.title = openglWindowTitle;
        // openglWindowContext.graphicsDevice = GraphicsDevice::OpenGL;
        // CreateAppWindow(openglWindowContext);

        // dirctX window
        // WindowContext directXWindowContext;
        // directXWindowContext.title = directXWindowTitle;
        // directXWindowContext.graphicsDevice = GraphicsDevice::DirectX12;
        // CreateAppWindow(directXWindowContext);
    }

    void Application::CreateAppWindow(const WindowContext& in_windowContext)
    {
        if (in_windowContext.title.empty())
        {
            spdlog::error("WindowContextTitle can't be empty for creating window.");
        }

        spdlog::info("Add new window:{}, height:{}, width:{}", in_windowContext.title, in_windowContext.height,
                     in_windowContext.width);
        auto newWindow = CreateUPtr<WinsWindow>(in_windowContext, this);
        m_windows.insert({in_windowContext.title, std::move(newWindow)});

        if (in_windowContext.graphicsDevice == GraphicsDevice::OpenGL)
        {
            if (!m_windows[in_windowContext.title]->InitializeWithOpenglDevice())
            {
                spdlog::error("Failed to createWindow during initOpenglDevice.");
                return;
            }
        }
        else if (in_windowContext.graphicsDevice == GraphicsDevice::DirectX12)
        {
            if (!m_windows[in_windowContext.title]->InitializeWithDirectXDevice())
            {
                spdlog::error("Failed to createWindow during initDirectXDevice.");
                return;
            }
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
                if (window->IswindowShutDown())
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
} // namespace sb