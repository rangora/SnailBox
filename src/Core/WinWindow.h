#pragma once

#include "Window.h"

#include <string>
#include <cstdint>
#include <GLFW/glfw3.h>

namespace sb
{
    class WinsWindow : public Window 
    {
        public:
           WinsWindow(const WindowContext& arg_WindowContext);
           virtual ~WinsWindow();

       private:
           struct WinWindowData
           {
               std::string title;
               uint32_t width, height;
               bool vsync;
           };

        void Init(const WindowContext& arg_WindowContext);

        GLFWwindow* m_window;

        WinWindowData m_data;
    };
}