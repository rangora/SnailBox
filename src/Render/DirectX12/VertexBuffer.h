#pragma once

#include "corepch.h"
#include "Render/ShaderGeometry.h"

namespace sb
{
    template<class T>
    class VertexBuffer
    {
    public:
        VertexBuffer() = default;
        ~VertexBuffer();
        HRESULT Initialize(ID3D12Device* device, ID3D12GraphicsCommandList* commandList, ShaderGeometry* geo,
                           D3D12_RESOURCE_BARRIER& outBarrier);

        ID3D12Resource* Get() const { return _buffer.Get(); }
        ID3D12Resource* GetAddressOf() const { return _buffer.GetAddressOf(); }
        D3D12_VERTEX_BUFFER_VIEW* GetBufferView() { return &_bufferView; }

    private:
        VertexBuffer(const VertexBuffer<T>& rhs) {}

        ComPtr<ID3D12Resource> _buffer = nullptr;
        ComPtr<ID3D12Resource> _bufferUploadHeap = nullptr;
        D3D12_VERTEX_BUFFER_VIEW _bufferView = {};

        uint32 _stride;
        uint32 _bufferSize = 0;
    };

    template <class T>
    VertexBuffer<T>::~VertexBuffer()
    {
        if (_buffer)
        {
            _buffer.Reset();
        }

        if (_bufferUploadHeap)
        {
            _bufferUploadHeap.Reset();
        }
    }

    template <class T>
    HRESULT VertexBuffer<T>::Initialize(ID3D12Device* device, ID3D12GraphicsCommandList* commandList,
                                        ShaderGeometry* geo, D3D12_RESOURCE_BARRIER& outBarrier)
    {
        if (geo == nullptr)
        {
            spdlog::error("Invalid shaderGeometry.");
            return E_FAIL;
        }

        if (_buffer.Get() != nullptr)
        {
            _buffer.Reset();
        }

        _bufferSize = geo->GetVertexByteSize();
        _stride = geo->GetStrideSize();

        // Create defaultHeap
        D3D12_HEAP_PROPERTIES defaultHeapProps = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
        D3D12_RESOURCE_DESC defaultResourceDesc = CD3DX12_RESOURCE_DESC::Buffer(_bufferSize);

        device->CreateCommittedResource(&defaultHeapProps, D3D12_HEAP_FLAG_NONE, &defaultResourceDesc,
                                        D3D12_RESOURCE_STATE_COPY_DEST, nullptr, IID_PPV_ARGS(&_buffer));
        _buffer->SetName(L"VertexBuffer DefaultResourceHeap");

        // CreateUploadHeap
        D3D12_HEAP_PROPERTIES uploadHeapProps = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
        D3D12_RESOURCE_DESC uploadResoourceDesc = CD3DX12_RESOURCE_DESC::Buffer(_bufferSize);

        device->CreateCommittedResource(&uploadHeapProps, D3D12_HEAP_FLAG_NONE, &uploadResoourceDesc,
                                              D3D12_RESOURCE_STATE_GENERIC_READ, nullptr,
                                              IID_PPV_ARGS(&_bufferUploadHeap));
        _bufferUploadHeap->SetName(L"VertexBuffer UploadeResourceHeap");

        D3D12_SUBRESOURCE_DATA vData = {};
        vData.pData = reinterpret_cast<BYTE*>(geo->GetVertexPointer());
        vData.RowPitch = _bufferSize;
        vData.SlicePitch = _bufferSize;

        UpdateSubresources(commandList, _buffer.Get(), _bufferUploadHeap.Get(), 0, 0, 1, &vData);

        _bufferView.BufferLocation = _buffer->GetGPUVirtualAddress();
        _bufferView.StrideInBytes = sizeof(T);
        _bufferView.SizeInBytes = _bufferSize;

        outBarrier = CD3DX12_RESOURCE_BARRIER::Transition(_buffer.Get(), D3D12_RESOURCE_STATE_COPY_DEST,
                                                       D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);

        return S_OK;
    }

} // namespace sb