#include "Direct3dContext.h"
#include "Core/Application.h"
#include "Core/Direct3dDriver.h"
#include "Render/RenderResource.h"

namespace sb
{
    Direct3dContext::Direct3dContext(Driver* in_driver) : m_direct3dDriver(static_cast<Direct3dDriver*>(in_driver))
    {
    }

    void Direct3dContext::Initialize()
    {
    }

    void Direct3dContext::SwapBuffers()
    {
    }

    void Direct3dContext::Render()
    {
    }

    void Direct3dContext::VertexProcess(SPtr<VertexRenderResource>& in_vertexResource)
    {
    }

    void Direct3dContext::StaticMeshProcess(SPtr<StaticMeshRenderResource>& in_staticMeshResource)
    {
    }

    void Direct3dContext::CreateBuffer()
    {
    }

    void Direct3dContext::BindBuffer()
    {
    }

    void Direct3dContext::AddData(const void* in_data)
    {
    }

    void Direct3dContext::CommitData()
    {
    }

    void Direct3dContext::Init(const std::vector<Vertex>& in_vertexBuffer, const std::vector<uint32>& in_indexBuffer)
    {
        CreateVertexBuffer(in_vertexBuffer);
        CreateIndexBuffer(in_indexBuffer);
    }

    void Direct3dContext::CreateVertexBuffer(const std::vector<Vertex>& in_buffer)
    {
        m_vertexCount = static_cast<uint32>(in_buffer.size());
        uint32 bufferSize = m_vertexCount * sizeof(Vertex);

        D3D12_HEAP_PROPERTIES heapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
        D3D12_RESOURCE_DESC desc = CD3DX12_RESOURCE_DESC::Buffer(bufferSize);

        sg_d3dDevice->CreateCommittedResource(&heapProp, D3D12_HEAP_FLAG_NONE, &desc, D3D12_RESOURCE_STATE_GENERIC_READ,
                                              nullptr, IID_PPV_ARGS(&m_vertexBuffer));

        void* vertexDataBuffer = nullptr;
        CD3DX12_RANGE readRange(0, 0);

        m_vertexBuffer->Map(0, &readRange, &vertexDataBuffer);
        ::memcpy(vertexDataBuffer, &in_buffer[0], bufferSize);
        m_vertexBuffer->Unmap(0, nullptr);

        m_vertexBufferView.BufferLocation = m_vertexBuffer->GetGPUVirtualAddress();
        m_vertexBufferView.StrideInBytes = sizeof(Vertex);
        m_vertexBufferView.SizeInBytes = bufferSize;
    }

    void Direct3dContext::CreateIndexBuffer(const std::vector<uint32>& in_buffer)
    {
        m_indexCount = static_cast<uint32>(in_buffer.size());
        uint32 bufferSize = m_indexCount * sizeof(Vertex);

        D3D12_HEAP_PROPERTIES heapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
        D3D12_RESOURCE_DESC desc = CD3DX12_RESOURCE_DESC::Buffer(bufferSize);

        sg_d3dDevice->CreateCommittedResource(&heapProp, D3D12_HEAP_FLAG_NONE, &desc, D3D12_RESOURCE_STATE_GENERIC_READ,
                                              nullptr, IID_PPV_ARGS(&m_indexBuffer));

        void* indexDataBuffer = nullptr;
        CD3DX12_RANGE readRange(0, 0);
        m_indexBuffer->Map(0, &readRange, &indexDataBuffer);
        ::memcpy(indexDataBuffer, &in_buffer[0], bufferSize);
        m_indexBuffer->Unmap(0, nullptr);

        m_indexBufferView.BufferLocation = m_indexBuffer->GetGPUVirtualAddress();
        m_indexBufferView.Format = DXGI_FORMAT_R32_UINT;
        m_indexBufferView.SizeInBytes = bufferSize;
    }
} // namespace sb