#pragma once

#include "corepch.h"
#include "Core/DirectXCanvas.h"

namespace sb
{
    class SwapChain
    {
    public:
        void Init(ComPtr<ID3D12Device> in_device, ComPtr<IDXGIFactory4> in_dxgi, ComPtr<ID3D12CommandQueue> in_cmdQueue,
                  HWND in_hwnd);
        void Clear();
        void Present();
        void SwapIndex();

        ComPtr<IDXGISwapChain3> GetSwapChain() { return m_swapChain3; }
        IDXGISwapChain3* GetSwapChain3() { return m_swapChain3.Get(); }
        ComPtr<ID3D12Resource> GetRenderTarget(int32 in_index) { return m_rtvBuffer[in_index]; }
        ComPtr<ID3D12Resource> GetBackRTVBuffer() { return m_rtvBuffer[m_backBufferIndex]; }
        D3D12_CPU_DESCRIPTOR_HANDLE GetBackRTV() { return m_rtvHandle[m_backBufferIndex]; }

        ID3D12Resource* GetMainRenderTargetResource(const int32 in_index) const { return m_mainRenderTargetResources[in_index]; }
        ID3D12Resource** GetMainRenderTargetResources() { return m_mainRenderTargetResources; }
    
    private:
        void CreateSwapChain(ComPtr<IDXGIFactory4> in_dxgi, ComPtr<ID3D12CommandQueue> in_cmdQueue, HWND in_hwnd);
        void CreateRTV(ComPtr<ID3D12Device> in_device);

        ComPtr<IDXGISwapChain3> m_swapChain3;
        ComPtr<ID3D12Resource> m_rtvBuffer[SWAP_CHAIN_BUFFER_COUNT];
        ComPtr<ID3D12DescriptorHeap> m_rtvHeap;
        D3D12_CPU_DESCRIPTOR_HANDLE m_rtvHandle[SWAP_CHAIN_BUFFER_COUNT]; // lecture 방식
        
        ID3D12Resource* m_mainRenderTargetResources[SWAP_CHAIN_BUFFER_COUNT]; // imgui 방식

        uint32 m_backBufferIndex = 0;
    };
};