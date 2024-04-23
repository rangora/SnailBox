#pragma once

#include <vector>
#include "Common.h"
#include "Window.h"
#include "Render/ShaderArchive.h"

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

        Window& GetWindow() { return *m_window; }
        Window& GetOpenglWindow() { return *(m_windows[0]); }
        static Application& Get() { return *s_instance; }

        void CreateLayer();
        void DestroyLayer(); // TEMP

        static ShaderArchive s_staticShaderArchive;

    private:
        UPtr<Window> m_window;
        std::vector<UPtr<Window>> m_windows;

        static int32_t m_runningWindowCount;
        static Application* s_instance;
    };
} // namespace sb