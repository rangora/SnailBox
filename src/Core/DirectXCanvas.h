#pragma once

#include "Canvas.h"
#include "Render/Camera.h"
#include "Render/DirectX12/CommandQueue.h"
#include "Render/DirectX12/RootSignature.h"
#include "Render/DirectX12/SwapChain.h"
#include "Render/DirectX12/TableDescriptorHeap.h"
#include "imgui.h"
#include "corepch.h"

struct GLFWwindow;

namespace sb
{
    class Window;

    struct FrameContext
    {
        ID3D12CommandAllocator* CommandAllocator;
        uint64 FenceValue;
    };

    class DirectXCanvas : public Canvas
    {
    public:
        DirectXCanvas(Window* in_window);
        DirectXCanvas() = delete;

        void* GetNativeWindow() final;

        bool InitCanvas(const WinWindowData* in_windowData) final;
        void Update() final;
        void OnUpdate(float in_delta) final;
        void Render() final;
        void SwapBuffers() final;
        bool IsWindowShouldClosed() final;

        ComPtr<ID3D12Device> GetDevice() const { return m_device; }
        TableDescriptorHeap* GetDescriptorHeap() const { return m_DescriptorHeap.get(); }
        RootSignature* GetRootSignature() const { return m_rootSignature.get(); }
        SwapChain* GetSwapChain() const { return m_swapChain.get(); }
        ComPtr<ID3D12Fence> GetFence() const { return m_fence; }
        HANDLE GetFenceEvent() const { return m_fenceEvent; }
        HANDLE GetSwapChainWaitableObject() const { return m_hSwapChainWaitableObject; }

        void SetSwapChainWaitableObject(HANDLE in_handle) { m_hSwapChainWaitableObject = in_handle; }

        Camera& GetCameraRef() { return m_camera; }

        void WaitForLastSubmittedFrame();
        void CreateRenderTarget();
        void CleanUpRenderTarget();
        FrameContext* GetFrameContexts() { return m_frameContexts; }

    private:
        void InitDevice();
        void CleanUpDevice();

        FrameContext* WaitForNextFrameResources();

        ComPtr<ID3D12Debug> m_debugController;
        ComPtr<IDXGIFactory4> m_dxgi;
        ComPtr<ID3D12Device> m_device;

        Camera m_camera;

        ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

        UPtr<RootSignature> m_rootSignature = nullptr;
        UPtr<CommandQueue> m_commandQueue = nullptr;
        UPtr<SwapChain> m_swapChain = nullptr;
        UPtr<TableDescriptorHeap> m_DescriptorHeap = nullptr;

        ComPtr<ID3D12Fence> m_fence; // fence도 객체화 되는 것이??
        HANDLE m_fenceEvent = INVALID_HANDLE_VALUE;
        HANDLE m_hSwapChainWaitableObject = nullptr;

        FrameContext m_frameContexts[NUM_FRAMES_IN_FLIGHT] = {};
        uint32 m_frameIndex = 0;
        uint32 m_fenceLastSignaledValue = 0;

        // Windows
        WNDCLASSEXW m_wc;
        HWND m_hwnd;

        bool bShutDownCalled = false;
    };
};