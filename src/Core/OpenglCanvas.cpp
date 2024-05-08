#define GLFW_INCLUDE_NONE

#include "OpenglCanvas.h"
#include "Render/OpenGL/OpenGLContext.h"
#include "Window.h"
#include "spdlog/spdlog.h"
#include <GLFW/glfw3.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

namespace sb
{
    OpenglCanvas::OpenglCanvas(Window* in_window)
    : Canvas(in_window)
    {
    }

    void* OpenglCanvas::GetNativeWindow()
    {
        return m_glfwWindow;
    }

    bool OpenglCanvas::InitCanvas(const WinWindowData* in_windowData)
    {
        if (m_graphicContext != nullptr)
        {
            spdlog::error("Failed init openglDevice, graphicContext already init.");
            return false;
        }

        // glfw init
        int success = glfwInit();
        if (!success)
        {
            spdlog::error("Failed to glfwInit.");
            return false;
        }

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

        m_glfwWindow =
            glfwCreateWindow(in_windowData->width, in_windowData->height, in_windowData->title.c_str(), nullptr, nullptr);

        glfwSwapInterval(1.f);
        m_graphicContext = GraphicsContext::Create(m_glfwWindow, this);
        m_graphicContext->m_window_handle = m_window;
        m_graphicContext->Initialize();

        return true;
    }

    void OpenglCanvas::Update(float in_delta)
    {
    }

    void OpenglCanvas::OnUpdate(float in_delta)
    {
        m_camera.OnUpdate(in_delta);
    }

    void OpenglCanvas::Render()
    {
        m_graphicContext->Render();
    }

    void OpenglCanvas::SwapBuffers()
    {
        m_graphicContext->SwapBuffers();
    }
} // namespace sb