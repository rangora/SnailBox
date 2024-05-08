#pragma once

#include "Common.h"

namespace sb
{
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
    };
};