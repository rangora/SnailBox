#pragma once

#include <sstream>

#include "Common.h"

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
    };
} // namespace sb
