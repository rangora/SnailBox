﻿#pragma once

#include "Window.h"

#include <string>
#include <cstdint>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

struct GLFWwindow;
struct ImGuiContext;

namespace sb
{
    class GraphicsContext;

    struct WinWindowData
    {
        std::string title;
        uint32_t width, height;
        bool vsync;
    };

    class WinsWindow : public Window
    {
        public:
           WinsWindow(const WindowContext& in_windowContext);
           WinsWindow(const WindowContext& in_windowContext, class Application* in_app);
           virtual ~WinsWindow();

           void InitRenderer() final;
           void Update() final;

           void OnWindowSizeChanged(int32 in_width, int32 in_height) final;

           void ProcessInput() final;
           void MouseMove(double in_x, double in_y) final;
           void MouseButton(int32 in_button, int32 in_action, double in_x, double in_y) final;

           const WinWindowData& GetWindowData() { return m_windowData; }

           // camera 애매.. 뺴야할거 같음
           bool m_cameraTranslation = false;
           float m_cameraPitch = 0.f;
           float m_cameraYaw = 0.f;
           glm::vec2 m_preMousePos = glm::vec2(0.f);
           glm::vec3 m_cameraFront = glm::vec3(0.f, -1.f, 0.f);
           glm::vec3 m_cameraPos = glm::vec3(0.f, 0.f, 3.f);
           glm::vec3 m_cameraUp = glm::vec3(0.f, 1.f, 0.f);
           // ~camera

       private:
           void ShutDown() final;

           Application* m_app = nullptr; // TEMP
           GLFWwindow* m_window = nullptr;
           ImGuiContext* m_imguiContext = nullptr;

           WinWindowData m_windowData;
    };
}