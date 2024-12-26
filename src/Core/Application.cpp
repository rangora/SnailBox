#define GLFW_INCLUDE_NONE
#include "Application.h"

#include "Direct3dDriver.h"
#include "Render/GraphicsEnum.h"
#include "Render/RenderResourceStorage.h"
#include "GlfwWindow.h"
#include "Input.h"
#include "WinWindow.h"
#include "spdlog/spdlog.h"
#include "World/Worldinstance.h"

namespace sb
{
    int32_t Application::m_runningWindowCount = 0;
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

        WindowContext windowCtx;
        windowCtx.graphicsDevice = GraphicsDevice::DirectX12;
        windowCtx.title = directXWindowTitle;
        CreateAppWindow(windowCtx);

        ReadyBaseResources();

        // create world instance
        if (!CheckWorldCreationReady())
        {
            assert(false);
            spdlog::error("Can't create world.");
            return;
        }

        s_worldInstnace = new WorldInstance;
    }

    void Application::CreateAppWindow(const WindowContext& in_windowContext)
    {
        if (in_windowContext.graphicsDevice == GraphicsDevice::None)
        {
            spdlog::error("Device is none.");
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

    bool Application::CheckWorldCreationReady()
    {
        return m_runningWindowCount > 0;
    }

    void Application::ReadyBaseResources()
    {
        s_renderResourceStorage = new RenderResourceStorage();

        if (s_currentGraphicsDevice == GraphicsDevice::DirectX12)
        {
            m_d3dDriver->InitializeResources();
        }
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

    sb::Window& Application::GetWinWindow()
{
        return *(m_windows[directXWindowTitle]);
    }

    Window& Application::GetFocusWindow()
    {
        const std::string key = m_isRunningDirectXWindow ? directXWindowTitle : openglWindowTitle;
        return *m_windows.find(key).get_ptr()->second;
    }
} // namespace sb