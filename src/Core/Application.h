#pragma once

#include <vector>
#include "Common.h"
#include "Window.h"

namespace sb
{
    class Application
    {
    public:
        Application();
        virtual ~Application(){};

        void Run();

        Window& GetWindow() { return *m_window; }
        static Application& Get() { return *s_instance; }

        void CreateLayer();
        void DestroyLayer(); // TEMP

    private:
        UPtr<Window> m_window;
        std::vector<UPtr<Window>> m_windows;

        static int32_t m_runningWindowCount;
        static Application* s_instance;
    };
} // namespace sb