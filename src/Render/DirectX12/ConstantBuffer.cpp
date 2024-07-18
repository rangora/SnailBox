#include "ConstantBuffer.h"
#include "Core/Application.h"

namespace sb
{
    ConstantBuffer::~ConstantBuffer()
    {
        if (m_cbvBuffer)
        {
            if (m_cbvBuffer != nullptr)
            {
                m_cbvBuffer->Unmap(0, nullptr);
            }
            m_cbvBuffer = nullptr;
        }
    }

    void ConstantBuffer::Init(uint32 in_size, uint32 in_count)
    {
        // 상수 버퍼는 256바이트의 배수로 만들어 줘야 함!!
        m_elementSize = (in_size + 255) & ~255;
        m_elementCount = in_count;

        CreateBuffer();
        CreateView();
    }

    void ConstantBuffer::Clear()
    {
        m_currentIndex = 0;
    }

    D3D12_GPU_VIRTUAL_ADDRESS ConstantBuffer::GetGpuVirtualAddress(uint32 in_index)
    {
        D3D12_GPU_VIRTUAL_ADDRESS objCBAddress = m_cbvBuffer->GetGPUVirtualAddress();
        objCBAddress += in_index * m_elementSize;
        return objCBAddress;
    }

    D3D12_CPU_DESCRIPTOR_HANDLE ConstantBuffer::GetCpuHandle(uint32 in_index)
    {
        return CD3DX12_CPU_DESCRIPTOR_HANDLE(m_cpuHandleBegin, in_index * m_handleIncrementSize);
    }

    void ConstantBuffer::CreateBuffer()
    {
        uint32 bufferSize = m_elementSize * m_elementCount;
        // D3D12_HEAP_TYPE_UPLOAD는 cpu가 쓰고 gpu가 읽을 수 있는 영역을 의미.
        D3D12_HEAP_PROPERTIES heapProperty = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
        D3D12_RESOURCE_DESC desc = CD3DX12_RESOURCE_DESC::Buffer(bufferSize);

        // D3D12_RESOURCE_STATE_GENERIC_READ : gpu가 읽는다.
        // m_cbvBuffer는 gpu resource로 gpu에 할당된 리소스이다.
        sg_d3dDevice->CreateCommittedResource(&heapProperty, D3D12_HEAP_FLAG_NONE, &desc,
                                              D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&m_cbvBuffer));

        // m_cbvBuffer는 gpu자원으로 cpu가 직접 못 읽기 때문에 맵핑을 시킨다.
        //  m_mappedBuffer에 cpu가 쓸 수 있는 cpu 주소 공간을 맵핑한다.
        // 그리고 gpu가 읽느다.
        m_cbvBuffer->Map(0, nullptr, reinterpret_cast<void**>(&m_mappedBuffer));
    }

    void ConstantBuffer::CreateView()
    {
        D3D12_DESCRIPTOR_HEAP_DESC cbvDesc = {};
        cbvDesc.NumDescriptors = m_elementCount;
        cbvDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
        cbvDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
        sg_d3dDevice->CreateDescriptorHeap(&cbvDesc, IID_PPV_ARGS(&m_cbvHeap));

        m_cpuHandleBegin = m_cbvHeap->GetCPUDescriptorHandleForHeapStart();
        m_handleIncrementSize = sg_d3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

        for (int32 i = 0; i < m_elementCount; ++i)
        {
            D3D12_CPU_DESCRIPTOR_HANDLE cbvHandle = GetCpuHandle(i);
            D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc = {};
            cbvDesc.BufferLocation = m_cbvBuffer->GetGPUVirtualAddress() + static_cast<uint64>(m_elementSize) * i;
            cbvDesc.SizeInBytes = m_elementSize;
            sg_d3dDevice->CreateConstantBufferView(&cbvDesc, cbvHandle);
        }
    }
} // namespace sb