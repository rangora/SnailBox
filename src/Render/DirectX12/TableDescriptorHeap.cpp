#include "TableDescriptorHeap.h"
#include "Core/DirectXCanvas.h"

namespace sb
{
    TableDescriptorHeap::TableDescriptorHeap()
    {
    }

    void TableDescriptorHeap::Init(uint32 in_count)
    {
        m_groupCount = in_count;

        {
            // RTV
            D3D12_DESCRIPTOR_HEAP_DESC desc = {};
            desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
            desc.NumDescriptors = SWAP_CHAIN_BUFFER_COUNT; // num of backBuffers
            desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
            desc.NodeMask = 1;

            if (sg_d3dDevice->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&m_descRtvHeap)) != S_OK)
            {
                return;
            }

            m_rtvHandleIncrementSize =
                sg_d3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
            m_cpuRtvHandleBegin = m_descRtvHeap->GetCPUDescriptorHandleForHeapStart();
            D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = m_cpuRtvHandleBegin;
            for (int32 i = 0; i < SWAP_CHAIN_BUFFER_COUNT; i++)
            {
                m_mainRenderTargetDescriptor[i] = rtvHandle;
                rtvHandle.ptr += m_rtvHandleIncrementSize;
            }
        }

        {
            //CBV SRV UAV
            D3D12_DESCRIPTOR_HEAP_DESC desc = {};
            desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
            desc.NumDescriptors = 1;
            desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
            if (sg_d3dDevice->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&m_descSrvHeap)) != S_OK)
            {
                return;
            }
        }
    }

    D3D12_CPU_DESCRIPTOR_HANDLE TableDescriptorHeap::GetCPUHandle(uint32 in_reg)
    {
        return CD3DX12_CPU_DESCRIPTOR_HANDLE(m_cpuRtvHandleBegin, in_reg * m_rtvHandleIncrementSize);
    }
}; // namespace sb