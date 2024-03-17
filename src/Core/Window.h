#pragma once

#include <sstream>
#include "Common.h"

struct GLFWwindow;

namespace sb
{
    struct WindowContext
    {
        std::string title;
        uint32_t width;
        uint32_t height;

        WindowContext(const std::string& arg_title = "New_Window",
                      uint32_t arg_width = 1000,
                      uint32_t arg_height = 600)
            : title(arg_title), width(arg_width), height(arg_height) {}
    };

    class Window
    {
    public:
        virtual ~Window() = default;
        virtual void Update() = 0;
        virtual void ShutDown() = 0;
        // static UPtr<Window> Create(const WindowContext& arg_context = WindowContext());

        // Input 처리는 외부로 뺴야함..
        virtual void MouseMove(double in_x, double in_y) = 0;
        virtual void ProcessInput(GLFWwindow* in_window) = 0;
        virtual void MouseButton(int32 in_button, int32 in_action, double in_x, double in_y) = 0;
        
        static void OnKeyEvent(GLFWwindow* in_window, int in_key, int in_scancode, int in_action, int in_modifier);
        static void OnCharEvent(GLFWwindow* in_window, uint32 in_ch);
        static void OnCursorPos(GLFWwindow* in_window, double in_x, double in_y);
        static void OnMouseButton(GLFWwindow* in_window, int32 in_button, int32 in_action, int32 in_modifier);
        static void OnScroll(GLFWwindow* in_window, double in_x_offset, double in_y_offset);
        // ~Input
    };
} // namespace sb
