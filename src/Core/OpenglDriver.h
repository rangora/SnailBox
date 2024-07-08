#pragma once

#include "Driver.h"
#include "Render/Camera.h"

struct GLFWwindow;

namespace sb
{
    class Window;

    class OpenglDriver : public Driver
    {
    public:
        OpenglDriver(Window* in_window);
        OpenglDriver() = delete;

        void* GetNativeWindow() final;

        bool InitDriver(const WindowData* in_windowData) final;
        bool BindglfwWindow() final { return false; };
        void Update() final;
        void OnUpdate(float in_delta) final;
        void Render() final;
        void SwapBuffers() final;
        bool IsWindowShouldClosed() final;

        Camera& GetCameraRef()
        {
            return m_camera;
        }

    private:
        // 소유자는 Window? driver?? 우선 driver 가지도록 했음..
        GLFWwindow* m_glfwWindow = nullptr;

        Camera m_camera;
    };
};