#include "Direct3dDriver.h"

#include "Core/Application.h"
#include "Core/Input.h"
#include "Render/DirectX12/ShaderResource.h"
#include "Render/ShaderGeometry.h"
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

    Direct3dDriver::Direct3dDriver()
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
        DEVICE_VALID_CHECK(_commandQueue);
        DEVICE_VALID_CHECK(_commandList);

        CreateSwapChain(in_hwnd);
        CreateRtvDescriptorHeap();
        CreateRenderTarget();
        CreateDepthStencil();

        // WndProc에서 swapChain 사용.
        ::ShowWindow(in_hwnd, SW_SHOWDEFAULT);
        ::UpdateWindow(in_hwnd);

        ImGui_ImplWin32_Init(in_hwnd);
        ImGui_ImplDX12_Init(sg_d3dDevice.Get(), NUM_FRAMES_IN_FLIGHT, DXGI_FORMAT_R8G8B8A8_UNORM, _srvHeap.Get(),
                            _srvHeap.Get()->GetCPUDescriptorHandleForHeapStart(),
                            _srvHeap.Get()->GetGPUDescriptorHandleForHeapStart());

        return true;
    }

    bool Direct3dDriver::InitializeResources()
    {
        // 이거 이전에 다른대서 설정되어야 함
        _vpWidth = 800;
        _vpHeight = 600;

        ShaderHeapInstruction instruction;
        ShaderResourceDesc data;
        
        // Process raw data.
        if (1)
        {
            ShaderGeometryRawData rawData;
            rawData._vertex.assign({Vertex({-0.5f, 0.5f, 0.5f}, {1.0f, 0.0f, 0.0f, 1.0f}),
                                    Vertex({0.5f, -0.5f, 0.5f}, {0.0f, 1.0f, 0.0f, 1.0f}),
                                    Vertex({-0.5f, -0.5f, 0.5f}, {0.0f, 0.0f, 1.0f, 1.0f}),
                                    Vertex({0.5f, 0.5f, 0.5f}, {1.0f, 0.0f, 1.0f, 1.0f})});
            rawData._index.assign({0, 1, 2, 0, 3, 1});

            UPtr<DxShaderGeometryFactory> factory = CreateUPtr<DxShaderGeometryFactory>();
            UPtr<ShaderGeometry> geo = factory->CreateGeometry(rawData);
            _shaderGeoData.insert({"sample1", std::move(geo)});

            data._shaderKey = "sample1";
            data._files._vsPath = projectPath + "/resources/shader/hlsl/sample1.vs.hlsl";
            data._files._fsPath = projectPath + "/resources/shader/hlsl/sample1.fs.hlsl";
        }

        if (0)
        {
            ShaderGeometryRawData rawData;
            rawData._vertex.assign({Vertex({-0.5f, 0.5f, 0.5f}, {0.0f, 0.0f, 1.0f, 1.0f}),
                                    Vertex({0.5f, -0.5f, 0.5f}, {0.0f, 0.0f, 1.0f, 1.0f}),
                                    Vertex({-0.5f, -0.5f, 0.5f}, {0.0f, 0.0f, 1.0f, 1.0f}),
                                    Vertex({0.5f, 0.5f, 0.5f}, {0.0f, 0.0f, 1.0f, 1.0f}),

                                    Vertex({-0.75f, 0.75f, 0.7f}, {1.0f, 0.0f, 0.0f, 1.0f}),
                                    Vertex({0.0f, 0.f, 0.7f}, {1.0f, 0.0f, 0.0f, 1.0f}),
                                    Vertex({-0.75f, 0.f, 0.7f}, {1.0f, 0.0f, 0.0f, 1.0f}),
                                    Vertex({0.f, 0.75f, 0.7f}, {1.0f, 0.0f, 0.0f, 1.0f})});
            rawData._index.assign({0, 1, 2, 0, 3, 1});

            UPtr<DxShaderGeometryFactory> factory = CreateUPtr<DxShaderGeometryFactory>();
            UPtr<ShaderGeometry> geo = factory->CreateGeometry(rawData);
            _shaderGeoData.insert({"sample2", std::move(geo)});
        }

        if (0)
        {
            ShaderGeometryRawData rawData;
            rawData._vertex.assign({Vertex({-0.5f, 0.5f, 0.5f}, {1.0f, 0.0f, 0.0f, 1.0f}),
                                    Vertex({0.5f, -0.5f, 0.5f}, {1.0f, 0.0f, 1.0f, 1.0f}),
                                    Vertex({-0.5f, -0.5f, 0.5f}, {0.0f, 0.0f, 1.0f, 1.0f}),
                                    Vertex({0.5f, 0.5f, 0.5f}, {0.0f, 1.0f, 1.0f, 1.0f})});

            rawData._index.assign({0, 1, 2, 0, 3, 1});

            UPtr<DxShaderGeometryFactory> factory = CreateUPtr<DxShaderGeometryFactory>();
            UPtr<ShaderGeometry> geo = factory->CreateGeometry(rawData);
            _shaderGeoData.insert({"sample3", std::move(geo)});
            
            data._shaderKey = "sample3";
            data._files._vsPath = projectPath + "/resources/shader/hlsl/sample.vs.hlsl";
            data._files._fsPath = projectPath + "/resources/shader/hlsl/sample.fs.hlsl";
            instruction._bTable = true;
            instruction._numDescriptors = 1;
            instruction._rangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
        }

        

        _shaderResource = new ShaderResource(data, instruction);

        HRESULT hr = _commandQueue->Signal(m_fence.Get(), m_fenceLastSignaledValue);
        if (FAILED(hr))
        {
            return false;
        }

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

        return true;
    }

    void Direct3dDriver::Update()
    {
        RenderBegin();
        Render();
        RenderEnd();
    }

    void Direct3dDriver::OnUpdate(float in_delta)
    {
    }

    void Direct3dDriver::Render()
    {
        double currentTime = GetSystemTime();
        const double tick = (currentTime - _PrevTime) * 0.001;
        _PrevTime = currentTime;

        // Tick shaderResource
        _shaderResource->Tick(tick);

        CD3DX12_CPU_DESCRIPTOR_HANDLE dsvHandle(_dsHeap->GetCPUDescriptorHandleForHeapStart());
        _commandList->OMSetRenderTargets(1, &_mainRtvCpuHandle[_backBufferIndex], FALSE, &dsvHandle);

        // render stuff..
        _shaderResource->Render(_commandList);

        // render imgui
        RenderImgui();
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

        // Create CommandList and CommandQueue.
        {
            D3D12_COMMAND_QUEUE_DESC queueDesc = {};
            queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
            queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
            queueDesc.NodeMask = 1;

            // 분석 필
            m_device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(_commandQueue.GetAddressOf()));

            for (int32 i = 0; i < NUM_FRAMES_IN_FLIGHT; i++)
            {
                m_device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT,
                                                 IID_PPV_ARGS(&sg_d3dDriver->GetFrameContexts()[i].CommandAllocator));
            }

            m_device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT,
                                        sg_d3dDriver->GetFrameContexts()[0].CommandAllocator, nullptr,
                                        IID_PPV_ARGS(_commandList.GetAddressOf()));
        }

        // SRV
        {
            D3D12_DESCRIPTOR_HEAP_DESC desc = {};
            desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
            desc.NumDescriptors = 1;
            desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
            if (m_device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(_srvHeap.GetAddressOf())) != S_OK)
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
        HRESULT hr = m_dxgi->CreateSwapChainForHwnd(_commandQueue.Get(), in_hwnd, &sd, nullptr, nullptr, &_swapChain1);

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

        if (_dsBuffer)
        {
            _dsBuffer.Reset();
        }

        if (_dsHeap)
        {
            _dsHeap.Reset();
        }

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

        if (_commandList)
        {
            _commandList.Reset();
        }

        if (_commandQueue)
        {
            _commandQueue.Reset();
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

    void Direct3dDriver::RenderBegin()
    {
        FrameContext* frameCtx = WaitForNextFrameResources();
        _backBufferIndex = _swapChain3->GetCurrentBackBufferIndex();

        // gpu에서 처리가 끝난 allocator를 reset 해줘야 함
        // reset으로 메모리 초기화 줌(저장된 commandList)
        // 하나의 commandList만 record 할 수 있기 때문에 다른 list는 close 상태여야 함
        frameCtx->CommandAllocator->Reset();
        // reset을 해좌야 record상태로 전환되고 commandAllocator에 record 할 수 있다.
        _commandList->Reset(frameCtx->CommandAllocator, _shaderResource->GetPipelineState().Get());
        // 이젠 record 시작(아래 command들은 commandAllocator에 저장됨)

        D3D12_RESOURCE_BARRIER barrier = {};
        barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
        barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
        barrier.Transition.pResource = _mainRtvResource[_backBufferIndex];
        barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
        barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
        barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;

        _commandList->ResourceBarrier(1, &barrier);

        const float clear_color_with_alpha[4] = {_clearColor.X * _clearColor.W, _clearColor.Y * _clearColor.W,
                                                 _clearColor.Z * _clearColor.W, _clearColor.W};

        CD3DX12_CPU_DESCRIPTOR_HANDLE dsvHandle(_dsHeap->GetCPUDescriptorHandleForHeapStart());

        // Settings
        {
            ImDrawData* DrawData = ImGui::GetDrawData();
            _viewport.Height = DrawData->DisplaySize.y;
            _viewport.Width = DrawData->DisplaySize.x;
            _scissorRect.bottom = DrawData->DisplaySize.y;
            _scissorRect.right = DrawData->DisplaySize.x;
            _vpHeight = DrawData->DisplaySize.y;
            _vpWidth = DrawData->DisplaySize.x;

            _commandList->RSSetViewports(1, &_viewport);
            _commandList->RSSetScissorRects(1, &_scissorRect);

            _commandList->ClearRenderTargetView(_mainRtvCpuHandle[_backBufferIndex], clear_color_with_alpha, 0, nullptr);
            _commandList->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH, 1.f, 0, 0, nullptr);
        }
    }

    void Direct3dDriver::RenderEnd()
    {
        D3D12_RESOURCE_BARRIER barrier = {};
        barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
        barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
        barrier.Transition.pResource = _mainRtvResource[_backBufferIndex];
        barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
        barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
        barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
        _commandList->ResourceBarrier(1, &barrier);
        _commandList->Close();

        ID3D12CommandList* ppCmdLists[] = {_commandList.Get()};
        _commandQueue->ExecuteCommandLists(_countof(ppCmdLists), ppCmdLists);

        _swapChain3->Present(1, 0); // 1이면 vsync

        // sync
        FrameContext* frameCtx = &m_frameContexts[m_frameIndex];
        uint64 fenceValue = m_fenceLastSignaledValue + 1;
        _commandQueue->Signal(m_fence.Get(), fenceValue);
        m_fenceLastSignaledValue = fenceValue;
        frameCtx->FenceValue = fenceValue;

        ImGui::UpdatePlatformWindows();
    }

    void Direct3dDriver::RenderImgui()
    {
        ID3D12DescriptorHeap* descriptorHeaps[] = {_srvHeap.Get()};
        _commandList->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);

        ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), _commandList.Get());
    }

    void Direct3dDriver::UpdateFenceValue()
    {
        FrameContext& frameCtx = m_frameContexts[m_frameIndex];
        frameCtx.FenceValue = ++m_fenceLastSignaledValue;
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

    ShaderGeometry* Direct3dDriver::GetShaderData(const std::string& key) const
    {
        auto geo =_shaderGeoData.find(key);
        return geo.get_ptr() ? geo->second.get() : nullptr;
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

    void Direct3dDriver::CreateDepthStencil()
    {
        D3D12_DESCRIPTOR_HEAP_DESC desc = {};
        desc.NumDescriptors = 1;
        desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
        desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
        HRESULT hr = sg_d3dDevice->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&_dsHeap));
        if (FAILED(hr))
        {
            spdlog::error("Failed to create depth/stencil descriptor heap.");
        }

        D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
        dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;
        dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
        dsvDesc.Flags = D3D12_DSV_FLAG_NONE;

        D3D12_CLEAR_VALUE clearValue = {};
        clearValue.Format = DXGI_FORMAT_D32_FLOAT;
        clearValue.DepthStencil.Depth = 1.f;
        clearValue.DepthStencil.Stencil = 0.f;

        const uint32 backBufferIndex = _swapChain3->GetCurrentBackBufferIndex();
        const D3D12_RESOURCE_DESC rtvDesc = _mainRtvResource[backBufferIndex]->GetDesc();
        const uint32 rw = rtvDesc.Width;
        const uint32 rh = rtvDesc.Height;

        D3D12_HEAP_PROPERTIES defaultHeapProps = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
        CD3DX12_RESOURCE_DESC resourceDesc = CD3DX12_RESOURCE_DESC::Tex2D(DXGI_FORMAT_D32_FLOAT, rw, rh, 1, 0, 1, 0,
                                                                          D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL);
        sg_d3dDevice->CreateCommittedResource(&defaultHeapProps, D3D12_HEAP_FLAG_NONE, &resourceDesc,
                                              D3D12_RESOURCE_STATE_DEPTH_WRITE, &clearValue, IID_PPV_ARGS(&_dsBuffer));

        hr = sg_d3dDevice->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&_dsHeap));
        if (FAILED(hr))
        {
            spdlog::error("Failed to create depth/stencil descriptor heap after commit.");
        }
        _dsHeap->SetName(L"depth/stencil resource heap");

       sg_d3dDevice->CreateDepthStencilView(_dsBuffer.Get(), &dsvDesc, _dsHeap->GetCPUDescriptorHandleForHeapStart());
    }
} // namespace sb