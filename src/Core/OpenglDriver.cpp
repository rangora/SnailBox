#define GLFW_INCLUDE_NONE

#include "OpenglDriver.h"
#include "Render/OpenGL/OpenGLContext.h"
#include "Window.h"
#include "spdlog/spdlog.h"
#include <GLFW/glfw3.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

namespace sb
{
    OpenglDriver::OpenglDriver(Window* in_window)
    : Driver(in_window)
    {
    }

    void* OpenglDriver::GetNativeWindow()
    {
        return m_glfwWindow;
    }

    bool OpenglDriver::InitDriver(const WindowData* in_windowData)
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

    void OpenglDriver::Update()
    {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        Render();
        ImGui::Render();

        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        ImGuiIO& io = ImGui::GetIO();
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            // in-statement 안됨! 꼭 뺴서 넣어야 됨
            // 왜 인지는 모름
            GLFWwindow* CurrentContext = glfwGetCurrentContext();
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
            glfwMakeContextCurrent(CurrentContext);
        }
    }

    void OpenglDriver::OnUpdate(float in_delta)
    {
        m_camera.OnUpdate(in_delta);
    }

    void OpenglDriver::Render()
    {
        m_graphicContext->Render();
    }

    void OpenglDriver::SwapBuffers()
    {
        m_graphicContext->SwapBuffers();
    }

    bool OpenglDriver::IsWindowShouldClosed()
    {
        return glfwWindowShouldClose(m_glfwWindow);
    }
} // namespace sb