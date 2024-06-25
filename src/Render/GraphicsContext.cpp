#include "GraphicsContext.h"
#include "Core/Driver.h"
#include "OpenGL/OpenGLContext.h"

namespace sb
{
    UPtr<GraphicsContext> GraphicsContext::Create(void* in_glWindow, Driver* in_drvier)
    {
        // Opengl
        return CreateUPtr<OpenglContext>(static_cast<GLFWwindow*>(in_glWindow), in_drvier);
    }
} // namespace sb
