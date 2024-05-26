#define GLFW_INCLUDE_NONE
#include "WinWindow.h"
#include "Application.h"
#include "OpenglCanvas.h"
#include "DirectXCanvas.h"
#include "Render/OpenGL/OpenGLContext.h"
#include "spdlog/spdlog.h"
#include "Input.h"
#include "InputKey.h"
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <vector>

void framebuffer_size_callback(GLFWwindow* InWindow, int InWidth, int InHeight)
{
    glViewport(0, 0, InWidth, InHeight);
}

namespace sb
{
    WinsWindow::WinsWindow(const WindowContext& in_windowContext)
    {
        m_windowData.height = in_windowContext.height;
        m_windowData.width = in_windowContext.width;
        m_windowData.title = in_windowContext.title;
    }

    WinsWindow::WinsWindow(const WindowContext& in_windowContext, Application* in_app)
    {
        m_windowData.height = in_windowContext.height;
        m_windowData.width = in_windowContext.width;
        m_windowData.title = in_windowContext.title;
        m_app = in_app;
    }

    WinsWindow::~WinsWindow()
    {
        ShutDown();
    }

    void WinsWindow::Update()
    {
        if(!m_canvas->IsWindowShouldClosed())
        {
            ProcessInput();
            m_canvas->Update();
            m_canvas->SwapBuffers();
        }
        else
        {
            // TEMP
            m_app->DestroyAppWindow();
        }
    }

    bool WinsWindow::InitializeWithOpenglDevice()
    {
        m_canvas = CreateUPtr<OpenglCanvas>(this);
        if (!m_canvas->InitCanvas(&m_windowData))
        {
            spdlog::error("Failed init openglCanvas.");
            return false;
        }

        AttachLayout(m_canvas.get());

        m_nativeWindow = static_cast<GLFWwindow*>(m_canvas->GetNativeWindow());
        if (m_nativeWindow == nullptr)
        {
            return false;
        }

        m_imguiContext = ImGui::CreateContext();
        ImGui::SetCurrentContext(m_imguiContext);

        // config초기화는 Impl초기화 이전에 해줘야 한다.
        ImGuiIO& io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

        const char* glsl_version = "#version 460";
        ImGui_ImplGlfw_InitForOpenGL(m_nativeWindow, false);
        ImGui_ImplOpenGL3_Init(glsl_version);
        ImGui_ImplOpenGL3_CreateFontsTexture();
        ImGui_ImplOpenGL3_CreateDeviceObjects();

        glfwSetWindowUserPointer(m_nativeWindow, this);

        // Register callbacks
        glfwSetFramebufferSizeCallback(m_nativeWindow, OnFreamBufferSizeChanged);

        // input callback binding은 init()에서 해준다.
        // Key input
        glfwSetKeyCallback(m_nativeWindow, OnKeyEvent);

        // 문자가 입력됬을 때 호출되는 callback.
        glfwSetCharCallback(m_nativeWindow, OnCharEvent);

        // Mouse input
        glfwSetCursorPosCallback(m_nativeWindow, OnCursorPos);
        glfwSetMouseButtonCallback(m_nativeWindow, OnMouseButton);
        glfwSetScrollCallback(m_nativeWindow, OnScroll);

        m_isOpenglWindow = true;

        return true;
    }

    bool WinsWindow::InitializeWithDirectXDevice()
    {
        m_canvas = CreateUPtr<DirectXCanvas>(this);
        if (!m_canvas->InitCanvas(&m_windowData))
        {
            spdlog::error("Failed init DirectXCanvas.");
            return false;
        }

        AttachLayout(m_canvas.get());

        return true;
    }

    bool WinsWindow::InitializeCanvas()
    {
        return false;
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
        if (glfwGetKey(m_nativeWindow, GLFW_KEY_W) == GLFW_PRESS)
            m_cameraPos += cameraSpeed * m_cameraFront;
        if (glfwGetKey(m_nativeWindow, GLFW_KEY_S) == GLFW_PRESS)
            m_cameraPos -= cameraSpeed * m_cameraFront;

        auto cameraRight = glm::normalize(glm::cross(m_cameraUp, -m_cameraFront));
        if (glfwGetKey(m_nativeWindow, GLFW_KEY_D) == GLFW_PRESS)
            m_cameraPos += cameraSpeed * cameraRight;
        if (glfwGetKey(m_nativeWindow, GLFW_KEY_A) == GLFW_PRESS)
            m_cameraPos -= cameraSpeed * cameraRight;

        auto cameraUp = glm::normalize(glm::cross(-m_cameraFront, cameraRight));
        if (glfwGetKey(m_nativeWindow, GLFW_KEY_E) == GLFW_PRESS)
            m_cameraPos += cameraSpeed * cameraUp;
        if (glfwGetKey(m_nativeWindow, GLFW_KEY_Q) == GLFW_PRESS)
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

    void WinsWindow::MouseButtonAction(int32 in_button, int32 in_action, double in_x, double in_y)
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

        // new
        switch (in_action)
        {
            case GLFW_PRESS:
            {
                Input::UpdateButtonState((MouseButton)in_button, KeyState::Pressed);
                break;
            }
            case GLFW_RELEASE:
            {
                Input::UpdateButtonState((MouseButton)in_button, KeyState::Released);
                break;
            }
        }
    }

    void WinsWindow::GetMousePos(double& out_x, double& out_y)
    {
        if (IsOpenglWindow())
        {
            glfwGetCursorPos(m_nativeWindow, &out_x, &out_y);
        }
    }

    void WinsWindow::AttachLayout(Layout* in_layout)
    {
        m_layouts.emplace_back(in_layout);
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
