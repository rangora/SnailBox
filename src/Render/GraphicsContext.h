#pragma once

#include "Core/Common.h"
#include "Enums.h"
#include "RenderBuffer.h"
#include <queue>
#include <vector>

namespace sb
{
    class Window;
    class Canvas;

    class GraphicsContext
    {
    public:
        virtual ~GraphicsContext() = default;

        virtual void Initialize() = 0;
        virtual void SwapBuffers() = 0;
        virtual void Render() = 0; // ??
        virtual void VertexProcess(SPtr<class VertexRenderResource>& in_vertexResource) = 0;
        virtual void StaticMeshProcess(SPtr<class StaticMeshRenderResource>& in_staticMeshResource) = 0;

        virtual void CreateBuffer() = 0;
        virtual void BindBuffer() = 0;
        virtual void AddData(const void* in_data) = 0;
        virtual void CommitData() = 0;

        static UPtr<GraphicsContext> Create(void* in_glWindow, Canvas* in_canvas);

        std::vector<class RenderResource*> m_targetRenderResources;
        std::vector<SPtr<RenderBuffer>> m_renderbuffers;

        GraphicsDevice m_graphicsFramework = GraphicsDevice::None;

        Window* m_window_handle = nullptr;
    };
} // namespace sb