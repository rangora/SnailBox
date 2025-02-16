#include "RootDescriptorHeap.h"
#include "Core/Application.h"

namespace sb
{
    void RootDescriptorHeap::Initialize(uint32 count)
    {
        _groupCount = count;

        // shader visible
        D3D12_DESCRIPTOR_HEAP_DESC desc = {};
        desc.NumDescriptors = SRV_REGISTER_COUNT + CBV_REGISTER_COUNT + UAV_REGISTER_COUNT;
        desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
        desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;

        sg_d3dDevice->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&_descHeap));

        _handleSize = sg_d3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
        _groupSize = _handleSize * (SRV_REGISTER_COUNT + CBV_REGISTER_COUNT + UAV_REGISTER_COUNT - 1);
    }

    void RootDescriptorHeap::Clear()
    {
        _currentGroupIndex = 0;
    }

    void RootDescriptorHeap::SetCBV(D3D12_CPU_DESCRIPTOR_HANDLE srcHandle, CBV_Register reg)
    {
        D3D12_CPU_DESCRIPTOR_HANDLE handle = GetCPUHandle(reg);
        uint32 destRange = 1;
        uint32 srcRange = 1;
        sg_d3dDevice->CopyDescriptors(1, &handle, &destRange, 1, &srcHandle, &srcRange,
                                      D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
    }

    void RootDescriptorHeap::SetRootDescriptorTable(ID3D12GraphicsCommandList* commandList)
    {
        D3D12_GPU_DESCRIPTOR_HANDLE handle = _descHeap->GetGPUDescriptorHandleForHeapStart();
        handle.ptr += _currentGroupIndex * _groupSize;
        commandList->SetGraphicsRootDescriptorTable(1, handle);
        _currentGroupIndex++;
    }

    D3D12_CPU_DESCRIPTOR_HANDLE RootDescriptorHeap::GetCPUHandle(CBV_Register reg)
    {
        uint8 val = static_cast<uint8>(reg);

        D3D12_CPU_DESCRIPTOR_HANDLE handle = _descHeap->GetCPUDescriptorHandleForHeapStart();
        handle.ptr += _currentGroupIndex * _groupSize;
        handle.ptr += (val - 1) * _handleSize;
        return handle;
    }
} // namespace sb