#define GLFW_INCLUDE_NONE
#include "GlfwWindow.h"
#include "Input.h"
#include "OpenglDriver.h"
#include "Render/OpenGL/OpenGLContext.h"
#include "spdlog/spdlog.h"
#include <GLFW/glfw3.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

namespace sb
{
    GlfwWindow::GlfwWindow(const WindowContext& in_windowContext)
    : Window(in_windowContext)
    {
        m_windowData.height = in_windowContext.height;
        m_windowData.width = in_windowContext.width;
        m_windowData.title = in_windowContext.title;
    }

    GlfwWindow::~GlfwWindow()
    {
    }

    void GlfwWindow::Update()
    {
        if (!IsShutdownReserved() && !m_driver->IsWindowShouldClosed())
        {
            glfwPollEvents();
            ProcessGlfwInput();

            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();
            ImGuiUpdate();
            ImGui::Render();

            m_driver->Update();
            m_driver->SwapBuffers();
        }
        else
        {
            ReadyWindowShutdown();
        }
    }

    void GlfwWindow::ImGuiUpdate()
    {
        Window::ImGuiUpdate();
    }

    void GlfwWindow::ProcessGlfwInput()
    {
        if (!m_cameraTranslation)
        {
            return;
        }

        const float cameraSpeed = 0.05f;
        if (glfwGetKey(m_glfwWindow, GLFW_KEY_W) == GLFW_PRESS)
            m_cameraPos += cameraSpeed * m_cameraFront;
        if (glfwGetKey(m_glfwWindow, GLFW_KEY_S) == GLFW_PRESS)
            m_cameraPos -= cameraSpeed * m_cameraFront;

        auto cameraRight = glm::normalize(glm::cross(m_cameraUp, -m_cameraFront));
        if (glfwGetKey(m_glfwWindow, GLFW_KEY_D) == GLFW_PRESS)
            m_cameraPos += cameraSpeed * cameraRight;
        if (glfwGetKey(m_glfwWindow, GLFW_KEY_A) == GLFW_PRESS)
            m_cameraPos -= cameraSpeed * cameraRight;

        auto cameraUp = glm::normalize(glm::cross(-m_cameraFront, cameraRight));
        if (glfwGetKey(m_glfwWindow, GLFW_KEY_E) == GLFW_PRESS)
            m_cameraPos += cameraSpeed * cameraUp;
        if (glfwGetKey(m_glfwWindow, GLFW_KEY_Q) == GLFW_PRESS)
            m_cameraPos -= cameraSpeed * cameraUp;
    }

    bool GlfwWindow::BindGraphicsDriver()
    {
        m_driver = new OpenglDriver(this);
        if (!m_driver->InitDriver(&m_windowData))
        {
            spdlog::error("Failed init openglDriver.");
            return false;
        }

        AttachLayout(m_driver);

        m_glfwWindow = static_cast<GLFWwindow*>(m_driver->GetNativeWindow());
        if (m_glfwWindow == nullptr)
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
        ImGui_ImplGlfw_InitForOpenGL(m_glfwWindow, false);
        ImGui_ImplOpenGL3_Init(glsl_version);
        ImGui_ImplOpenGL3_CreateFontsTexture();
        ImGui_ImplOpenGL3_CreateDeviceObjects();

        glfwSetWindowUserPointer(m_glfwWindow, this);

        // Register callbacks
        glfwSetFramebufferSizeCallback(m_glfwWindow, OnFreamBufferSizeChanged);

        // input callback binding은 init()에서 해준다.
        // Key input
        glfwSetKeyCallback(m_glfwWindow, OnKeyEvent);

        // 문자가 입력됬을 때 호출되는 callback.
        glfwSetCharCallback(m_glfwWindow, OnCharEvent);

        // Mouse input
        glfwSetCursorPosCallback(m_glfwWindow, OnCursorPos);
        glfwSetMouseButtonCallback(m_glfwWindow, OnMouseButton);
        glfwSetScrollCallback(m_glfwWindow, OnScroll);

        m_isOpenglWindow = true;

        return true;
    }

    void GlfwWindow::ReadyWindowShutdown()
    {
         ImGui_ImplOpenGL3_DestroyFontsTexture();
         ImGui_ImplOpenGL3_DestroyDeviceObjects();
         ImGui_ImplOpenGL3_Shutdown();
         ImGui_ImplGlfw_Shutdown();
         ImGui::DestroyContext(m_imguiContext);
         glfwTerminate();

         Window::ReadyWindowShutdown();
    }

    void GlfwWindow::OnWindowSizeChanged(int32 in_width, int32 in_height)
    {
        Window::OnWindowSizeChanged(in_width, in_height);

        glViewport(0.f, 0.f, m_windowData.width, m_windowData.height);
    }

    void GlfwWindow::OnKeyEvent(GLFWwindow* in_window, int in_key, int in_scancode, int in_action, int in_modifier)
    {
        ImGui_ImplGlfw_KeyCallback(in_window, in_key, in_scancode, in_action, in_modifier);

        // 원래 입력은 Queue에 모아서 처리해야 한다.
        // 지금은 여기서 바로 처리하도록 함.
        switch (in_action)
        {
            case GLFW_PRESS:
            {
                if (in_key == GLFW_KEY_ESCAPE)
                {
                    glfwSetWindowShouldClose(in_window, true);
                    GlfwWindow* glfwWindow = (GlfwWindow*)glfwGetWindowUserPointer(in_window);
                    glfwWindow->SetWindowShutdownFlag();
                }
            }
        }
    }

    void GlfwWindow::OnCharEvent(GLFWwindow* in_window, uint32 in_ch)
    {
        ImGui_ImplGlfw_CharCallback(in_window, in_ch);
    }

    void GlfwWindow::OnCursorPos(GLFWwindow* in_window, double in_x, double in_y)
    {
        GlfwWindow* glfwWindow = (GlfwWindow*)glfwGetWindowUserPointer(in_window);
        glfwWindow->MouseMove(in_x, in_y);
    }

    void GlfwWindow::OnMouseButton(GLFWwindow* in_window, int32 in_button, int32 in_action, int32 in_modifier)
    {
        ImGui_ImplGlfw_MouseButtonCallback(in_window, in_button, in_action, in_modifier);
        GlfwWindow* glfwWindow = (GlfwWindow*)glfwGetWindowUserPointer(in_window);
        double x, y;
        glfwGetCursorPos(in_window, &x, &y);
        glfwWindow->MouseButtonAction(in_button, in_action, x, y);
    }

    void GlfwWindow::OnScroll(GLFWwindow* in_window, double in_x_offset, double in_y_offset)
    {
        ImGui_ImplGlfw_ScrollCallback(in_window, in_x_offset, in_y_offset);
    }

    void GlfwWindow::OnFreamBufferSizeChanged(GLFWwindow* in_window, int32 in_width, int32 in_height)
    {
        GlfwWindow* glfwWindow = (GlfwWindow*)glfwGetWindowUserPointer(in_window);
        glfwWindow->OnWindowSizeChanged(in_width, in_height);
    }

    void GlfwWindow::MouseMove(double in_x, double in_y)
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

    void GlfwWindow::MouseButtonAction(int32 in_button, int32 in_action, double in_x, double in_y)
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
}; // namespace sb