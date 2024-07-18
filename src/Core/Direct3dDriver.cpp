#include "Direct3dDriver.h"

#include "corepch.h"
#include "imgui.h"
#include "imgui_impl_dx12.h"
#include "imgui_impl_win32.h"
#include <tchar.h>
#include "Core/Application.h"
#include "Core/Input.h"
#include "WinWindow.h"

#ifdef _DEBUG
#define DX12_ENABLE_DEBUG_LAYER
#endif

#ifdef DX12_ENABLE_DEBUG_LAYER
#include <dxgidebug.h>
#pragma comment(lib, "dxguid.lib")
#endif

namespace sb
{
    Direct3dDriver::Direct3dDriver(Window* in_window)
    : Driver(in_window)
    {
    }

    Direct3dDriver::~Direct3dDriver()
    {
    }

    bool Direct3dDriver::BindWinWindow(const HWND in_hwnd)
    {
#define DEVICE_VALID_CHECK(_device)                                                                                    \
    if (!_device)                                                                                                      \
    {                                                                                                                  \
        #_device;                                                                                                      \
        spdlog::error("Invalid {}. Make sure InitD3dDevice called.", #_device);                                        \
        assert(false);                                                                                                 \
        return false;                                                                                                  \
    }

        // Check device
        DEVICE_VALID_CHECK(m_device);
        DEVICE_VALID_CHECK(m_dxgi);
        DEVICE_VALID_CHECK(m_rootSignature);
        DEVICE_VALID_CHECK(m_commandQueue);
        DEVICE_VALID_CHECK(m_DescriptorHeap);

        // SwapChain need hwnd.
        m_swapChain = CreateUPtr<SwapChain>();
        m_swapChain->Init(sg_d3dDevice, m_dxgi, m_commandQueue->GetCmdQueue(), in_hwnd);

        // WndProc에서 swapChain 사용.
        ::ShowWindow(in_hwnd, SW_SHOWDEFAULT);
        ::UpdateWindow(in_hwnd);

        ImGui_ImplWin32_Init(in_hwnd);
        ImGui_ImplDX12_Init(sg_d3dDevice.Get(), NUM_FRAMES_IN_FLIGHT, DXGI_FORMAT_R8G8B8A8_UNORM,
                            m_DescriptorHeap->GetSrvHeap(),
                            m_DescriptorHeap->GetSrvHeap()->GetCPUDescriptorHandleForHeapStart(),
                            m_DescriptorHeap->GetSrvHeap()->GetGPUDescriptorHandleForHeapStart());

        return true;
    }

    void Direct3dDriver::Update()
    {
        // Render imgui graphics
        const float w = 1.f; // TEMP
        Vector3f clearColor{0.45f, 0.55f, 0.60f};
        if (m_ImGuiProperties.size())
        {
            if (auto colorVector = std::get_if<Vector3f>(&m_ImGuiProperties[0].m_property))
            {
                clearColor = {colorVector->X, colorVector->Y, colorVector->Z};
            }
        }
 
        // Render things..
        auto mainRenderTargetDescriptor = m_DescriptorHeap->GetRenderTargetDescriptors();
        auto mainRenderTargetResource = m_swapChain->GetMainRenderTargetResources();
        auto commandList = m_commandQueue->GetCmdList().Get();
        auto commandQueue = m_commandQueue->GetCmdQueue();
        auto swapChain = m_swapChain->GetSwapChain3();
        auto srvDescHeap = m_DescriptorHeap->GetSrvHeap();

        // RenderBegin
        FrameContext* frameCtx = WaitForNextFrameResources();
        uint32 backBufferIdx = m_swapChain->GetSwapChain3()->GetCurrentBackBufferIndex();
        frameCtx->CommandAllocator->Reset();

        D3D12_RESOURCE_BARRIER barrier = {};
        barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
        barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
        barrier.Transition.pResource = mainRenderTargetResource[backBufferIdx];
        barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
        barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
        barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
        commandList->Reset(frameCtx->CommandAllocator, nullptr);
        commandList->ResourceBarrier(1, &barrier);

        m_constantBuffer->Clear();
        m_DescriptorHeap->Clear();

        // CBV
        commandList->SetGraphicsRootSignature(m_rootSignature->GetSignature().Get());
        ID3D12DescriptorHeap* cbvHeap = m_DescriptorHeap->GetCbvHeap().Get();
        ID3D12DescriptorHeap* descriptorHeaps[] = {srvDescHeap, cbvHeap};

        const float clear_color_with_alpha[4] = {clearColor.X * w, clearColor.Y * w, clearColor.Z * w, w};
        commandList->ClearRenderTargetView(mainRenderTargetDescriptor[backBufferIdx], clear_color_with_alpha, 0,
                                           nullptr);
        commandList->OMSetRenderTargets(1, &mainRenderTargetDescriptor[backBufferIdx], FALSE, nullptr);
        commandList->SetDescriptorHeaps(1, &srvDescHeap);
        //commandList->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);


        ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), commandList);
        
