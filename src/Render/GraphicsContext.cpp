#include "GraphicsContext.h"

#include "OpenGL/OpenGLContext.h"

namespace sb
{
    UPtr<GraphicsContext> GraphicsContext::Create(void* arg_window)
    {
        // Opengl
        return CreateUPtr<OpenGLContext>(static_cast<GLFWwindow*>(arg_window));
    }
} // namespace sb
