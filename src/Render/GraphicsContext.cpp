#include "GraphicsContext.h"
#include "Core/Canvas.h"
#include "OpenGL/OpenGLContext.h"

namespace sb
{
    UPtr<GraphicsContext> GraphicsContext::Create(void* in_glWindow, Canvas* in_canvas)
    {
        // Opengl
        return CreateUPtr<OpenglContext>(static_cast<GLFWwindow*>(in_glWindow), in_canvas);
    }
} // namespace sb
