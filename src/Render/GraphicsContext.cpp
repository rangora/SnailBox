#include "GraphicsContext.h"

#include "OpenGL/OpenGLContext.h"

namespace sb
{
    UPtr<GraphicsContext> GraphicsContext::Create(void* in_glWindow, Window* in_window)
    {
        // Opengl
        return CreateUPtr<OpenglContext>(static_cast<GLFWwindow*>(in_glWindow), in_window);
    }
} // namespace sb
