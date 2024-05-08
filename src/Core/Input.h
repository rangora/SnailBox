#pragma once

#include "Common.h"
#include "InputKey.h"
#include <utility>

namespace sb
{
    class Input
    {
    public:
        static std::pair<double, double> GetMousePosition();
        static double GetMouseX();
        static double GetMouseY();
    };
};