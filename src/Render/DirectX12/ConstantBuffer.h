#pragma once

#include "corepch.h"
#include "coreMinimal.h"
#include "ShaderStructure.h"

namespace sb
{
    class ConstantBuffer
    {
    public:
        ConstantBuffer() = default;
        ~ConstantBuffer();

        void Initialize(CBV_Register reg, uint32 size, uint32 count);
        void Clear();

        void PushData(void* buffer, uint32 size);
        void SetGlobalData(ID3D12GraphicsCommandList* cmdList, void* buffer, uint32 size);

        D3D12_GPU_VIRTUAL_ADDRESS GetGpuVirtualAddress(uint32 index);
        D3D12_CPU_DESCRIPTOR_HANDLE GetCpuHandle(uint32 index);

    private:
        void CreateBuffer();
        void CreateView();

        ComPtr<ID3D12Resource> _cbvBuffer = nullptr;
        ComPtr<ID3D12DescriptorHeap> _cbvHeap = nullptr;
        D3D12_CPU_DESCRIPTOR_HANDLE _cpuHandleBegin = {};

        BYTE* _mappedBuffer = nullptr;
        uint32 _elementSize = 0;
        uint32 _elementCount = 0;
        uint32 _handleIncrementSize = 0;
        uint32 _curIdx = 0;

        CBV_Register _reg = {};
    };
} // namespace sb