        // RenderEnd
        barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
        barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
        commandList->ResourceBarrier(1, &barrier);
        commandList->Close();

        commandQueue->ExecuteCommandLists(1, (ID3D12CommandList* const*)&commandList);

        swapChain->Present(1, 0); // 1이면 vsync

        uint64 fenceValue = m_fenceLastSignaledValue + 1;
        commandQueue->Signal(m_fence.Get(), fenceValue);
        m_fenceLastSignaledValue = fenceValue;
        frameCtx->FenceValue = fenceValue;

        ImGui::UpdatePlatformWindows();

        m_ImGuiProperties.clear();
    }

    void Direct3dDriver::OnUpdate(float in_delta)
    {
    }

    void Direct3dDriver::SwapBuffers()
    {
    }

    bool Direct3dDriver::IsWindowShouldClosed()
    {
        if (bShutDownCalled)
        {
            const HWND hwnd = GetTargetWindow()->m_hwnd;
            const WNDCLASSEXW wc = GetTargetWindow()->m_wc;

            WaitForLastSubmittedFrame();

            ImGui_ImplDX12_Shutdown();
            ImGui_ImplWin32_Shutdown();
            ImGui::DestroyContext();

            CleanUpDevice();
            ::DestroyWindow(hwnd);
            ::UnregisterClassW(wc.lpszClassName, wc.hInstance);
            return true;
        }

        return false;
    }

    void Direct3dDriver::CleanDriver()
    {
        WaitForLastSubmittedFrame();

        ImGui_ImplDX12_Shutdown();
        ImGui_ImplWin32_Shutdown();

        CleanUpDevice();
    }

    void Direct3dDriver::InitD3dDevice()
    {
#ifdef _DEBUG
        ::D3D12GetDebugInterface(IID_PPV_ARGS(&m_debugController));
        m_debugController->EnableDebugLayer();

#endif
        ::CreateDXGIFactory1(IID_PPV_ARGS(&m_dxgi));
        ::D3D12CreateDevice(nullptr, D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&m_device));

#ifdef _DEBUG
        ID3D12InfoQueue* pInfoQueue = nullptr;
        m_device->QueryInterface(IID_PPV_ARGS(&pInfoQueue));
        pInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, true);
        pInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, true);
        pInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, true);
        pInfoQueue->Release();
        m_debugController.Reset();
#endif

        m_rootSignature = CreateUPtr<RootSignature>();
        m_commandQueue = CreateUPtr<CommandQueue>();
        m_DescriptorHeap = CreateUPtr<TableDescriptorHeap>();
        m_constantBuffer = CreateUPtr<ConstantBuffer>();

        m_commandQueue->Init(sg_d3dDevice);
        m_DescriptorHeap->Init(256);
        m_rootSignature->Init(sg_d3dDevice);
        m_constantBuffer->Init(sizeof(DirectX::XMFLOAT4), 256);

        HRESULT hr = m_device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_fence));
        if (FAILED(hr))
        {
            return;
        }

        m_fenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
        if (m_fenceEvent == nullptr)
        {
            return;
        }
    }

    void Direct3dDriver::EnqueueImGuiProperty(ImGuiPropertyPlaceHolder in_property)
    {
        m_ImGuiProperties.emplace_back(std::move(in_property));
    }

    void Direct3dDriver::CleanUpDevice()
    {
        CleanUpRenderTarget();
        if (m_swapChain->GetSwapChain3())
        {
            m_swapChain->Clear();
            m_swapChain.reset();
        }

        if (m_hSwapChainWaitableObject != nullptr)
        {
            CloseHandle(m_hSwapChainWaitableObject);
        }

        for (int i = 0; i < NUM_FRAMES_IN_FLIGHT; i++)
        {
            if (m_frameContexts[i].CommandAllocator)
            {
                m_frameContexts[i].CommandAllocator->Release();
                m_frameContexts[i].CommandAllocator = nullptr;
            }
        }

        if (ComPtr<ID3D12CommandQueue> cmdQueue = m_commandQueue->GetCmdQueue())
        {
            cmdQueue.Reset();
        }

        if (ComPtr<ID3D12GraphicsCommandList> cmdList = m_commandQueue->GetCmdList())
        {
            cmdList.Reset();
        }

        if (ID3D12DescriptorHeap* rtvHeap = m_DescriptorHeap->GetRtvHeap())
        {
            rtvHeap->Release();
            rtvHeap = nullptr;
        }

        if (ID3D12DescriptorHeap* srvHeap = m_DescriptorHeap->GetSrvHeap())
        {
            srvHeap->Release();
            srvHeap = nullptr;
        }

        if (m_fence)
        {
            m_fence.Reset();
        }

        if (m_fenceEvent)
        {
            CloseHandle(m_fenceEvent);
            m_fenceEvent = nullptr;
        }

        if (m_device)
        {
            m_device.Reset();
        }

        m_rootSignature.reset();
        m_DescriptorHeap.reset();
        m_swapChain.reset();
        m_commandQueue.reset();
        m_constantBuffer.reset();

#ifdef _DEBUG
        // 이거 안되는데 나중에 한 번 다시 보기
        IDXGIDebug1* pDebug = nullptr;
        if (SUCCEEDED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(&pDebug))))
        {
            pDebug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_SUMMARY);
            pDebug->Release();
        }
