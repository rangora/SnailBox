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


        // 프로그램을 실행하면 기본으로 레이어(window)를 하나 만든다.(MainLayer)
        CreateLayer();
    }

    void Application::CreateLayer()
    {
        spdlog::info("Add new layer");
        m_window = CreateUPtr<WinsWindow>(WindowContext("OpenGL Window"), this);
        m_windows.emplace_back(std::move(m_window));
        m_runningWindowCount++;
    }

    void Application::DestroyLayer()
    {
        m_windows.clear();
        m_runningWindowCount = 0;
    }

    void Application::Run()
    {
        while (m_runningWindowCount)
        {
            for (auto& window : m_windows)
            {
                window->Update();
            }
        }
    }
} // namespace sb