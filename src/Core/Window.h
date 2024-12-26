#pragma once

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
        GraphicsDevice graphicsDevice = GraphicsDevice::None;

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

        virtual void Update();
        virtual void ImGuiUpdate();
        virtual void PostUpdate();
        virtual void ReadyWindowShutdown() { m_isReadyWindowShutdown = true; }

        virtual bool BindGraphicsDriver() = 0;
        virtual bool InitializeDriver() { return false; };

        virtual void OnWindowSizeChanged(int32 in_width, int32 in_height);
        virtual void AttachLayout(Layout* in_layout) {}
        virtual bool IsShutdownReserved();

        bool IsOpenglWindow() const { return m_isOpenglWindow; }
        bool IsReadyWindowShutdown() const { return m_isReadyWindowShutdown; }
        std::vector<Layout*>& GetLayoutRef() { return m_layouts; }

        void UpdateMousePosition(int mouseX, int mouseY);

        UPtr<GraphicsContext> m_graphicContext = nullptr;
        Driver* m_driver = nullptr;

        bool m_bForceShutDown = false;
        Vector2f _mouseDiff = Vector2f::zeroVector;

    protected:
        bool m_isOpenglWindow = false; // TEMP
        bool m_isReadyWindowShutdown = false;
        bool m_isWindowShutDownKeyPressed = false;
        bool _bConsumeMouseInput = false;

        Vector2f _preMousePos = Vector2f::zeroVector;
        Vector2f _MousePos = Vector2f::zeroVector;

        ImGuiContext* m_imguiContext = nullptr;

        WindowData m_windowData;
        std::vector<Layout*> m_layouts;
    };
} // namespace sb
