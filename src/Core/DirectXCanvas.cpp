#include "DirectXCanvas.h"

#include "imgui.h"
#include "imgui_impl_dx12.h"
#include "imgui_impl_win32.h"
#include <d3d12.h>
#include <dxgi1_4.h>
#include <tchar.h>

#ifdef _DEBUG
#define DX12_ENABLE_DEBUG_LAYER
#endif

#ifdef DX12_ENABLE_DEBUG_LAYER
#include <dxgidebug.h>
#pragma comment(lib, "dxguid.lib")
#endif

// forward declare
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

namespace
{
    constexpr uint32 NUM_BACK_BUFFERS = 3;
    constexpr int NUM_FRAMES_IN_FLIGHT = 3;
}

struct FrameContext
{
    ID3D12CommandAllocator* CommandAllocator;
    uint64 FenceValue;
};

// DirectX data
ID3D12Device* g_cacheDevice = nullptr;
static ID3D12DescriptorHeap* g_pd3dRtvDescHeap = nullptr;
static ID3D12DescriptorHeap* g_pd3dSrvDescHeap = nullptr;
static ID3D12CommandQueue* g_pd3dCommandQueue = nullptr;
static ID3D12GraphicsCommandList* g_pd3dCommandList = nullptr;
static ID3D12Fence* g_fence = nullptr;
static IDXGISwapChain3* g_pSwapChain = nullptr;
static HANDLE g_fenceEvent = nullptr;
static HANDLE g_hSwapChainWaitableObject = nullptr;
static D3D12_CPU_DESCRIPTOR_HANDLE g_mainRenderTargetDescriptor[NUM_BACK_BUFFERS] = {};
static FrameContext g_frameContext[NUM_FRAMES_IN_FLIGHT] = {};
static ID3D12Resource* g_mainRenderTargetResource[NUM_BACK_BUFFERS] = {};
static UINT g_frameIndex = 0;
static UINT64 g_fenceLastSignaledValue = 0;

// DirectX helper functions
void WaitForLastSubmittedFrame()
{
    FrameContext* frameCtx = &g_frameContext[g_frameIndex % NUM_FRAMES_IN_FLIGHT];

    uint64 fenceValue = frameCtx->FenceValue;
    if (frameCtx->FenceValue = 0)
    {
        return; // No fence was signaled
    }

    frameCtx->FenceValue = 0;
    if (g_fence->GetCompletedValue() >= fenceValue)
    {
        return;
    }

    g_fence->SetEventOnCompletion(fenceValue, g_fenceEvent);
    WaitForSingleObject(g_fenceEvent, INFINITE);
}

void CleanupRenderTarget()
{
    WaitForLastSubmittedFrame();

    for (int i = 0; i < NUM_BACK_BUFFERS; i++)
    {
        if (g_mainRenderTargetResource[i])
        {
            g_mainRenderTargetResource[i]->Release();
            g_mainRenderTargetResource[i] = nullptr;
        }
    }
}

void CreateRenderTarget()
{
    for (int i = 0; i < NUM_BACK_BUFFERS; i++)
    {
        ID3D12Resource* pBackBuffer = nullptr;
        g_pSwapChain->GetBuffer(i, IID_PPV_ARGS(&pBackBuffer));
        g_cacheDevice->CreateRenderTargetView(pBackBuffer, nullptr, g_mainRenderTargetDescriptor[i]);
        g_mainRenderTargetResource[i] = pBackBuffer;
    }
}

void CleanupDeviceD3D()
{
    CleanupRenderTarget();
    if (g_pSwapChain)
    {
        g_pSwapChain->SetFullscreenState(false, nullptr);
        g_pSwapChain->Release();
        g_pSwapChain = nullptr;
    }

    if (g_hSwapChainWaitableObject != nullptr)
    {
        CloseHandle(g_hSwapChainWaitableObject);
    }

    for (int i = 0; i < NUM_FRAMES_IN_FLIGHT; i++)
    {
        g_frameContext[i].CommandAllocator->Release();
        g_frameContext[i].CommandAllocator = nullptr;
    }

    if (g_pd3dCommandQueue)
    {
        g_pd3dCommandQueue->Release();
        g_pd3dCommandQueue = nullptr;
    }

    if (g_pd3dCommandList)
    {
        g_pd3dCommandList->Release();
        g_pd3dCommandList = nullptr;
    }

    if (g_pd3dRtvDescHeap)
    {
        g_pd3dRtvDescHeap->Release();
        g_pd3dRtvDescHeap = nullptr;
    }

    if (g_pd3dSrvDescHeap)
    {
        g_pd3dSrvDescHeap->Release();
        g_pd3dSrvDescHeap = nullptr;
    }

    if (g_fence)
    {
        g_fence->Release();
        g_fence = nullptr;
    }

    if (g_fenceEvent)
    {
        CloseHandle(g_fenceEvent);
        g_fenceEvent = nullptr;
    }

    if (g_cacheDevice)
    {
        g_cacheDevice->Release();
        g_cacheDevice = nullptr;
    }

#ifdef DX12_ENABLE_DEBUG_LAYER
    IDXGIDebug1* pDebug = nullptr;
    if (SUCCEEDED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(&pDebug))))
    {
        pDebug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_SUMMARY);
        pDebug->Release();
    }
