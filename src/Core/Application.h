#pragma once

#include "Common.h"
#include "Window.h"

namespace sb
{
    class Application
    {
    public:
        Application();
        virtual ~Application(){};

        Window& GetWindow() { return *m_window; }
        static Application& Get() { return *s_instance; }

        void CreateLayer();

        // private:
        UPtr<Window> m_window;
        static Application* s_instance;
    };
} // namespace sb