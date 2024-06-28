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
    class Application
    {
    public:
        Application();
        Application(const Application&) = delete;
        Application& operator=(const Application&) = delete;
        virtual ~Application(){};

        void Run();

        Window& GetOpenglWindow() { return *(m_windows[openglWindowTitle]); }
        Window& GetDirectXWindow() { return *(m_windows[directXWindowTitle]); }
        Window& GetFocusWindow() { return *(m_windows[openglWindowTitle]); } // TEMP
        static Application& Get() { return *s_instance; }

        void CreateAppWindow(const WindowContext& in_windowContext);
        void DestroyAppWindow(); // TEMP

        static ComPtr<ID3D12Device> GetD3Device();
        static class Direct3dDriver* GetDirect3dDriver();

        static ShaderArchive s_staticShaderArchive;

    private:
        void InitializeDirect3dDriver();

        void PreProcessOnFrame();

        boost::container::flat_map<std::string, UPtr<Window>> m_windows;

        static int32_t m_runningWindowCount;
        static Application* s_instance;
        // Graphics drivers
        UPtr<Direct3dDriver> m_d3dDriver = nullptr;
    };
} // namespace sb