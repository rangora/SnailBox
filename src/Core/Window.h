#pragma once

#include "Actor/Actor.h"
#include "Driver.h"
#include "corepch.h"
#include <sstream>
#include <vector>

struct GLFWwindow;

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

    struct WinWindowData
    {
        std::string title;
        uint32_t width, height;
        bool vsync;
    };

    class Window
    {
    public:
        virtual ~Window() = default;
        virtual void Update() = 0;
        virtual void OnWindowShutDown() = 0;

        virtual bool InitializeWithOpenglDevice() = 0;
        virtual bool InitializeWithDirectXDevice() = 0;
        virtual bool InitializeDriver() = 0;

        virtual void GetMousePos(double& out_x, double& out_y) = 0;

        // Input 처리는 외부로 뺴야함..
        virtual void ProcessGlfwInput() {};
        virtual void ProcessWinInput() {};
        virtual void MouseMove(double in_x, double in_y) = 0;
        virtual void MouseButtonAction(int32 in_button, int32 in_action, double in_x, double in_y) = 0;

        virtual void AttachLayout(Layout* in_layout) {}
        
        virtual ComPtr<ID3D12Device> GetD3dDevice() { return nullptr; }

        bool IsOpenglWindow() const { return m_isOpenglWindow; }

        static void OnKeyEvent(GLFWwindow* in_window, int in_key, int in_scancode, int in_action, int in_modifier);
        static void OnCharEvent(GLFWwindow* in_window, uint32 in_ch);
        static void OnCursorPos(GLFWwindow* in_window, double in_x, double in_y);
        static void OnMouseButton(GLFWwindow* in_window, int32 in_button, int32 in_action, int32 in_modifier);
        static void OnScroll(GLFWwindow* in_window, double in_x_offset, double in_y_offset);
        // ~Input

        virtual void OnWindowSizeChanged(int32 in_width, int32 in_height) = 0;
        static void OnFreamBufferSizeChanged(GLFWwindow* in_window, int32 in_width, int32 in_height);

        std::vector<Layout*>& GetLayoutRef() { return m_layouts; }
        bool IswindowShutDown() const { return m_isWindowShutdown; }

        UPtr<GraphicsContext> m_graphicContext = nullptr;
        Driver* m_driver = nullptr;

    protected:
        bool m_isOpenglWindow = false; // TEMP
        bool m_isWindowShutdown = false;

        std::vector<Layout*> m_layouts;
    };
} // namespace sb
