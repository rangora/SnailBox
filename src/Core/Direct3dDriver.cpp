#include "Direct3dDriver.h"

#include "imgui.h"
#include "imgui_impl_dx12.h"
#include "imgui_impl_win32.h"
#include <d3d12.h>
#include <dxgi1_4.h>
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

    void* Direct3dDriver::GetNativeWindow()
    {
        return nullptr;
    }

    bool Direct3dDriver::InitDriver(const WinWindowData* in_windowData)
    {
        const HWND hwnd = GetWinWindow()->m_hwnd;

        m_rootSignature = CreateUPtr<RootSignature>();
        m_swapChain = CreateUPtr<SwapChain>();
        m_commandQueue = CreateUPtr<CommandQueue>();
        m_DescriptorHeap = CreateUPtr<TableDescriptorHeap>();

        m_commandQueue->Init(sg_d3dDevice, m_swapChain.get());
        m_DescriptorHeap->Init(256);
        m_swapChain->Init(sg_d3dDevice, m_dxgi, m_commandQueue->GetCmdQueue(), hwnd);
        m_rootSignature->Init(sg_d3dDevice);

        // fence
        HRESULT hr = m_device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_fence));
        if (FAILED(hr))
        {
            return false;
        }

        m_fenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
        if (m_fenceEvent == nullptr)
        {
            return false;
        }
        // ~fence

        ::ShowWindow(hwnd, SW_SHOWDEFAULT);
        ::UpdateWindow(hwnd);

        // Setup imGuiContext
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
        io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

        ImGui::StyleColorsDark();

        ImGui_ImplWin32_Init(hwnd);
        ImGui_ImplDX12_Init(sg_d3dDevice.Get(), NUM_FRAMES_IN_FLIGHT, DXGI_FORMAT_R8G8B8A8_UNORM,
                            m_DescriptorHeap->GetSrvHeap(),
                            m_DescriptorHeap->GetSrvHeap()->GetCPUDescriptorHandleForHeapStart(),
                            m_DescriptorHeap->GetSrvHeap()->GetGPUDescriptorHandleForHeapStart());

        return true;
    }

    void Direct3dDriver::Update()
    {
        auto pressedAction = [this]()
        {
            bShutDownCalled = true;
        };
        auto heldAction = []() {};
        auto releasedAction = []() {};

        Input::KeyInputAction(pressedAction, KeyState::Pressed, KeyButton::Esc);
        Input::KeyInputAction(heldAction, KeyState::Held, KeyButton::Esc);
        Input::KeyInputAction(releasedAction, KeyState::Released, KeyButton::Esc);


        // Poll handle messages
        MSG msg;
        while (::PeekMessage(&msg, nullptr, 0U, 0U, PM_REMOVE))
        {
            ::TranslateMessage(&msg);
            ::DispatchMessage(&msg);
            if (msg.message == WM_QUIT)
            {
                // 종료
                break;
            }

            ImGui_ImplDX12_NewFrame();
            ImGui_ImplWin32_NewFrame();
            ImGui::NewFrame();
            Render();
            ImGui::Render();

            // Render things..
            {
                auto mainRenderTargetDescriptor = m_DescriptorHeap->GetRenderTargetDescriptors();
                auto mainRenderTargetResource = m_swapChain->GetMainRenderTargetResources();
                auto commandList = m_commandQueue->GetCmdList().Get();
                auto commandQueue = m_commandQueue->GetCmdQueue();
                auto swapChain = m_swapChain->GetSwapChain3();
                auto srvDescHeap = m_DescriptorHeap->GetSrvHeap();

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

                // Render imgui graphics
                const float clear_color_with_alpha[4] = {clear_color.x * clear_color.w, clear_color.y * clear_color.w,
                                                         clear_color.z * clear_color.w, clear_color.w};
                commandList->ClearRenderTargetView(mainRenderTargetDescriptor[backBufferIdx],
                                                         clear_color_with_alpha, 0, nullptr);
                commandList->OMSetRenderTargets(1, &mainRenderTargetDescriptor[backBufferIdx], FALSE, nullptr);
                commandList->SetDescriptorHeaps(1, &srvDescHeap);
                ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), commandList);
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
            }
        }
    }

    void Direct3dDriver::OnUpdate(float in_delta)
    {
    }

    void Direct3dDriver::Render()
    {
        static float f = 0.0f;
        static int counter = 0;

        ImGui::Begin("Hello, world!");
        ImGui::Text("This is some useful text.");          // Display some text (you can use a format strings too)
        // ImGui::Checkbox("Demo Window", &true); // Edit bools storing our window open/close state
        // ImGui::Checkbox("Another Window", &true);

        ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
        ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

        if (ImGui::Button(
                "Button")) // Buttons return true when clicked (most widgets return true when edited/activated)
            counter++;
        ImGui::SameLine();
        ImGui::Text("counter = %d", counter);

        // ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
        ImGui::End();
    }

    void Direct3dDriver::SwapBuffers()
    {
    }

    bool Direct3dDriver::IsWindowShouldClosed()
    {
        if (bShutDownCalled)
        {
            const HWND hwnd = GetWinWindow()->m_hwnd;
            const WNDCLASSEXW wc = GetWinWindow()->m_wc;

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

    void Direct3dDriver::InitDevice()
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
        m_debugController->Release();
#endif
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
            cmdQueue->Release();
            cmdQueue.Reset();
        }

        if (ComPtr<ID3D12GraphicsCommandList> cmdList = m_commandQueue->GetCmdList())
        {
            cmdList->Release();
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

#ifdef _DEBUG
        // 이거 안되는데 나중에 한 번 다시 보기
        // IDXGIDebug1* pDebug = nullptr;
        // if (SUCCEEDED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(&pDebug))))
        // {
        //     pDebug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_SUMMARY);
        //     pDebug->Release();
        // }
#endif
    }

    WinsWindow* Direct3dDriver::GetWinWindow() const
    {
        // TEMP
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