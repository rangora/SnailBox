#pragma once

#include "Common.h"
#include "InputKey.h"
#include "platform.h"
#include <utility>
#include <map>

namespace sb
{
    class Input
    {
    public:
        static std::pair<double, double> GetMousePosition();
        static bool IsMouseButtonDown(MouseButton in_button);
        static double GetMouseX();
        static double GetMouseY();

        // update
        static void TransitionPressedButtons();
        static void UpdateButtonState(MouseButton in_button, KeyState in_state);
        static void ClearReleasedKeys();

    private:
        inline static std::map<MouseButton, ButtonData> s_mouseData;
    };
};