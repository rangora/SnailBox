#include "CommandQueue.h"
#include "SwapChain.h"
#include "Core/DirectXCanvas.h"

namespace sb
{
    CommandQueue::CommandQueue(DirectXCanvas* in_canvas) : m_canvas(in_canvas)
    {
    }

    CommandQueue::~CommandQueue()
    {
        ::CloseHandle(m_fenceEvent);
    }

    void CommandQueue::Init(ComPtr<ID3D12Device> in_device, SwapChain* in_swapChain)
    {
        m_swapChain = in_swapChain;

        D3D12_COMMAND_QUEUE_DESC queueDesc = {};
        queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
        queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
        queueDesc.NodeMask = 1;

        // 분석 필
        in_device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&m_cmdQueue));

        for (int32 i = 0; i < NUM_FRAMES_IN_FLIGHT; i++)
        {
            in_device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT,
                                              IID_PPV_ARGS(&sg_d3dDriver->GetFrameContexts()[i].CommandAllocator));
        }

        in_device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT,
                                     sg_d3dDriver->GetFrameContexts()[0].CommandAllocator, nullptr,
                                     IID_PPV_ARGS(&m_cmdList));
        m_cmdList->Close();

        // fence 외부로 추출
        // in_device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_fence));
        // m_fenceEvent = ::CreateEvent(nullptr, FALSE, FALSE, nullptr);
    }

    void CommandQueue::WaitSync()
    {
        m_fenceValue++;
        m_cmdQueue->Signal(m_fence.Get(), m_fenceValue);
        if (m_fence->GetCompletedValue() < m_fenceValue)
        {
            m_fence->SetEventOnCompletion(m_fenceValue, m_fenceEvent);
            ::WaitForSingleObject(m_fenceEvent, INFINITE);
        }
    }

    void CommandQueue::RenderBegin(const D3D12_VIEWPORT* in_vp, const D3D12_RECT* in_rect)
    {
        m_cmdAllocator->Reset();
        m_cmdList->Reset(m_cmdAllocator.Get(), nullptr);

        D3D12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
            m_swapChain->GetBackRTVBuffer().Get(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);

        m_cmdList->SetGraphicsRootSignature(m_canvas->GetRootSignature()->GetSignature().Get());

        // 여기서부터 cb가 나온다.
    }

    void CommandQueue::RenderEnd()
    {
    }
} // namespace sb