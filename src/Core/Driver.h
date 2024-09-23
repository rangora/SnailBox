#pragma once

#include "Actor/Actor.h"
#include "Common.h"
#include "Layout.h"
#include "Render/Camera.h"
#include "Render/GraphicsContext.h"
#include <vector>

namespace sb
{
    struct WindowData;
    class GraphicsContext;
    class Window;

    class Driver : public Layout
    {
    public:
        Driver(Window* in_window);
        Driver() = default;

        virtual void* GetNativeWindow() = 0; // 제거 예정
        virtual bool InitDriver(const WindowData* in_windowData) { return false; } // 제거 예정
        virtual bool InitializeResources() { return false; }
        virtual bool BindWinWindow(const HWND in_hwnd) { return false; };
        virtual bool BindglfwWindow() { return false; };
        virtual void Render() = 0;
        virtual void Update() = 0;
        virtual void SwapBuffers() = 0;
        virtual bool IsWindowShouldClosed() = 0;
        virtual void CleanDriver() {};

        GraphicsContext* GetGraphcisContext() const { return m_graphicContext.get(); }

        std::vector<UPtr<Actor>> m_actors;

    protected:
        Window* m_window = nullptr;
        UPtr<GraphicsContext> m_graphicContext = nullptr;
        Camera m_camera;
    };
};