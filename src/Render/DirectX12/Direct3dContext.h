#pragma once

#include "Render/GraphicsContext.h"
#include "corepch.h"

namespace sb
{
    class Direct3dContext : public GraphicsContext
    {
    public:
        Direct3dContext() = default;
        Direct3dContext(class Driver* in_driver);

        void Initialize() final;
        void SwapBuffers() final;

        void Render() final;
        void VertexProcess(SPtr<class VertexRenderResource>& in_vertexResource) final;
        void StaticMeshProcess(SPtr<class StaticMeshRenderResource>& in_staticMeshResource) final;

        void CreateBuffer() final;
        void BindBuffer() final;
        void AddData(const void* in_data) final;
        void CommitData() final;

    private:
        class Direct3dDriver* m_direct3dDriver = nullptr;
    };
}; // namespace sb