#endif
    }

    WinsWindow* Direct3dDriver::GetTargetWindow() const
    {
        return static_cast<WinsWindow*>(&(sb::Application::Get().GetDirectXWindow()));
    }

    FrameContext* Direct3dDriver::WaitForNextFrameResources()
    {
        uint32 nextFrameIndex = m_frameIndex + 1;
        m_frameIndex = nextFrameIndex;

        HANDLE waitableObjects[] = {sg_d3dDriver->GetSwapChainWaitableObject(), nullptr};
        DWORD numWaitableObjects = 1;

        FrameContext* frameCtx = &m_frameContexts[nextFrameIndex % NUM_FRAMES_IN_FLIGHT];
        uint64 fenceValue = frameCtx->FenceValue;
        if (fenceValue != 0) // Means no fence was singaled
        {
            frameCtx->FenceValue = 0;
            m_fence->SetEventOnCompletion(fenceValue, m_fenceEvent);
            waitableObjects[1] = m_fenceEvent;
            numWaitableObjects = 2;
        }

        WaitForMultipleObjects(numWaitableObjects, waitableObjects, TRUE, INFINITE);

        return frameCtx;
    }

    void Direct3dDriver::WaitForLastSubmittedFrame()
    {
        FrameContext* frameCtx = &m_frameContexts[m_frameIndex % NUM_FRAMES_IN_FLIGHT];

        uint64 fenceValue = frameCtx->FenceValue;
        if (fenceValue == 0)
        {
            return; // No fence was signaled
        }

        frameCtx->FenceValue = 0;
        auto fence = sg_d3dDriver->GetFence();
        if (fence->GetCompletedValue() >= fenceValue)
        {
            return;
        }

        fence->SetEventOnCompletion(fenceValue, sg_d3dDriver->GetFenceEvent());
        WaitForSingleObject(sg_d3dDriver->GetFenceEvent(), INFINITE);
    }

    void Direct3dDriver::CreateRenderTarget()
    {
        for (int i = 0; i < SWAP_CHAIN_BUFFER_COUNT; i++)
        {
            ID3D12Resource* pBackBuffer = nullptr;
            m_swapChain->GetSwapChain3()->GetBuffer(i, IID_PPV_ARGS(&pBackBuffer));
            m_device->CreateRenderTargetView(pBackBuffer, nullptr,
                                                          m_DescriptorHeap->GetRenderTargetDescriptors()[i]);
            m_swapChain->GetMainRenderTargetResources()[i] = pBackBuffer;
        }
    }

    void Direct3dDriver::CleanUpRenderTarget()
    {
        WaitForLastSubmittedFrame();

        for (int i = 0; i < SWAP_CHAIN_BUFFER_COUNT; i++)
        {
            if (m_swapChain->GetMainRenderTargetResources()[i])
            {
                m_swapChain->GetMainRenderTargetResources()[i]->Release();
                m_swapChain->GetMainRenderTargetResources()[i] = nullptr;
            }
        }
    }
} // namespace sb