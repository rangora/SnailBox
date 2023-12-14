#define GLFW_INCLUDE_NONE
#include "WinWindow.h"

#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include "Render/OpenGL/OpenGLContext.h"

// TEmp input callback
void OnKeyEvent(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    ImGui_ImplGlfw_KeyCallback(window, key, scancode, action, mods);

    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, true);
    }
}

void OnCursorPos(GLFWwindow* window, double x, double y)
{
    // auto context = (Context*)glfwGetWindowUserPointer(window);
    // context->MouseMove(x, y);
}

void OnMouseButton(GLFWwindow* window, int button, int action, int modifier)
{
    ImGui_ImplGlfw_MouseButtonCallback(window, button, action, modifier);

    // auto context = (Context*)glfwGetWindowUserPointer(window);
    double x, y;
    glfwGetCursorPos(window, &x, &y);
    // context->MouseButton(button, action, x, y);
}

void OnCharEvent(GLFWwindow* window, unsigned int ch)
{
    ImGui_ImplGlfw_CharCallback(window, ch);
}

void OnScroll(GLFWwindow* window, double xoffset, double yoffset)
{
    ImGui_ImplGlfw_ScrollCallback(window, xoffset, yoffset);
}

void framebuffer_size_callback(GLFWwindow* InWindow, int InWidth, int InHeight)
{
    glViewport(0, 0, InWidth, InHeight);
}

// END

namespace sb
{
    WinsWindow::WinsWindow(const WindowContext& arg_WindowContext)
    {
        Init(arg_WindowContext);
    }

    WinsWindow::~WinsWindow()
    {
    }

    void WinsWindow::Init(const WindowContext& arg_WindowContext)
    {
        // glfw init
        int success = glfwInit();
        if (!success)
        {
            return;
        }

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

        GLFWwindow* glfw_window = glfwCreateWindow(arg_WindowContext.width, arg_WindowContext.height,
                                                   arg_WindowContext.title.c_str(), nullptr, nullptr);

        UPtr<GraphicsContext> render_context = GraphicsContext::Create(glfw_window);
        render_context->Init();

        glfwSetFramebufferSizeCallback(glfw_window, framebuffer_size_callback);

        ImGuiContext* imgui_Context = ImGui::CreateContext();
        ImGui::SetCurrentContext(imgui_Context);
        ImGui_ImplGlfw_InitForOpenGL(glfw_window, false);
        ImGui_ImplOpenGL3_Init();
        ImGui_ImplOpenGL3_CreateFontsTexture();
        ImGui_ImplOpenGL3_CreateDeviceObjects();

        // input
        glfwSetKeyCallback(glfw_window, OnKeyEvent);
        glfwSetCursorPosCallback(glfw_window, OnCursorPos);
        glfwSetMouseButtonCallback(glfw_window, OnMouseButton);
        glfwSetCharCallback(glfw_window, OnCharEvent); // 문자가 입력됬을 때 호출되는 callback.
        glfwSetScrollCallback(glfw_window, OnScroll);

        while (!glfwWindowShouldClose(glfw_window))
        {
            glfwPollEvents();

            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();
            render_context->Render();
            ImGui::Render();

            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
            glfwSwapBuffers(glfw_window);
        }

        ImGui_ImplOpenGL3_DestroyFontsTexture();
        ImGui_ImplOpenGL3_DestroyDeviceObjects();
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext(imgui_Context);

        glfwTerminate();
    }
} // namespace sb
