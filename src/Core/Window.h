#pragma once

#include "Actor/Actor.h"
#include "Driver.h"
#include "corepch.h"
#include <sstream>
#include <vector>

struct GLFWwindow;
struct ImGuiContext;

namespace sb
{
    class Layout;
    class Actor;
    class GraphicsContext;

    struct WindowContext
    {
        std::string title;
        uint32_t width;
        uint32_t height;
        GraphicsDevice graphicsDevice;

        WindowContext(const std::string& arg_title = "New_Window",
                      uint32_t arg_width = 1000,
                      uint32_t arg_height = 600)
            : title(arg_title), width(arg_width), height(arg_height) {}
    };

    struct WindowData
    {
        std::string title;
        uint32_t width, height;
        bool vsync;
    };

    class Window
    {
    public:
        Window() = delete;
        Window(const WindowContext& in_windowContext) {}
        virtual ~Window() {}

        virtual void Update() = 0;
        virtual void OnWindowShutDown() {};

        virtual bool BindGraphicsDriver() = 0;
        virtual bool InitializeDriver() { return false; };

        // Input 처리는 외부로 뺴야함..
        virtual void ProcessGlfwInput() {};
        virtual void ProcessWinInput() {};
        // ~Input

        virtual void AttachLayout(Layout* in_layout) {}

        bool IsOpenglWindow() const { return m_isOpenglWindow; }

        virtual void OnWindowSizeChanged(int32 in_width, int32 in_height) {};

        std::vector<Layout*>& GetLayoutRef() { return m_layouts; }
        bool IswindowShutDown() const { return m_isWindowShutdown; }

        UPtr<GraphicsContext> m_graphicContext = nullptr;
        Driver* m_driver = nullptr;

        bool m_bForceShutDown = false;

    protected:
        bool m_isOpenglWindow = false; // TEMP
        bool m_isWindowShutdown = false;

        ImGuiContext* m_imguiContext = nullptr;

        WindowData m_windowData;
        std::vector<Layout*> m_layouts;
    };
} // namespace sb