#endif
}

FrameContext* WaitForNextFrameResources()
{
    uint32 nextFrameIndex = g_frameIndex + 1;
    g_frameIndex = nextFrameIndex;

    HANDLE waitableObjects[] = {g_hSwapChainWaitableObject, nullptr};
    DWORD numWaitableObjects = 1;

    FrameContext* frameCtx = &g_frameContext[nextFrameIndex % NUM_FRAMES_IN_FLIGHT];
    uint64 fenceValue = frameCtx->FenceValue;
    if (fenceValue != 0) // Means no fence was singaled
    {
        frameCtx->FenceValue = 0;
        g_fence->SetEventOnCompletion(fenceValue, g_fenceEvent);
        waitableObjects[1] = g_fenceEvent;
        numWaitableObjects = 2;
    }

    WaitForMultipleObjects(numWaitableObjects, waitableObjects, TRUE, INFINITE);

    return frameCtx;
}

LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
    {
        return true;
    }

    switch (msg)
    {
        case WM_SIZE:
        {
            if (wParam != SIZE_MINIMIZED)
            {
                WaitForLastSubmittedFrame();
                CleanupRenderTarget();
                HRESULT result =
                    g_pSwapChain->ResizeBuffers(0, (UINT)LOWORD(lParam), (UINT)HIWORD(lParam), DXGI_FORMAT_UNKNOWN,
                                                DXGI_SWAP_CHAIN_FLAG_FRAME_LATENCY_WAITABLE_OBJECT);

                assert(SUCCEEDED(result) && "Failed to reszie swapchain.");
                CreateRenderTarget();
            }
            return 0;
        }
        case WM_SYSCOMMAND:
        {
            // Disable ALT application menu
            if ((wParam & 0xfff0) == SC_KEYMENU)
            {
                return 0;
            }
            break;
        }
        case WM_DESTROY:
        {
            ::PostQuitMessage(0);
            break;
        }
    }

    return ::DefWindowProcW(hWnd, msg, wParam, lParam);
}

namespace sb
{
    DirectXCanvas::DirectXCanvas(Window* in_window)
    : Canvas(in_window)
    {
    }

    void* DirectXCanvas::GetNativeWindow()
    {

        return nullptr;
    }

    bool DirectXCanvas::InitCanvas(const WinWindowData* in_windowData)
    {
        m_device = CreateUPtr<Device>();

        WNDCLASSEXW wc = {
            sizeof(wc), CS_CLASSDC, WndProc,          0L,     0L, GetModuleHandle(nullptr), nullptr, nullptr,
            nullptr,    nullptr,    L"Imgui Example", nullptr};
        ::RegisterClassExW(&wc);

        HWND hwnd = ::CreateWindowW(wc.lpszClassName, L"Dear Imgui DirectX12 Example", WS_OVERLAPPEDWINDOW, 100, 100,
                                    1280, 800, nullptr, nullptr, wc.hInstance, nullptr);

        if (!CreateDevice(hwnd))
        {
            CleanupDeviceD3D();
            ::UnregisterClassW(wc.lpszClassName, wc.hInstance);
            return false;
        }

        // Show window
        ::ShowWindow(hwnd, SW_SHOWDEFAULT);
        ::UpdateWindow(hwnd);

        // Setup imguiContext
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO();
        (void)io;
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

        ImGui::StyleColorsDark();

        // Setup platform/Renderer backends
        ImGui_ImplWin32_Init(hwnd);
        ImGui_ImplDX12_Init(g_cacheDevice, NUM_FRAMES_IN_FLIGHT, DXGI_FORMAT_R8G8B8A8_UNORM, g_pd3dSrvDescHeap,
                            g_pd3dSrvDescHeap->GetCPUDescriptorHandleForHeapStart(),
                            g_pd3dSrvDescHeap->GetGPUDescriptorHandleForHeapStart());

        // State
        bool show_demo_window = true;
        bool show_another_window = false;

        return true;
    }

