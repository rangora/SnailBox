#pragma once

#include "Canvas.h"

struct GLFWwindow;

namespace sb
{
    class Window;

    class OpenglCanvas : public Canvas
    {
    public:
        OpenglCanvas(Window* in_window);
        OpenglCanvas() = delete;

        void* GetNativeWindow() final;

        bool InitCanvas(const WinWindowData* in_windowData) final;
        void Update() final;
        void SwapBuffers() final;

    private:
        // 소유자는 Window? Cnavas?? 우선 canvas가 가지도록 했음..
        GLFWwindow* m_glfwWindow = nullptr;
    };
};