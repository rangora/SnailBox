#include "IndexBuffer.h"
#include "Render/ShaderGeometry.h"

namespace sb
{
    IndexBuffer::~IndexBuffer()
    {
        if (_bufferUploadHeap)
        {
            _bufferUploadHeap.Reset();
        }

        if (_buffer)
        {
            _buffer.Reset();
        }
    }

    HRESULT IndexBuffer::Initialize(ID3D12Device* device, ID3D12GraphicsCommandList* commandList, ShaderGeometry* geo,
                                    D3D12_RESOURCE_BARRIER& outBarrier)
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

        _bufferSize = geo->GetIndexByteSize();

        // Create default heap
        D3D12_HEAP_PROPERTIES defaultHeapProps = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
        D3D12_RESOURCE_DESC defaultResoourceDesc = CD3DX12_RESOURCE_DESC::Buffer(_bufferSize);
        device->CreateCommittedResource(&defaultHeapProps, D3D12_HEAP_FLAG_NONE, &defaultResoourceDesc,
                                        D3D12_RESOURCE_STATE_COPY_DEST, nullptr, IID_PPV_ARGS(_buffer.GetAddressOf()));
        _buffer->SetName(L"Index Buffer Upload Resource Heap");

        // Create upload heap
        D3D12_HEAP_PROPERTIES uploadHeapProps = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
        D3D12_RESOURCE_DESC uploadResoourceDesc = CD3DX12_RESOURCE_DESC::Buffer(_bufferSize);
        device->CreateCommittedResource(&uploadHeapProps, D3D12_HEAP_FLAG_NONE, &uploadResoourceDesc,
                                        D3D12_RESOURCE_STATE_GENERIC_READ, nullptr,
                                        IID_PPV_ARGS(_bufferUploadHeap.GetAddressOf()));
        _bufferUploadHeap->SetName(L"Index Buffer Upload Resource Heap");

        D3D12_SUBRESOURCE_DATA data = {};
        data.pData = reinterpret_cast<BYTE*>(geo->GetIndexPointer());
        data.RowPitch = _bufferSize;
        data.SlicePitch = _bufferSize;

        UpdateSubresources(commandList, _buffer.Get(), _bufferUploadHeap.Get(), 0, 0, 1, &data);

        _bufferView.BufferLocation = _buffer->GetGPUVirtualAddress();
        _bufferView.Format = DXGI_FORMAT_R32_UINT;
        _bufferView.SizeInBytes = _bufferSize;

        outBarrier = CD3DX12_RESOURCE_BARRIER::Transition(_buffer.Get(), D3D12_RESOURCE_STATE_COPY_DEST,
                                                          D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);
        return S_OK;
    }

} // namespace sb