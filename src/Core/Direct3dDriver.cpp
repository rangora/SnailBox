#include "Direct3dDriver.h"

#include "Core/Application.h"
#include "Core/Input.h"
#include "Render/DirectX12/Direct3dContext.h"
#include "Render/DirectX12/ShaderResource.h"
#include "Render/RenderResource.h"
#include "WinWindow.h"
#include "corepch.h"
#include "imgui.h"
#include "imgui_impl_dx12.h"
#include "imgui_impl_win32.h"
#include <tchar.h>

#ifdef _DEBUG
#define DX12_ENABLE_DEBUG_LAYER
#endif

#ifdef DX12_ENABLE_DEBUG_LAYER
#include <dxgidebug.h>
#pragma comment(lib, "dxguid.lib")
#endif

namespace sb
{
    Direct3dDriver::Direct3dDriver(Window* in_window) : Driver(in_window)
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
        DEVICE_VALID_CHECK(m_commandQueue);

        CreateSwapChain(in_hwnd);
        CreateRtvDescriptorHeap();
        CreateRenderTarget();

        // WndProc에서 swapChain 사용.
        ::ShowWindow(in_hwnd, SW_SHOWDEFAULT);
        ::UpdateWindow(in_hwnd);

        ImGui_ImplWin32_Init(in_hwnd);
        ImGui_ImplDX12_Init(sg_d3dDevice.Get(), NUM_FRAMES_IN_FLIGHT, DXGI_FORMAT_R8G8B8A8_UNORM,
                            _srvHeap.Get(),
                            _srvHeap.Get()->GetCPUDescriptorHandleForHeapStart(),
                            _srvHeap.Get()->GetGPUDescriptorHandleForHeapStart());

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
        auto commandList = m_commandQueue->GetCmdList().Get();
        auto commandQueue = m_commandQueue->GetCmdQueue();

        // RenderBegin
        FrameContext* frameCtx = WaitForNextFrameResources();
        // FrameContext* frameCtx = WaitForPreviousFrame();
        uint32 backBufferIdx = _swapChain3->GetCurrentBackBufferIndex();

        // gpu에서 처리가 끝난 allocator를 reset 해줘야 함
        // reset으로 메모리 초기화 줌(저장된 commandList)
        // 하나의 commandList만 record 할 수 있기 때문에 다른 list는 close 상태여야 함
        frameCtx->CommandAllocator->Reset();

        D3D12_RESOURCE_BARRIER barrier = {};
        barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
        barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
        barrier.Transition.pResource = _mainRtvResource[backBufferIdx];
        barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
        barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
        barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;

        // reset을 해좌야 record상태로 전환되고 commandAllocator에 record 할 수 있다.
        commandList->Reset(frameCtx->CommandAllocator, _shaderResource->GetPipelineState().Get());
        // 이젠 record 시작(아래 command들은 commandAllocator에 저장됨)

        commandList->ResourceBarrier(1, &barrier);

        /*    m_constantBuffer->Clear();
            m_DescriptorHeap->Clear();*/

        // CBV
        ID3D12DescriptorHeap* descriptorHeaps[] = {_srvHeap.Get()};

