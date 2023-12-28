#include "Application.h"

#include "WinWindow.h"

namespace sb
{
    int32_t Application::m_runningWindowCount = 0;
    Application* Application::s_instance = nullptr;

    Application::Application()
    {
        // s_instance에 assert 필요!
        s_instance = this;

        // 프로그램을 실행하면 기본으로 레이어(window)를 하나 만든다.(MainLayer)
        CreateLayer();
    }
    
    void Application::CreateLayer()
    {
        m_window = CreateUPtr<WinsWindow>(WindowContext("Custom_Window"), this);
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