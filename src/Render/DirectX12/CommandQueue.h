#pragma once

#include <Core/Common.h>
#include <d3d12.h>

namespace sb
{
    class SwapChain;

    class CommandQueue
    {
    public:
        CommandQueue();
        ~CommandQueue();

        void Init(ComPtr<ID3D12Device> in_device);
        // void WaitSync();

        // void RenderBegin(const D3D12_VIEWPORT* in_vp, const D3D12_RECT* in_rect);
        // void RenderEnd();

        ComPtr<ID3D12CommandQueue> GetCmdQueue() { return m_cmdQueue; }
        ComPtr<ID3D12GraphicsCommandList> GetCmdList() { return m_cmdList; }

    private:
        ComPtr<ID3D12CommandQueue> m_cmdQueue;
        ComPtr<ID3D12GraphicsCommandList> m_cmdList;
        // ComPtr<ID3D12CommandAllocator> m_cmdAllocator;

        // ComPtr<ID3D12Fence> m_fence;
        // uint32 m_fenceValue = 0;
        // HANDLE m_fenceEvent = INVALID_HANDLE_VALUE;

        SwapChain* m_swapChain;
    };
};