#include "GraphicsContext.h"
#include "Core/Application.h"
#include "Core/Driver.h"
#include "OpenGL/OpenGLContext.h"

namespace sb
{
    GraphicsDevice GraphicsContext::s_currentDevice = GraphicsDevice::None;

    UPtr<GraphicsContext> GraphicsContext::Create(void * in_userPointer, Driver* in_drvier)
    {
        switch (Application::s_currentGraphicsDevice)
        {
            case GraphicsDevice::OpenGL:
                return CreateUPtr<OpenglContext>(static_cast<GLFWwindow*>(in_userPointer), in_drvier);
                break;
            case GraphicsDevice::None:
            default:
                assert(false);
                break;
        }

        return nullptr;
    }
} // namespace sb
