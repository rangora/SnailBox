#include "Direct3dDriver.h"

#include "Core/Application.h"
#include "Core/Input.h"
#include "Render/DirectX12/ShaderResource.h"
#include "Render/RenderResourceStorage.h"
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

        _vpWidth = 800;
        _vpHeight = 600;

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

    void Direct3dDriver::Update()
    {
        UpdateTick();
        UpdateViewMatrix();

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
        _tick = (currentTime - _PrevTime) * 0.001;
        _PrevTime = currentTime;

        // Tick shaderResource
        _shaderResource->Tick(_tick);

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

    bool Direct3dDriver::InitializeResources()
    {
        RenderResourceStorage& handler = RenderResourceStorage::Get();

       for (const auto& resource : handler._customData)
        {
           if (!resource.GetName()._Equal("sample4"))
           {
               continue;
           }

            UPtr<DxShaderGeometryFactory> factory = CreateUPtr<DxShaderGeometryFactory>();
            UPtr<ShaderGeometry> geo = factory->CreateGeometry(resource._RawData);
            _shaderGeoData.insert({resource.GetName(), std::move(geo)});

            ShaderHeapInstruction instruction;
            ShaderResourceDesc data;

            data._shaderKey = resource.GetName();
            data._files._vsPath = resource._vsPath;
            data._files._fsPath = resource._psPath;

            if (data._shaderKey._Equal("sample3"))
            {
                instruction._bTable = true;
                instruction._numDescriptors = 1;
                instruction._rangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
            }
            else if (data._shaderKey._Equal("sample4"))
            {
                instruction._rootSignType = RootSignatureType::Descriptor;
            }

            _shaderData.insert({data._shaderKey, CreateUPtr<ShaderResource>(ShaderResource(data, instruction))});
        }

        _shaderResource = _shaderData.find("sample4")->second.get(); // temp

        _commandList->Close();

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

        // camera init [TEMP]
        // build projection and view matrix

        // New Camera
        _camera.SetPosition(0.f, 5.f, -4.f);
        _camera.SetProjection(90.f, (float)_vpWidth / (float)_vpHeight, 0.1f, 1000.f);

        XMMATRIX tmpMat = XMMatrixPerspectiveFovLH(45.0f * (3.14f / 180.0f), (float)_vpWidth / (float)_vpHeight, 0.1f, 1000.0f);
        XMStoreFloat4x4(&_cameraProjMat, tmpMat);

        XMFLOAT3 a, b;
        _cameraPosition = XMVectorSet(0.0f, 5.0f, -4.0f, 0.0f);
        _cameraTarget = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
        _cameraUp = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
        _cameraForward = XMVectorSubtract(_cameraTarget, _cameraPosition);
        _cameraForward = XMVector3Normalize(_cameraForward);

        // build view matrix
        tmpMat = XMMatrixLookAtLH(_cameraPosition, _cameraTarget, _cameraUp);
        XMStoreFloat4x4(&_cameraViewMat, tmpMat);

        // set starting cubes position
        // first cube
        _cube1Position = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f); // set cube 1's position
        XMVECTOR posVec = XMLoadFloat4(&_cube1Position);   // create xmvector for cube1's position

        tmpMat = XMMatrixTranslationFromVector(posVec);    // create translation matrix from cube1's position vector
        XMStoreFloat4x4(&_cube1RotMat, XMMatrixIdentity()); // initialize cube1's rotation matrix to identity matrix
        XMStoreFloat4x4(&_cube1WorldMat, tmpMat);           // store cube1's world matrix

        return true;
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

        D3D12_MESSAGE_ID hide[] = {
            D3D12_MESSAGE_ID_CREATERESOURCE_STATE_IGNORED,
            // D3D12_MESSAGE_ID_MAP_INVALID_NULLRANGE,
            // D3D12_MESSAGE_ID_UNMAP_INVALID_NULLRANGE,
            //// Workarounds for debug layer issues on hybrid-graphics systems
            // D3D12_MESSAGE_ID_EXECUTECOMMANDLISTS_WRONGSWAPCHAINBUFFERREFERENCE,
            // D3D12_MESSAGE_ID_RESOURCE_BARRIER_MISMATCHING_COMMAND_LIST_TYPE,
        };
        D3D12_INFO_QUEUE_FILTER filter = {};
        filter.DenyList.NumIDs = static_cast<UINT>(std::size(hide));
        filter.DenyList.pIDList = hide;

        pInfoQueue->AddStorageFilterEntries(&filter);
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

    void Direct3dDriver::UpdateViewMatrix()
    {

        //XMVECTOR cPos = XMLoadFloat4(&_cameraPosition);
        //XMVECTOR cTarg = XMLoadFloat4(&_cameraTarget);
        //XMVECTOR cUp = XMLoadFloat4(&_cameraUp);
        //XMMATRIX tmpMat = XMMatrixLookAtLH(cPos, cTarg, cUp);
        //XMStoreFloat4x4(&_cameraViewMat, tmpMat);

        //_cube1Position = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
        //XMVECTOR posVec = XMLoadFloat4(&_cube1Position);

        //tmpMat = XMMatrixTranslationFromVector(posVec);
       /* XMStoreFloat4x4(&_cube1RotMat, XMMatrixIdentity());
        XMStoreFloat4x4(&_cube1WorldMat, tmpMat);*/
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
        return static_cast<WinsWindow*>(&(sb::Application::Get().GetWinWindow()));
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

    void Direct3dDriver::UpdateTick()
    {
        bool bDown = Input::IsMouseButtonDown(MouseButton::Right);
        if (bDown)
        {
            //_cameraPosition.x += 0.1f;
            //_cameraTarget.x += 0.1f;

            Vector2f& mouseDiff = GetTargetWindow()->_mouseDiff;
            float dx = mouseDiff.X * 0.0001f;
            float dy = mouseDiff.Y * 0.000f;

            if (mouseDiff.X > 0) // move right
            {

            }
            else if (mouseDiff.X < 0) // move left
            {

            }

            if (mouseDiff.Y > 0) // mouse down
            {

            }
            else if (mouseDiff.Y < 0) // mouse up
            {

            }

            float yawAngle = dx;
            float pitchAngle = dy;

            // yaw
            XMVECTOR yawAxis = _cameraUp;
            XMVECTOR yawRotation = XMQuaternionRotationAxis(yawAxis, yawAngle);

            // pitch
            XMVECTOR rightVector = XMVector3Normalize(XMVector3Cross(_cameraUp, _cameraForward));
            XMVECTOR pitchRotation = XMQuaternionRotationAxis(rightVector, pitchAngle);

            // combine
            XMVECTOR combinedRotation = XMQuaternionMultiply(pitchRotation, yawRotation);

            _cameraForward = XMVector3Rotate(_cameraForward, combinedRotation);
            _cameraForward = XMVector3Normalize(_cameraForward);

            _cameraUp = XMVector3Normalize(XMVector3Cross(rightVector, _cameraForward));

            XMVECTOR focusPosition = XMVectorAdd(_cameraPosition, _cameraForward);

            // view matrix
            _cameraTarget = XMVectorAdd(_cameraPosition, _cameraForward);
            XMMATRIX tmpMat = XMMatrixLookAtLH(_cameraPosition, focusPosition, _cameraUp);
            XMStoreFloat4x4(&_cameraViewMat, tmpMat);

            //_cube1Position = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f); // set cube 1's position
            //XMVECTOR posVec = XMLoadFloat4(&_cube1Position);   // create xmvector for cube1's position

            //tmpMat = XMMatrixTranslationFromVector(posVec); // create translation matrix from cube1's position vector
            //XMStoreFloat4x4(&_cube1RotMat, XMMatrixIdentity()); // initialize cube1's rotation matrix to identity matrix
            //XMStoreFloat4x4(&_cube1WorldMat, tmpMat);           // store cube1's world matrix

           /* XMVECTOR cameraPosVector = XMLoadFloat4(&_cameraPosition);
             XMVECTOR cameraTargetVector = XMLoadFloat4(&_cameraTarget);
             XMVECTOR cameraForwardVector = XMVectorSubtract(cameraTargetVector, cameraPosVector);
             XMStoreFloat4(&_cameraForward, cameraForwardVector);

             XMVECTOR forward = XMVector3Normalize(cameraForwardVector);
             float deltaAngle = 2.f;

             deltaAngle = XMConvertToRadians(deltaAngle);

             XMFLOAT3 Axis(0.f, 0.f, 1.f);
             XMVECTOR rotationAxis = XMLoadFloat3(&Axis);
             XMVECTOR rotationQuat = XMQuaternionRotationAxis(rotationAxis, deltaAngle);
             XMVECTOR rotatedDirection = XMVector3Rotate(forward, rotationQuat);
             XMVECTOR newFocusPos = XMVectorAdd(cameraPosVector, rotatedDirection);

             XMStoreFloat4(&_cameraTarget, newFocusPos);

             XMVECTOR cPos = XMLoadFloat4(&_cameraPosition);
             XMVECTOR cTarg = XMLoadFloat4(&_cameraTarget);
             XMVECTOR cUp = XMLoadFloat4(&_cameraUp);
             XMMATRIX tmpMat = XMMatrixLookAtLH(cPos, cTarg, cUp);*/
            //XMStoreFloat4x4(&_cameraViewMat, tmpMat);

            spdlog::info("right button down..");
        }
    }

    void Direct3dDriver::UpdateViewMatrix2()
    {

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