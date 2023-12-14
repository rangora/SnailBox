#include "Application.h"

#include "WinWindow.h"

namespace sb
{
    Application* Application::s_instance = nullptr;

    Application::Application()
    {
        // s_instance에 assert 필요!
        s_instance = this;
        m_window = CreateUPtr<WinsWindow>(WindowContext("Custom_Window"));
    }
    void Application::CreateLayer()
    {
        m_window;
    }
} // namespace sb