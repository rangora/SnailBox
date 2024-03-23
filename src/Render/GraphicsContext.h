#pragma once

#include "Core/Common.h"

namespace sb
{
    class Window;

    class GraphicsContext
    {
    public:
        virtual ~GraphicsContext() = default;

        virtual void Initialize() = 0;
        virtual void SwapBuffers() = 0;
        virtual void Render() = 0; // ??

        static UPtr<GraphicsContext> Create(void* in_glWindow, Window* in_window);
    };
} // namespace sb