#include "TableDescriptorHeap.h"
#include "Core/Application.h"
#include "Core/Direct3dDriver.h"
#include "Render/ShaderBase.h"

namespace sb
{
    TableDescriptorHeap::TableDescriptorHeap()
    {
    }

    void TableDescriptorHeap::Init(uint32 in_count)
    {

        // RTV
        {
            D3D12_DESCRIPTOR_HEAP_DESC desc = {};
            desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
            desc.NumDescriptors = SWAP_CHAIN_BUFFER_COUNT; // num of backBuffers
            desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
            desc.NodeMask = 1;

            if (sg_d3dDevice->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&m_descRtvHeap)) != S_OK)
            {
                return;
            }

            m_rtvHandleIncrementSize = sg_d3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
            m_cpuRtvHandleBegin = m_descRtvHeap->GetCPUDescriptorHandleForHeapStart();
            D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = m_cpuRtvHandleBegin;
            for (int32 i = 0; i < SWAP_CHAIN_BUFFER_COUNT; i++)
            {
                m_mainRenderTargetDescriptor[i] = rtvHandle;
                rtvHandle.ptr += m_rtvHandleIncrementSize;
            }
        }

        // CBV SRV UAV
        {
            D3D12_DESCRIPTOR_HEAP_DESC desc = {};
            desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
            desc.NumDescriptors = 1;
            desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
            if (sg_d3dDevice->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&m_descSrvHeap)) != S_OK)
            {
                return;
            }
        }

        // CBV
        m_groupCount = in_count;

        const uint32 RegisterCount = static_cast<uint32>(CBV_Register::END);

        D3D12_DESCRIPTOR_HEAP_DESC desc = {};
        desc.NumDescriptors = in_count;
        desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
        desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
        sg_d3dDevice->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&m_cbvHeap));

        m_handleSize = sg_d3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
        m_groupSize = m_handleSize * RegisterCount;
    }

    void TableDescriptorHeap::CommitToDescriptorTable(ComPtr<ID3D12GraphicsCommandList> in_commandList)
    {
        D3D12_GPU_DESCRIPTOR_HANDLE handle = m_cbvHeap->GetGPUDescriptorHandleForHeapStart();
        handle.ptr += m_currentGroupIndex * m_groupSize;
        in_commandList->SetGraphicsRootDescriptorTable(0, handle);
        m_currentGroupIndex++;
    }

    void TableDescriptorHeap::SetCbvFromSource(D3D12_CPU_DESCRIPTOR_HANDLE in_srcHandle, CBV_Register in_reg)
    {
        D3D12_CPU_DESCRIPTOR_HANDLE destHandle = GetRegisterCPUHandle(static_cast<uint32>(in_reg));
        uint32 destRange = 1;
        uint32 srcRange = 1;

        sg_d3dDevice->CopyDescriptors(1, &destHandle, &destRange, 1, &in_srcHandle,
                                      &srcRange, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
    }

    void TableDescriptorHeap::Clear()
    {
        m_currentGroupIndex = 0;
    }

    D3D12_CPU_DESCRIPTOR_HANDLE TableDescriptorHeap::GetRegisterCPUHandle(uint32 in_reg)
    {
        D3D12_CPU_DESCRIPTOR_HANDLE startHandle = m_cbvHeap->GetCPUDescriptorHandleForHeapStart();
        return CD3DX12_CPU_DESCRIPTOR_HANDLE(startHandle,
                                             (m_currentGroupIndex * m_groupSize) + (in_reg * m_handleSize));
    }
}; // namespace sb