#include "GraphicsContext.h"
#include "Core/Application.h"
#include "Core/Driver.h"
#include "OpenGL/OpenGLContext.h"
#include "DirectX12/Direct3dContext.h"

namespace sb
{
    UPtr<GraphicsContext> GraphicsContext::Create(void * in_userPointer, Driver* in_drvier)
    {
        switch (Application::s_currentGraphicsDevice)
        {
            case GraphicsDevice::OpenGL:
                return CreateUPtr<OpenglContext>(static_cast<GLFWwindow*>(in_userPointer), in_drvier);
                break;
            case GraphicsDevice::DirectX12:
                return CreateUPtr<Direct3dContext>(in_drvier);
                break;
            case GraphicsDevice::None:
            default:
                assert(false);
                break;
        }

        return nullptr;
    }
} // namespace sb
