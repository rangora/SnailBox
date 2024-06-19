#include "SwapChain.h"
#include "Core/DirectXCanvas.h"
#include "TableDescriptorHeap.h"
#include "Core/Application.h"

namespace sb
{
    void SwapChain::Init(ComPtr<ID3D12Device> in_device, ComPtr<IDXGIFactory4> in_dxgi,
                         ComPtr<ID3D12CommandQueue> in_cmdQueue, HWND in_hwnd)
    {
        CreateSwapChain(in_dxgi, in_cmdQueue, in_hwnd);
        // CreateRTV(in_device);
    }

    void SwapChain::Present()
    {
        m_swapChain3->Present(0, 0);
    }

    void SwapChain::SwapIndex()
    {
        m_backBufferIndex = (m_backBufferIndex + 1) % SWAP_CHAIN_BUFFER_COUNT;
    }

    void SwapChain::CreateSwapChain(ComPtr<IDXGIFactory4> in_dxgi, ComPtr<ID3D12CommandQueue> in_cmdQueue, HWND in_hwnd)
    {
        //m_swapChain3.Reset();

        DXGI_SWAP_CHAIN_DESC1 sd;
        ZeroMemory(&sd, sizeof(sd));
        sd.BufferCount = SWAP_CHAIN_BUFFER_COUNT;
        sd.Width = 0;
        sd.Height = 0;
        sd.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        sd.Flags = DXGI_SWAP_CHAIN_FLAG_FRAME_LATENCY_WAITABLE_OBJECT;
        sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        sd.SampleDesc.Count = 1;
        sd.SampleDesc.Quality = 0;
        sd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
        sd.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
        sd.Scaling = DXGI_SCALING_STRETCH;
        sd.Stereo = FALSE;

        IDXGISwapChain1* m_swapChain1 = nullptr;
        in_dxgi->CreateSwapChainForHwnd(in_cmdQueue.Get(), in_hwnd, &sd, nullptr, nullptr, &m_swapChain1);

        m_swapChain1->QueryInterface(IID_PPV_ARGS(&m_swapChain3));
        m_swapChain1->Release();
        m_swapChain3->SetMaximumFrameLatency(SWAP_CHAIN_BUFFER_COUNT);
        sg_d3dDriver->SetSwapChainWaitableObject(m_swapChain3->GetFrameLatencyWaitableObject());

        // CreateRenderTarget
        // 이 방식은 lecture 방식과 imgui방식이 다른데 우선 imgui방식을 따름.
        auto Heap = sg_d3dDriver->GetDescriptorHeap();

        for (int32 i = 0; i < SWAP_CHAIN_BUFFER_COUNT; i++)
        {
            ID3D12Resource* pBackBuffer = nullptr;
            HRESULT hr = m_swapChain3->GetBuffer(i, IID_PPV_ARGS(&pBackBuffer));
            if (FAILED(hr))
            {
                assert(false);
                continue;
            }

            sg_d3dDevice->CreateRenderTargetView(pBackBuffer, nullptr, Heap->GetRenderTargetDescriptors()[i]);
            m_mainRenderTargetResources[i] = pBackBuffer;
        }
    }

    void SwapChain::CreateRTV(ComPtr<ID3D12Device> in_device)
    {
        // int32 rtvHeapSize = in_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

        // D3D12_DESCRIPTOR_HEAP_DESC rtvDesc;
        // rtvDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
        // rtvDesc.NumDescriptors = SWAP_CHAIN_BUFFER_COUNT;
        // rtvDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
        // rtvDesc.NodeMask = 1;

        // in_device->CreateDescriptorHeap(&rtvDesc, IID_PPV_ARGS(&m_rtvHeap));

        // D3D12_CPU_DESCRIPTOR_HANDLE rtvHeapBegin = m_rtvHeap->GetCPUDescriptorHandleForHeapStart();
        // for (int32 i = 0; i < SWAP_CHAIN_BUFFER_COUNT; i++)
        // {
        //     m_rtvHandle[i] = CD3DX12_CPU_DESCRIPTOR_HANDLE(rtvHeapBegin, i * rtvHeapSize);
        //     in_device->CreateRenderTargetView(m_mainRenderTargetResources[i], nullptr, m_rtvHandle[i]);
        // }
    }
}; // namespace sb