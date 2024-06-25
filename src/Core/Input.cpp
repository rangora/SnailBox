#include "Input.h"
#include "Application.h"
#include "Core/Driver.h"
#include "Core/Window.h"
#include <GLFW/glfw3.h>

namespace sb
{
    std::pair<double, double> Input::GetMousePosition()
    {
        Window& window = Application::Get().GetFocusWindow();

        if (window.IsOpenglWindow())
        {
            auto driver = window.m_driver.get();
            GLFWwindow* glfwWindow = static_cast<GLFWwindow*>(driver->GetNativeWindow());
            double x = 0.0, y = 0.0;
            glfwGetCursorPos(glfwWindow, &x, &y);
            return {(double)x, (double)y};
        }

        return std::pair<double, double>(0.0, 0.0);
    }

    double Input::GetMouseX()
    {
        auto [x, y] = GetMousePosition();
        return x;
    }

    double Input::GetMouseY()
    {
        auto [x, y] = GetMousePosition();
        return y;
    }

    bool Input::IsMouseButtonDown(MouseButton in_button)
    {
        Window& window = Application::Get().GetFocusWindow();

        if (window.IsOpenglWindow())
        {
            auto driver = window.m_driver.get();
            GLFWwindow* glfwWindow = static_cast<GLFWwindow*>(driver->GetNativeWindow());
            int state = glfwGetMouseButton(glfwWindow, static_cast<int32>(in_button));
            return state == GLFW_PRESS;
        }

        return false;
    }

    bool Input::IsKeyButtonDown(KeyButton in_key)
    {
        if (s_keyData[in_key].m_key == in_key)
        {
            return s_keyData[in_key].m_state == KeyState::Pressed;
        }

        return false;
    }

    bool Input::IsKeyButtonHeld(KeyButton in_key)
    {
        if (s_keyData[in_key].m_key == in_key)
        {
            return s_keyData[in_key].m_state == KeyState::Held;
        }

        return false;
    }

    bool Input::IsKeyButtonReleased(KeyButton in_key)
    {
        if (s_keyData[in_key].m_key == in_key)
        {
            return s_keyData[in_key].m_state == KeyState::Released;
        }

        return false;
    }

    void Input::TransitionPressedButtons()
    {
        for (auto& [button, buttonData] : s_mouseData)
        {
            if (buttonData.m_state == KeyState::Pressed)
            {
                if (buttonData.m_bDone)
                {
                    UpdateButtonState(button, KeyState::Held);
                }
            }
        }

        for (auto& [key, keyData] : s_keyData)
        {
            if (keyData.m_state == KeyState::Pressed)
            {
                if(keyData.m_bDone)
                {
                    UpdateKeyState(key, KeyState::Held);
                }
            }
        }
    }

    void Input::UpdateButtonState(MouseButton in_button, KeyState in_state)
    {
        auto& mouseData = s_mouseData[in_button];
        mouseData.m_button = in_button;
        mouseData.m_oldState = mouseData.m_state;
        mouseData.m_state = in_state;
        mouseData.m_bDone = false;
    }

    void Input::UpdateKeyState(KeyButton in_key, KeyState in_state)
    {
        auto& keyData = s_keyData[in_key];
        keyData.m_key = in_key;
        keyData.m_oldState = keyData.m_state;
        keyData.m_state = in_state;
        keyData.m_bDone = false;
    }

    void Input::ClearReleasedKeys()
    {
        for (auto& [button, buttonData] : s_mouseData)
        {
            if (buttonData.m_bDone && buttonData.m_state == KeyState::Released)
            {
                UpdateButtonState(button, KeyState::None);
            }
        }

        for (auto& [key, keyData] : s_keyData)
        {
            if (keyData.m_bDone && keyData.m_state == KeyState::Released)
            {
                UpdateKeyState(key, KeyState::None);
            }
        }
    }

    void Input::KeyInputAction(const std::function<void()>& in_fn, const KeyState in_state, const KeyButton in_button)
    {
        auto& keyData = s_keyData[in_button];
        if (keyData.m_state == in_state)
        {
            in_fn();
            keyData.m_bDone = true;
        }
    }
} // namespace sb