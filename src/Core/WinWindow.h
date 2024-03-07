#pragma once

#include "Window.h"

#include <string>
#include <cstdint>
#include <GLFW/glfw3.h>

struct ImGuiContext;

namespace sb
{
    class WinsWindow : public Window
    {
        public:
           WinsWindow(const WindowContext& arg_WindowContext);
           WinsWindow(const WindowContext& arg_WindowContext, class Application* in_app);
           virtual ~WinsWindow();
           void Update() final;

       private:
           struct WinWindowData
           {
               std::string title;
               uint32_t width, height;
               bool vsync;
           };

        void Init(const WindowContext& arg_WindowContext);
        void ShutDown() final;

        Application* m_app = nullptr; // TEMP

        class GLFWwindow* m_window = nullptr;
        ImGuiContext* m_imguiContext = nullptr;

        UPtr<class GraphicsContext> m_graphicContext = nullptr; // 필요 없을 듯

        WinWindowData m_data;
    };
}