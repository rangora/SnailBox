#pragma once

#include "src/Core/Common.h"

namespace sb
{
    class GraphicsContext
    {
    public:
        virtual ~GraphicsContext() = default;

        virtual void Initialize() = 0;
        virtual void SwapBuffers() = 0;
        virtual void Render() = 0; // ??

        static UPtr<GraphicsContext> Create(void* arg_window);
    };
} // namespace sb