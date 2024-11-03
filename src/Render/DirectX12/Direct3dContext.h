#pragma once

#include "Render/GraphicsContext.h"
#include "Render/RenderResource.h"
#include "Render/Vertex.h"
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

        // TEMP hardcode
        void Init(const std::vector<Vertex>& in_vertexBuffer, const std::vector<uint32>& in_indexBuffer);
        void CreateVertexBuffer(const std::vector<Vertex>& in_buffer);
        void CreateIndexBuffer(const std::vector<uint32>& in_buffer);
        // ~TEMP hardcode

    private:
        ComPtr<ID3D12Resource> m_vertexBuffer = nullptr;
        D3D12_VERTEX_BUFFER_VIEW m_vertexBufferView = {};
        uint32 m_vertexCount = 0;

        ComPtr<ID3D12Resource> m_indexBuffer = nullptr;
        D3D12_INDEX_BUFFER_VIEW m_indexBufferView;
        uint32 m_indexCount = 0;

        DirectX::XMFLOAT4 m_transform;

        class Direct3dDriver* m_direct3dDriver = nullptr;
    };
}; // namespace sb