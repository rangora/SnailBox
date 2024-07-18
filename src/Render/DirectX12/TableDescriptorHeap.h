#pragma once

#include "corepch.h"
#include "Render/ShaderBase.h"

namespace sb
{
    class TableDescriptorHeap
    {
    public:
        TableDescriptorHeap();
        void Init(uint32 in_count);
        void CommitToDescriptorTable(ComPtr<ID3D12GraphicsCommandList> in_commandList);
        void SetCbvFromSource(D3D12_CPU_DESCRIPTOR_HANDLE in_srcHandle, CBV_Register in_reg);
        void Clear();

        ID3D12DescriptorHeap* GetRtvHeap() const { return m_descRtvHeap.Get(); }
        ID3D12DescriptorHeap* GetSrvHeap() const { return m_descSrvHeap.Get(); }
        ComPtr<ID3D12DescriptorHeap> GetCbvHeap() const { return m_cbvHeap; }
        D3D12_CPU_DESCRIPTOR_HANDLE* GetRenderTargetDescriptors() {return m_mainRenderTargetDescriptor; }

        D3D12_CPU_DESCRIPTOR_HANDLE GetRegisterCPUHandle(uint32 in_reg);

    private:
        ComPtr<ID3D12DescriptorHeap> m_descRtvHeap;
        ComPtr<ID3D12DescriptorHeap> m_descSrvHeap;
        uint64 m_handleSize = 0;
        uint64 m_groupSize = 0;
        uint64 m_groupCount = 0;
        uint32 m_currentGroupIndex = 0;

        uint32 m_rtvHandleIncrementSize = 0;
        D3D12_CPU_DESCRIPTOR_HANDLE m_cpuRtvHandleBegin = {};

        // 아마 RenderTargetBuffer에 있어야 할거 같은데 우선 여기에 둠..
        D3D12_CPU_DESCRIPTOR_HANDLE m_mainRenderTargetDescriptor[SWAP_CHAIN_BUFFER_COUNT] = {};

        // render
        ComPtr<ID3D12DescriptorHeap> m_cbvHeap = nullptr;
    };
};