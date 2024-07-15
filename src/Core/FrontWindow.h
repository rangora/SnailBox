#pragma once

#include "corepch.h"
#include "WinWindow.h"

namespace sb
{
    class FrontWindow : public WinsWindow
    {
    public:
        FrontWindow() = delete;
        FrontWindow(const WindowContext& in_windowContext);
        ~FrontWindow() {};

        void ProcessWinInput() final {}

        bool InitializeFrontWindow();
        bool BindDirect3dDriver();

        void ImGuiUpdate() final;

        int32 m_driverType = 2;

        bool m_Next = false;
    };
};