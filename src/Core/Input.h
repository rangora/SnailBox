#pragma once

#include "corepch.h"
#include "platform.h"
#include <utility>
#include <map>

namespace sb
{
    enum class KeyButton : uint32
    {
        None,
        Esc
    };

    enum class MouseButton : uint8
    {
        Left,
        Right,
        Middle,
    };

    enum class KeyState : uint8
    {
        None,
        Pressed,
        Held,
        Released,
    };

    struct ButtonData
    {
        MouseButton m_button;
        KeyState m_state = KeyState::None;
        KeyState m_oldState = KeyState::None;
        bool m_bDone = false;
    };

    struct KeyData
    {
        KeyButton m_key;
        KeyState m_state = KeyState::None;
        KeyState m_oldState = KeyState::None;
        bool m_bDone = false;
    };

    class Input
    {
    public:
        static std::pair<double, double> GetMousePosition();
        static bool IsMouseButtonDown(MouseButton in_button);
        static bool IsKeyButtonDown(KeyButton in_key);
        static bool IsKeyButtonHeld(KeyButton in_key);
        static bool IsKeyButtonReleased(KeyButton in_key);
        static double GetMouseX();
        static double GetMouseY();

        // update
        static void TransitionPressedButtons();
        static void UpdateButtonState(MouseButton in_button, KeyState in_state);
        static void UpdateKeyState(KeyButton in_key, KeyState in_state);
        static void ClearReleasedKeys();

        static void KeyInputAction(const std::function<void()>& in_fn, const KeyState in_state, const KeyButton in_button);

    private:
        inline static std::map<MouseButton, ButtonData> s_mouseData;
        inline static std::map<KeyButton, KeyData> s_keyData;
    };
};