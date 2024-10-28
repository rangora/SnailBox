#pragma once

#include "Direct3dDriver.h"
#include "FrontWindow.h"
#include "Render/ShaderArchive.h"
#include "Window.h"
#include "corepch.h"
#include <boost/container/flat_map.hpp>
#include <vector>

#define sg_d3dDevice sb::Application::GetD3Device()
#define sg_d3dDriver sb::Application::GetDirect3dDriver()

namespace sb
{
    class FrontWindow;

    struct ApplicationSpec
    {
        bool _pix = false;
    };

    class Application
    {
    public:
        Application(const ApplicationSpec& spec);
        Application(const Application&) = delete;
        Application& operator=(const Application&) = delete;
        virtual ~Application();

        void Run();

        Window& GetOpenglWindow() { return *(m_windows[openglWindowTitle]); }
        Window& GetDirectXWindow();
        Window& GetFocusWindow();
        static Application& Get() { return *s_instance; }

        void CreateAppWindow(const WindowContext& in_windowContext);

        static ComPtr<ID3D12Device> GetD3Device();
        static class Direct3dDriver* GetDirect3dDriver();

        static ShaderArchive s_staticShaderArchive;
        static GraphicsDevice s_currentGraphicsDevice;

    private:
        void OpenFrontWindow(WindowContext& OutWindowContext);

        void InitializeDirect3dDriver();
        void CleanGraphicsDriver();

        void PreProcessOnFrame();

        boost::container::flat_map<std::string, UPtr<Window>> m_windows;

        static int32_t m_runningWindowCount;
        static Application* s_instance;

        bool m_isRunningFrontWindow = false;
        bool m_isRunningDirectXWindow = false;

        // Graphics drivers
        UPtr<Direct3dDriver> m_d3dDriver = nullptr;

        UPtr<FrontWindow> m_frontWindow = nullptr;
    };
} // namespace sb