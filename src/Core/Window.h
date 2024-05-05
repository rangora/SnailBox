#pragma once

#include "Actor/Actor.h"
#include "Canvas.h"
#include "Common.h"
#include <sstream>
#include <vector>

struct GLFWwindow;

namespace sb
{
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
        virtual void ShutDown() = 0;

        virtual bool InitializeWithOpenglDevice() = 0;
        virtual bool InitializeCanvas() = 0;

        virtual void OnWindowSizeChanged(int32 in_width, int32 in_height) = 0;

        // Input 처리는 외부로 뺴야함..
        virtual void ProcessInput() = 0;
        virtual void MouseMove(double in_x, double in_y) = 0;
        virtual void MouseButton(int32 in_button, int32 in_action, double in_x, double in_y) = 0;

        static void OnKeyEvent(GLFWwindow* in_window, int in_key, int in_scancode, int in_action, int in_modifier);
        static void OnCharEvent(GLFWwindow* in_window, uint32 in_ch);
        static void OnCursorPos(GLFWwindow* in_window, double in_x, double in_y);
        static void OnMouseButton(GLFWwindow* in_window, int32 in_button, int32 in_action, int32 in_modifier);
        static void OnScroll(GLFWwindow* in_window, double in_x_offset, double in_y_offset);
        // ~Input

        static void OnFreamBufferSizeChanged(GLFWwindow* in_window, int32 in_width, int32 in_height);

        UPtr<GraphicsContext> m_graphicContext = nullptr;
        UPtr<Canvas> m_canvas = nullptr;
    };
} // namespace sb
