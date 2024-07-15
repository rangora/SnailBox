#pragma once

#include "Window.h"
#include "corepch.h"

namespace sb
{
    class GlfwWindow : public Window
    {
    public:
        GlfwWindow() = delete;
        GlfwWindow(const WindowContext& in_windowContext);
        ~GlfwWindow();

        void Update() final;
        void ProcessGlfwInput() final; // virtual??
        bool BindGraphicsDriver() final; // virtual??

        void ReadyWindowShutdown() final;

        void SetWindowShutdownFlag() { m_isWindowShutDownKeyPressed = true; }

        // camera 애매.. 뺴야할거 같음
        bool m_cameraTranslation = false;
        float m_cameraPitch = 0.f;
        float m_cameraYaw = 0.f;
        glm::vec2 m_preMousePos = glm::vec2(0.f);
        glm::vec3 m_cameraFront = glm::vec3(0.f, -1.f, 0.f);
        glm::vec3 m_cameraPos = glm::vec3(0.f, 0.f, 3.f);
        glm::vec3 m_cameraUp = glm::vec3(0.f, 1.f, 0.f);
        // ~camera

        GLFWwindow* m_glfwWindow = nullptr;

    private:
        static void OnKeyEvent(GLFWwindow* in_window, int in_key, int in_scancode, int in_action, int in_modifier);
        static void OnCharEvent(GLFWwindow* in_window, uint32 in_ch);
        static void OnCursorPos(GLFWwindow* in_window, double in_x, double in_y);
        static void OnMouseButton(GLFWwindow* in_window, int32 in_button, int32 in_action, int32 in_modifier);
        static void OnScroll(GLFWwindow* in_window, double in_x_offset, double in_y_offset);
        static void OnFreamBufferSizeChanged(GLFWwindow* in_window, int32 in_width, int32 in_height);

        void MouseMove(double in_x, double in_y);
        void MouseButtonAction(int32 in_button, int32 in_action, double in_x, double in_y);
    };
}; // namespace sb