        const float clear_color_with_alpha[4] = {clearColor.X * w, clearColor.Y * w, clearColor.Z * w, w};
        commandList->ClearRenderTargetView(_mainRtvCpuHandle[backBufferIdx], clear_color_with_alpha, 0,
                                           nullptr);
         commandList->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);

        ImDrawData* DrawData = ImGui::GetDrawData();
        _viewport.Height = DrawData->DisplaySize.y;
        _viewport.Width = DrawData->DisplaySize.x;
        _scissorRect.bottom = DrawData->DisplaySize.y;
        _scissorRect.right = DrawData->DisplaySize.x;

        // 여기 또는
        commandList->SetGraphicsRootSignature(_shaderResource->GetRootSignature().Get());
        commandList->RSSetViewports(1, &_viewport);
        commandList->RSSetScissorRects(1, &_scissorRect);
        commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        commandList->IASetVertexBuffers(0, 1, _shaderResource->GetVertexBufferView());
        commandList->DrawInstanced(3, 1, 0, 0);

        commandList->OMSetRenderTargets(1, &_mainRtvCpuHandle[backBufferIdx], FALSE, nullptr);

        ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), commandList);

        // 여기

        // RenderEnd
        barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
        barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
        commandList->ResourceBarrier(1, &barrier);
        commandList->Close();

        commandQueue->ExecuteCommandLists(1, (ID3D12CommandList* const*)&commandList);

        _swapChain3->Present(1, 0); // 1이면 vsync

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

    void Direct3dDriver::InitRenderInfo()
    {
        _shaderResource = new ShaderResource;
        _shaderResource->Init();

        auto cmdList = m_commandQueue->GetCmdList();
        UpdateSubresources(cmdList.Get(), _shaderResource->GetVertexBuffer().Get(),
                           _shaderResource->GetVertexBufferUploadHeap().Get(), 0, 0, 1,
                           _shaderResource->GetVertexBufferData());

        // resource 만들 때 defaultHeap을 COPY_DEST로 사용했다.
        // 그리고 VertexBuffer 값을 복사해 넣었다.
        // 이젠 VertexBufferState로 바꿔 사용할 것이다.
        ID3D12Resource* vResource = _shaderResource->GetVertexBuffer().Get();
        D3D12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
            vResource, D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);
        cmdList->ResourceBarrier(1, &barrier);

        cmdList->Close();
        auto cmdQueue = m_commandQueue->GetCmdQueue();
        ID3D12CommandList* ppCmdLists[] = {cmdList.Get()};
        cmdQueue->ExecuteCommandLists(_countof(ppCmdLists), ppCmdLists);

        uint64 fenceValue = m_fenceLastSignaledValue + 1;
        auto& frameCtx = m_frameContexts[m_frameIndex];
        frameCtx.FenceValue = fenceValue;
        m_fenceLastSignaledValue = fenceValue;

        HRESULT hr = cmdQueue->Signal(m_fence.Get(), fenceValue);
        if (FAILED(hr))
        {
            return;
        }

        _shaderResource->CreateVertexBufferView();

        // Fill out the Viewport
        _viewport.TopLeftX = 0;
        _viewport.TopLeftY = 0;
        _viewport.Width = 800;
        _viewport.Height = 600;
        _viewport.MinDepth = 0.0f;
        _viewport.MaxDepth = 1.0f;

        // Fill out a scissor rect
        _scissorRect.left = 0;
        _scissorRect.top = 0;
        _scissorRect.right = 800;
        _scissorRect.bottom = 600;
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

        m_commandQueue = CreateUPtr<CommandQueue>();
        m_commandQueue->Init(sg_d3dDevice);

        // SRV
        {
            D3D12_DESCRIPTOR_HEAP_DESC desc = {};
            desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
            desc.NumDescriptors = 1;
            desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
            if (m_device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&_srvHeap)) != S_OK)
            {
                return;
            }
        }

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

        // TEMP
        InitRenderInfo();

        // m_commandQueue->GetCmdList()->Close();
    }

    void Direct3dDriver::EnqueueImGuiProperty(ImGuiPropertyPlaceHolder in_property)
    {
        m_ImGuiProperties.emplace_back(std::move(in_property));
    }

    void Direct3dDriver::CreateSwapChain(const HWND in_hwnd)
    {
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

        IDXGISwapChain1* _swapChain1 = nullptr;
        HRESULT hr = m_dxgi->CreateSwapChainForHwnd(m_commandQueue->GetCmdQueue().Get(), in_hwnd, &sd, nullptr, nullptr,
                                       &_swapChain1);

        if (FAILED(hr))
        {
            return;
        }

        _swapChain1->QueryInterface(IID_PPV_ARGS(&_swapChain3));
        _swapChain1->Release();
        _swapChain3->SetMaximumFrameLatency(SWAP_CHAIN_BUFFER_COUNT);
        SetSwapChainWaitableObject(_swapChain3->GetFrameLatencyWaitableObject()); // ?
    }

    void Direct3dDriver::CreateRtvDescriptorHeap()
    {
        D3D12_DESCRIPTOR_HEAP_DESC desc = {};
        desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
        desc.NumDescriptors = SWAP_CHAIN_BUFFER_COUNT; // num of backBuffers
        desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
        desc.NodeMask = 1;

        if (sg_d3dDevice->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&_mainRtvHeap)) != S_OK)
        {
            return;
        }

        uint32 rtvHandleIncrementSize = sg_d3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
        D3D12_CPU_DESCRIPTOR_HANDLE cpuRtvHandleBegin = _mainRtvHeap->GetCPUDescriptorHandleForHeapStart();
        D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = cpuRtvHandleBegin;
        for (int32 i = 0; i < SWAP_CHAIN_BUFFER_COUNT; i++)
        {
            _mainRtvCpuHandle[i] = rtvHandle;
            rtvHandle.ptr += rtvHandleIncrementSize;
        }
    }

    void Direct3dDriver::CleanUpDevice()
    {
        CleanUpRenderTarget();
        if (_swapChain3)
        {
            _swapChain3.Reset();
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

        m_commandQueue.reset();

#ifdef _DEBUG
        // 이거 안되는데 나중에 한 번 다시 보기
        /* IDXGIDebug1* pDebug = nullptr;
         if (SUCCEEDED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(&pDebug))))
         {
             pDebug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_SUMMARY);
             pDebug->Release();
         }*/
#endif
    }

    WinsWindow* Direct3dDriver::GetTargetWindow() const
    {
        return static_cast<WinsWindow*>(&(sb::Application::Get().GetDirectXWindow()));
    }

    FrameContext* Direct3dDriver::WaitForNextFrameResources()
    {
        m_frameIndex = m_frameIndex++;

        HANDLE waitableObjects[] = {sg_d3dDriver->GetSwapChainWaitableObject(), nullptr};
        DWORD numWaitableObjects = 1;

        FrameContext* frameCtx = &m_frameContexts[m_frameIndex % NUM_FRAMES_IN_FLIGHT];
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

    FrameContext* Direct3dDriver::WaitForPreviousFrame()
    {
        FrameContext* frameCtx = &m_frameContexts[m_frameIndex % NUM_FRAMES_IN_FLIGHT];

        if (m_fence->GetCompletedValue() < frameCtx->FenceValue)
        {
            m_fence->SetEventOnCompletion(frameCtx->FenceValue, m_fenceEvent);
            WaitForSingleObject(m_fenceEvent, INFINITE);
        }

        m_frameIndex++;

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
            _swapChain3->GetBuffer(i, IID_PPV_ARGS(&pBackBuffer));
            m_device->CreateRenderTargetView(pBackBuffer, nullptr, _mainRtvCpuHandle[i]);
            _mainRtvResource[i] = pBackBuffer;
        }
    }

    void Direct3dDriver::CleanUpRenderTarget()
    {
        WaitForLastSubmittedFrame();

        for (int i = 0; i < SWAP_CHAIN_BUFFER_COUNT; i++)
        {
            if (_mainRtvResource[i])
            {
                _mainRtvResource[i]->Release();
                _mainRtvResource[i] = nullptr;
            }
        }
    }
} // namespace sb