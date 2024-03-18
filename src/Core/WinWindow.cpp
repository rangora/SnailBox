#define GLFW_INCLUDE_NONE
#include "WinWindow.h"

#include <iostream> // TEMP:LOG추가하면 지울 것
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include "Render/OpenGL/OpenGLContext.h"
#include "Platform/Input.h"
#include "Application.h"

void framebuffer_size_callback(GLFWwindow* InWindow, int InWidth, int InHeight)
{
    glViewport(0, 0, InWidth, InHeight);
}

namespace sb
{
    WinsWindow::WinsWindow(const WindowContext& arg_WindowContext)
    {
        Init(arg_WindowContext);
    }

    WinsWindow::WinsWindow(const WindowContext& arg_WindowContext, Application* in_app)
    {
        Init(arg_WindowContext);
        m_app = in_app;
    }

    WinsWindow::~WinsWindow()
    {
        ShutDown();
    }

    void WinsWindow::Update()
    {
        if (!glfwWindowShouldClose(m_window))
        {
            glfwPollEvents();

            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();
            ProcessInput();
            m_graphicContext->Render();
            ImGui::Render();

            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

            ImGuiIO& io = ImGui::GetIO();
            if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
            {
                ImGui::UpdatePlatformWindows();
                ImGui::RenderPlatformWindowsDefault();
                glfwMakeContextCurrent(glfwGetCurrentContext());
            }

            m_graphicContext->SwapBuffers();
        }
        else
        {
            // TEMP
            m_app->DestroyLayer();
        }
    }

    void WinsWindow::OnWindowSizeChanged(int32 in_width, int32 in_height)
    {
        m_windowData.width = in_width;
        m_windowData.height = in_height;
        glViewport(0.f, 0.f, m_windowData.width, m_windowData.height);
    }

    void WinsWindow::ProcessInput()
    {
        if(!m_cameraTranslation)
        {
            return;
        }

        const float cameraSpeed = 0.05f;
        if (glfwGetKey(m_window, GLFW_KEY_W) == GLFW_PRESS)
            m_cameraPos += cameraSpeed * m_cameraFront;
        if (glfwGetKey(m_window, GLFW_KEY_S) == GLFW_PRESS)
            m_cameraPos -= cameraSpeed * m_cameraFront;

        auto cameraRight = glm::normalize(glm::cross(m_cameraUp, -m_cameraFront));
        if (glfwGetKey(m_window, GLFW_KEY_D) == GLFW_PRESS)
            m_cameraPos += cameraSpeed * cameraRight;
        if (glfwGetKey(m_window, GLFW_KEY_A) == GLFW_PRESS)
            m_cameraPos -= cameraSpeed * cameraRight;

        auto cameraUp = glm::normalize(glm::cross(-m_cameraFront, cameraRight));
        if (glfwGetKey(m_window, GLFW_KEY_E) == GLFW_PRESS)
            m_cameraPos += cameraSpeed * cameraUp;
        if (glfwGetKey(m_window, GLFW_KEY_Q) == GLFW_PRESS)
            m_cameraPos -= cameraSpeed * cameraUp;
    }

    void WinsWindow::MouseMove(double in_x, double in_y)
    {
        if (!m_cameraTranslation)
        {
            return;
        }

        auto pos = glm::vec2((float)in_x, (float)in_y);
        auto deltaPos = pos - m_preMousePos;

        const float cameraRotSpeed = 0.8f;
        m_cameraYaw -= deltaPos.x * cameraRotSpeed;
        m_cameraPitch -= deltaPos.y * cameraRotSpeed;

        if (m_cameraYaw < 0.0f)
            m_cameraYaw += 360.0f;
        if (m_cameraYaw > 360.0f)
            m_cameraYaw -= 360.0f;

        if (m_cameraPitch > 89.0f)
            m_cameraPitch = 89.0f;
        if (m_cameraPitch < -89.0f)
            m_cameraPitch = -89.0f;

        m_preMousePos = pos;
    }

    void WinsWindow::MouseButton(int32 in_button, int32 in_action, double in_x, double in_y)
    {
        if (in_button == GLFW_MOUSE_BUTTON_RIGHT)
        {
            if (in_action == GLFW_PRESS)
            {
                m_cameraTranslation = true;
                m_preMousePos = glm::vec2((float)in_x, (float)in_y);
            }
            else if (in_action == GLFW_RELEASE)
            {
                m_cameraTranslation = false;
            }
        }
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

        m_window = glfwCreateWindow(arg_WindowContext.width, arg_WindowContext.height, arg_WindowContext.title.c_str(),
                                    nullptr, nullptr);

        m_graphicContext = GraphicsContext::Create(m_window, this);
        m_graphicContext->Initialize();

        glfwSetFramebufferSizeCallback(m_window, framebuffer_size_callback);

        m_imguiContext = ImGui::CreateContext();
        ImGui::SetCurrentContext(m_imguiContext);

        // config초기화는 Impl초기화 이전에 해줘야 한다.
        ImGuiIO& io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

        ImGui_ImplGlfw_InitForOpenGL(m_window, false);
        ImGui_ImplOpenGL3_Init();
        ImGui_ImplOpenGL3_CreateFontsTexture();
        ImGui_ImplOpenGL3_CreateDeviceObjects();

        glfwSetWindowUserPointer(m_window, this);

        glfwSetFramebufferSizeCallback(m_window, OnFreamBufferSizeChanged);

        // input callback binding은 init()에서 해준다.
        // Key input
        glfwSetKeyCallback(m_window, OnKeyEvent);

        // 문자가 입력됬을 때 호출되는 callback.
        glfwSetCharCallback(m_window, OnCharEvent);

        // Mouse input
        glfwSetCursorPosCallback(m_window, OnCursorPos);
        glfwSetMouseButtonCallback(m_window, OnMouseButton);
        glfwSetScrollCallback(m_window, OnScroll);
    }

    void WinsWindow::ShutDown()
    {
        ImGui_ImplOpenGL3_DestroyFontsTexture();
        ImGui_ImplOpenGL3_DestroyDeviceObjects();
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext(m_imguiContext);

        glfwTerminate();
    }
} // namespace sb
