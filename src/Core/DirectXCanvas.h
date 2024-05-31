#pragma once

#include "Canvas.h"
#include "Render/Camera.h"
#include "imgui.h"
#include "Render/DirectX12/Device.h"

struct GLFWwindow;

namespace sb
{
    class Window;

    class DirectXCanvas : public Canvas
    {
    public:
        DirectXCanvas(Window* in_window);
        DirectXCanvas() = delete;

        void* GetNativeWindow() final;

        bool InitCanvas(const WinWindowData* in_windowData) final;
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
        bool CreateDevice(HWND in_hwnd);

        Camera m_camera;

        ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

        UPtr<Device> m_device;

    };
};