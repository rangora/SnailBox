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

            glfwSwapBuffers(m_window);
        }
        else
        {
            // TEMP
            m_app->DestroyLayer();
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

        m_graphicContext = GraphicsContext::Create(m_window);
        m_graphicContext->Init();

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

        OpenGLContext* OpenGLContextPtr = static_cast<OpenGLContext*>(m_graphicContext.get());
        OpenGLContextPtr->TestInit();
        // shader bidning
        OpenGLContextPtr->AttachShader(Application::s_staticShaderArchive);

        // input callback binding은 init()에서 해준다.
        // Key input
        glfwSetKeyCallback(m_window,
                           [](GLFWwindow* in_window, int in_key, int in_scancode, int in_action, int in_modifier)
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
                                       }
                                   }
                               }
                           });

        // 문자가 입력됬을 때 호출되는 callback.
        glfwSetCharCallback(m_window, [](GLFWwindow* in_window, unsigned int in_ch)
                            { ImGui_ImplGlfw_CharCallback(in_window, in_ch); });

        // Mouse input
        glfwSetCursorPosCallback(m_window, [](GLFWwindow* in_window, double in_x, double y)
        {
            // 여기서 MoveMouse() 처리.
        });
        glfwSetMouseButtonCallback(m_window,
                                   [](GLFWwindow* in_window, int in_button, int in_action, int int_modifier)
                                   {
                                       // 마우스 입력을 처리할 때 대상 Window를 알아야 하기 떄문에
                                       // glfwGetWindowUserPointer로 포인터를 가져와서 처리해줘야 하지만
                                       // 지금은 여기서 모든걸 하도록 한다.
                                       switch (in_action)
                                       {
                                           case GLFW_PRESS:
                                           {
                                               ImGui_ImplGlfw_MouseButtonCallback(in_window, in_button, in_action,
                                                                                  int_modifier);

                                               // log 대신..
                                               std::cout << "GLFW_PRESS" << std::endl;
                                               break;
                                           }
                                           case GLFW_RELEASE:
                                           {
                                               ImGui_ImplGlfw_MouseButtonCallback(in_window, in_button, in_action,
                                                                                  int_modifier);
                                               std::cout << "GLFW_RELEASE" << std::endl;
                                               break;
                                           }
                                       }
                                   });
        glfwSetScrollCallback(m_window, [](GLFWwindow* in_window, double in_xoffset, double in_yoffset)
                              { ImGui_ImplGlfw_ScrollCallback(in_window, in_xoffset, in_yoffset); });
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
