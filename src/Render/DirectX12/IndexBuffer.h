#pragma once

#include "corepch.h"
#include "coreMinimal.h"

namespace sb
{
    class IndexBuffer
    {
    public:
        IndexBuffer() = default;
        ~IndexBuffer();
        HRESULT Initialize(ID3D12Device* device, ID3D12GraphicsCommandList* commandList, class ShaderGeometry* geo,
                           D3D12_RESOURCE_BARRIER& outBarrier);
        D3D12_INDEX_BUFFER_VIEW* GetBufferView() { return &_bufferView; }

    private:
        IndexBuffer(const IndexBuffer& rhs) {}

        ComPtr<ID3D12Resource> _bufferUploadHeap = nullptr;
        ComPtr<ID3D12Resource> _buffer = nullptr;
        D3D12_INDEX_BUFFER_VIEW _bufferView = {};

        uint32 _bufferSize = 0;
    };
} // namespace sb