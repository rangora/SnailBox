#define GLFW_INCLUDE_NONE
#include "Application.h"

#include "Enums.h"
#include "Render/ShaderCompiler.h"
#include "WinWindow.h"
#include "spdlog/spdlog.h"

namespace sb
{
    int32_t Application::m_runningWindowCount = 0;
    Application* Application::s_instance = nullptr;
    ShaderArchive Application::s_staticShaderArchive(GraphicsLibrary::OpenGL);

    Application::Application()
    {
        // s_instance에 assert 필요!
        s_instance = this;

        // ShaderPreCompiler list
        s_staticShaderArchive.Add("../resources/shader/simple.vs");
        s_staticShaderArchive.Add("../resources/shader/simple.fs");

        // 1) 쉐이더 컴파일
        ShaderCompiler::Compile(s_staticShaderArchive);

        // 2) program에 attach(link)


        // 프로그램을 실행하면 기본으로 window를 하나 만든다.
        WindowContext openglWindowContext;
        openglWindowContext.title = openglWindowTitle;
        CreateAppWindow(openglWindowContext);
    }

    void Application::CreateAppWindow(const WindowContext& in_windowContext)
    {
        if (in_windowContext.title.empty())
        {
            spdlog::error("WindowContextTitle can't be empty for creating window.");
        }

        spdlog::info("Add new window:{}, height:{}, width:{}", in_windowContext.title, in_windowContext.height,
                     in_windowContext.width);
        auto newWindow = CreateUPtr<WinsWindow>(in_windowContext, this);
        m_windows.insert({in_windowContext.title, std::move(newWindow)});
        m_windows[in_windowContext.title]->InitRenderer();
        m_runningWindowCount++;
    }

    void Application::DestroyAppWindow()
    {
        m_windows.clear();
        m_runningWindowCount = 0;
    }

    void Application::Run()
    {
        while (m_runningWindowCount)
        {
            for (auto& [_, window] : m_windows)
            {
                window->Update();
            }
        }
    }
} // namespace sb