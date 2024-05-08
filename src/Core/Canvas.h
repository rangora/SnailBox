#pragma once

#include "Actor/Actor.h"
#include "Common.h"
#include "Layout.h"
#include "Render/Camera.h"
#include "Render/GraphicsContext.h"
#include <vector>

namespace sb
{
    struct WinWindowData;
    class GraphicsContext;
    class Window;

    class Canvas : public Layout
    {
    public:
        Canvas(Window* in_window);
        Canvas() = delete;

        virtual void* GetNativeWindow() = 0;
        virtual bool InitCanvas(const WinWindowData* in_windowData) = 0;
        virtual void Render() = 0;
        virtual void Update(float in_delta) = 0;
        virtual void SwapBuffers() = 0;

        GraphicsContext* GetGraphcisContext() const { return m_graphicContext.get(); }

        std::vector<UPtr<Actor>> m_actors;

    protected:
        Window* m_window = nullptr;
        UPtr<GraphicsContext> m_graphicContext = nullptr;
        Camera m_camera;
    };
};