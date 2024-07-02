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

        bool InitializeFrontWindow();
        bool BindDirect3dDriver();

        void ImGuiUpdate() final;

        int32 m_driverType = 2;
    };
};