    void DirectXCanvas::Update()
    {
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
                FrameContext* frameCtx = WaitForNextFrameResources();
                uint32 backBufferIdx = g_pSwapChain->GetCurrentBackBufferIndex();
                frameCtx->CommandAllocator->Reset();

                D3D12_RESOURCE_BARRIER barrier = {};
                barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
                barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
                barrier.Transition.pResource = g_mainRenderTargetResource[backBufferIdx];
                barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
                barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
                barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
                g_pd3dCommandList->Reset(frameCtx->CommandAllocator, nullptr);
                g_pd3dCommandList->ResourceBarrier(1, &barrier);

                // Render imgui graphics
                const float clear_color_with_alpha[4] = {clear_color.x * clear_color.w, clear_color.y * clear_color.w,
                                                         clear_color.z * clear_color.w, clear_color.w};
                g_pd3dCommandList->ClearRenderTargetView(g_mainRenderTargetDescriptor[backBufferIdx],
                                                         clear_color_with_alpha, 0, nullptr);
                g_pd3dCommandList->OMSetRenderTargets(1, &g_mainRenderTargetDescriptor[backBufferIdx], FALSE, nullptr);
                g_pd3dCommandList->SetDescriptorHeaps(1, &g_pd3dSrvDescHeap);
                ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), g_pd3dCommandList);
                barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
                barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
                g_pd3dCommandList->ResourceBarrier(1, &barrier);
                g_pd3dCommandList->Close();

                g_pd3dCommandQueue->ExecuteCommandLists(1, (ID3D12CommandList* const*)&g_pd3dCommandList);

                g_pSwapChain->Present(1, 0); // 1이면 vsync

                uint64 fenceValue = g_fenceLastSignaledValue + 1;
                g_pd3dCommandQueue->Signal(g_fence, fenceValue);
                g_fenceLastSignaledValue = fenceValue;
                frameCtx->FenceValue = fenceValue;
            }
        }
    }

    void DirectXCanvas::OnUpdate(float in_delta)
    {
    }

    void DirectXCanvas::Render()
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

    void DirectXCanvas::SwapBuffers()
    {
    }

    bool DirectXCanvas::IsWindowShouldClosed()
    {
        return false;
    }

    bool DirectXCanvas::CreateDevice(HWND in_hwnd)
    {
        // Device
        m_device->Init();
        g_cacheDevice = m_device->GetDevice().Get();
        assert(g_cacheDevice);

        // SwapChain
        DXGI_SWAP_CHAIN_DESC1 sd;
        {
            ZeroMemory(&sd, sizeof(sd));
            sd.BufferCount = NUM_BACK_BUFFERS;
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
        }

        {
            D3D12_DESCRIPTOR_HEAP_DESC desc = {};
            desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
            desc.NumDescriptors = NUM_BACK_BUFFERS;
            desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
            desc.NodeMask = 1;
            if (g_cacheDevice->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&g_pd3dRtvDescHeap)) != S_OK)
            {
                return false;
            }

            SIZE_T rtvDescriptorSize = g_cacheDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
            D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = g_pd3dRtvDescHeap->GetCPUDescriptorHandleForHeapStart();
            for (UINT i = 0; i < NUM_BACK_BUFFERS; i++)
            {
                g_mainRenderTargetDescriptor[i] = rtvHandle;
                rtvHandle.ptr += rtvDescriptorSize;
            }
        }

        {
            D3D12_DESCRIPTOR_HEAP_DESC desc = {};
            desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
            desc.NumDescriptors = 1;
            desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
            if (g_cacheDevice->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&g_pd3dSrvDescHeap)) != S_OK)
            {
                return false;
            }
        }

        {
            D3D12_COMMAND_QUEUE_DESC desc = {};
            desc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
            desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
            desc.NodeMask = 1;
            if (g_cacheDevice->CreateCommandQueue(&desc, IID_PPV_ARGS(&g_pd3dCommandQueue)) != S_OK)
            {
                return false;
            }
        }

        for (uint32 i = 0; i < NUM_FRAMES_IN_FLIGHT; i++)
        {
            if (g_cacheDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT,
                                                      IID_PPV_ARGS(&g_frameContext[i].CommandAllocator)) != S_OK)
            {
                return false;
            }
        }

        if (g_cacheDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, g_frameContext[0].CommandAllocator,
                                             nullptr, IID_PPV_ARGS(&g_pd3dCommandList)) != S_OK ||
            g_pd3dCommandList->Close() != S_OK)
        {
            return false;
        }

        if (g_cacheDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&g_fence)) != S_OK)
        {
            return false;
        }

        g_fenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
        if (g_fenceEvent == nullptr)
        {
            return false;
        }

        {
            IDXGIFactory4* dxgiFactory = nullptr;
            IDXGISwapChain1* swapChain1 = nullptr;
            if (CreateDXGIFactory1(IID_PPV_ARGS(&dxgiFactory)) != S_OK)
            {
                return false;
            }

            if (dxgiFactory->CreateSwapChainForHwnd(g_pd3dCommandQueue, in_hwnd, &sd, nullptr, nullptr, &swapChain1) !=
                S_OK)
            {
                return false;
            }

            if (swapChain1->QueryInterface(IID_PPV_ARGS(&g_pSwapChain)) != S_OK)
            {
                return false;
            }

            swapChain1->Release();
            dxgiFactory->Release();
            g_pSwapChain->SetMaximumFrameLatency(NUM_BACK_BUFFERS);
            g_hSwapChainWaitableObject = g_pSwapChain->GetFrameLatencyWaitableObject();
        }

        CreateRenderTarget();
        return true;
    }
} // namespace sb