#pragma once

#include "Common.h"
#include "Render/ShaderArchive.h"
#include "Window.h"
#include <boost/container/flat_map.hpp>
#include <vector>

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
        Window& GetFocusWindow() { return *(m_windows[openglWindowTitle]); } // TEMP
        static Application& Get() { return *s_instance; }

        void CreateAppWindow(const WindowContext& in_windowContext);
        void DestroyAppWindow(); // TEMP

        static ShaderArchive s_staticShaderArchive;

    private:
        boost::container::flat_map<std::string, UPtr<Window>> m_windows;

        static int32_t m_runningWindowCount;
        static Application* s_instance;
    };
} // namespace sb