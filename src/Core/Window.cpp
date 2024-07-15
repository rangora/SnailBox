#include "Window.h"

namespace sb
{
    bool Window::IsShutdownReserved()
    {
        return m_isWindowShutDownKeyPressed || m_bForceShutDown;
    }
} // namespace sb