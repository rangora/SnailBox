#include "Window.h"

namespace sb
{
    bool Window::IsShutdownReserved()
    {
        return m_isWindowShutDownKeyPressed || m_bForceShutDown;
    }

    void Window::OnWindowSizeChanged(int32 in_width, int32 in_height)
    {
        m_windowData.width = in_width;
        m_windowData.height = in_height;
    }
} // namespace sb