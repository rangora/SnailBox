#include "ConstantBuffer.h"
#include "core/Application.h"

namespace sb
{
    ConstantBuffer::~ConstantBuffer()
    {
        if (_cbvBuffer)
        {
            _cbvBuffer.Reset();
        }
    }

    void ConstantBuffer::Initialize(CBV_Register reg, uint32 size, uint32 count)
    {
        _reg = reg;

        // 상수 버퍼는 256 바이트의 배수여야 함
        _elementSize = (size + 255) & ~255;
        _elementCount = count;

        CreateBuffer();
        CreateView();
    }

    void ConstantBuffer::Clear()
    {
    }

    void ConstantBuffer::PushData(void* buffer, uint32 size)
    {
    }

    void ConstantBuffer::SetGlobalData(void* buffer, uint32 size)
    {
    }

    D3D12_GPU_VIRTUAL_ADDRESS ConstantBuffer::GetGpuVirtualAddress(uint32 index)
    {
        D3D12_GPU_VIRTUAL_ADDRESS addr = _cbvBuffer->GetGPUVirtualAddress();
        addr += index * _elementSize;
        return addr;
    }

    D3D12_CPU_DESCRIPTOR_HANDLE ConstantBuffer::GetCpuHandle(uint32 index)
    {
        return CD3DX12_CPU_DESCRIPTOR_HANDLE(_cpuHandleBegin, index * _handleIncrementSize);
    }

    void ConstantBuffer::CreateBuffer()
    {
        uint32 bufferSize = _elementSize * _elementCount;
        D3D12_HEAP_PROPERTIES heapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
        D3D12_RESOURCE_DESC desc = CD3DX12_RESOURCE_DESC::Buffer(bufferSize);

        sg_d3dDevice->CreateCommittedResource(&heapProp, D3D12_HEAP_FLAG_NONE, &desc, D3D12_RESOURCE_STATE_GENERIC_READ,
                                              nullptr, IID_PPV_ARGS(&_cbvBuffer));

        _cbvBuffer->SetName(L"Constant Buffer Upload Resource Heap");

        _cbvBuffer->Map(0, nullptr, reinterpret_cast<void**>(&_mappedBuffer));
    }

    void ConstantBuffer::CreateView()
    {
        D3D12_DESCRIPTOR_HEAP_DESC desc = {};
        desc.NumDescriptors = _elementCount;
        desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
        desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
        sg_d3dDevice->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&_cbvHeap));

        _cpuHandleBegin = _cbvHeap->GetCPUDescriptorHandleForHeapStart();
        _handleIncrementSize = sg_d3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

        for (uint32 i = 0; i < _elementCount; ++i)
        {
            D3D12_CPU_DESCRIPTOR_HANDLE handle = GetCpuHandle(i);
            D3D12_CONSTANT_BUFFER_VIEW_DESC viewDesc = {};
            viewDesc.BufferLocation = _cbvBuffer->GetGPUVirtualAddress() + static_cast<uint64>(_elementSize) * i;
            viewDesc.SizeInBytes = _elementSize;

            sg_d3dDevice->CreateConstantBufferView(&viewDesc, handle);
        }
    }
} // namespace sb