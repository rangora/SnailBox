#pragma once

#include "corepch.h"
#include "WinWindow.h"

namespace sb
{
    class FrontWindow : public WinsWindow
    {
    public:
        FrontWindow(const WindowContext& in_windowContext);
        FrontWindow(const WindowContext& in_windowContext, class Application* in_app);
        virtual ~FrontWindow() {};

        void ImGuiUpdate() final;
    };
};