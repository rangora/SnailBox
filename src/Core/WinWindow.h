#pragma once

#include "Window.h"

#include "corepch.h"
#include <GLFW/glfw3.h>
#include <cstdint>
#include <glm/glm.hpp>
#include <string>

LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

struct GLFWwindow;
struct ImGuiContext;

namespace sb
{
    class GraphicsContext;

    class WinsWindow : public Window
    {
    public:
        WinsWindow() = delete;
        WinsWindow(const WindowContext& in_windowContext);
        ~WinsWindow();

        void Update() override;
        void ImGuiUpdate() override;

        bool InitializeWindows(const std::string& in_menuName, const std::string& in_className);
        bool InitializeDriver() final;
        bool BindGraphicsDriver() final;

        // input
        virtual void ProcessWinInput();
        // ~input

        void AttachLayout(Layout* in_layout) final;

        const WindowData& GetWindowData() { return m_windowData; }
        int GetClientWidth() const { return _clientWidth; }
        int GetClientHeight() const { return _clientHeight; }

        // camera 애매.. 뺴야할거 같음
        bool m_cameraTranslation = false;
        float m_cameraPitch = 0.f;
        float m_cameraYaw = 0.f;
        glm::vec2 m_preMousePos = glm::vec2(0.f);
        glm::vec3 m_cameraFront = glm::vec3(0.f, -1.f, 0.f);
        glm::vec3 m_cameraPos = glm::vec3(0.f, 0.f, 3.f);
        glm::vec3 m_cameraUp = glm::vec3(0.f, 1.f, 0.f);
        // ~camera

        // Windows
        WNDCLASSEXW m_wc;
        HWND m_hwnd;
        RECT _rcClient;
        int _clientWidth = 0;
        int _clientHeight = 0;

        // Render properties
        float m_f = 0.f;
        int32 m_counter = 0;
        Vector3f m_clearColor = {0.45f, 0.55f, 0.60f};

    private:
        void WindowUpdate();

        void SetupImGuiContext();
        void ReadyWindowShutdown() final;

        GLFWwindow* m_nativeWindow = nullptr;
    };
}