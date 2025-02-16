#pragma once

#include "corepch.h"
#include "ShaderStructure.h"
#include "Core/Common.h"

namespace sb
{
    class RootDescriptorHeap
    {
    public:
        RootDescriptorHeap() = default;
        ~RootDescriptorHeap() = default;

        void Initialize(uint32 count);
        void Clear();

        void SetCBV(D3D12_CPU_DESCRIPTOR_HANDLE srcHandle, CBV_Register reg);
        void SetRootDescriptorTable(ID3D12GraphicsCommandList* commandList);

        D3D12_CPU_DESCRIPTOR_HANDLE GetCPUHandle(CBV_Register reg);

        ComPtr<ID3D12DescriptorHeap> GetDescriptorHeap() { return _descHeap; }

    private:
        ComPtr<ID3D12DescriptorHeap> _descHeap = nullptr;
        uint64 _handleSize = 0;
        uint64 _groupSize = 0;
        uint64 _groupCount = 0;
        uint32 _currentGroupIndex = 0;
    };
} // namespace sb