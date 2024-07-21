#pragma once

#include "Driver.h"
#include "Render/Camera.h"
#include "Render/DirectX12/CommandQueue.h"
#include "Render/DirectX12/RootSignature.h"
#include "Render/DirectX12/SwapChain.h"
#include "Render/DirectX12/TableDescriptorHeap.h"
#include "Render/DirectX12/ConstantBuffer.h"
#include "corepch.h"
#include "imgui.h"
#include <variant>

struct GLFWwindow;

namespace sb
{
    class WinsWindow;

    struct FrameContext
    {
        ID3D12CommandAllocator* CommandAllocator;
        uint64 FenceValue;
    };

    struct ImGuiPropertyPlaceHolder
    {
        using ImGuiProperty = std::variant<float, int32, Vector3f>;

        ImGuiProperty m_property;
    };

    class Direct3dDriver : public Driver
    {
    public:
        Direct3dDriver(Window* in_window);
        Direct3dDriver() = default;
        ~Direct3dDriver();

        void* GetNativeWindow() final { return nullptr; }; // ??

        bool BindWinWindow(const HWND in_hwnd) final;
        void Update() final;
        void OnUpdate(float in_delta) final;
        void Render() final {} // ??
        void SwapBuffers() final;
        bool IsWindowShouldClosed() final;
        void CleanDriver() final;

        // TEMP RenderFunc
        void InitRenderInfo();

        void InitD3dDevice();
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

        // ImGui Interface
        void EnqueueImGuiProperty(ImGuiPropertyPlaceHolder in_property);

    private:
        void CleanUpDevice();

        WinsWindow* GetTargetWindow() const;

        FrameContext* WaitForNextFrameResources();

        class Direct3dContext* m_direct3dContext = nullptr;
        class DirectXShader* m_directdShader = nullptr;

        ComPtr<ID3D12Debug> m_debugController;
        ComPtr<IDXGIFactory4> m_dxgi;
        ComPtr<ID3D12Device> m_device;

        Camera m_camera;
        
        UPtr<RootSignature> m_rootSignature = nullptr;
        UPtr<CommandQueue> m_commandQueue = nullptr;
        UPtr<SwapChain> m_swapChain = nullptr;
        UPtr<TableDescriptorHeap> m_DescriptorHeap = nullptr;
        UPtr<ConstantBuffer> m_constantBuffer = nullptr;

        ComPtr<ID3D12Fence> m_fence; // fence도 객체화 되는 것이??
        HANDLE m_fenceEvent = INVALID_HANDLE_VALUE;
        HANDLE m_hSwapChainWaitableObject = nullptr;

        FrameContext m_frameContexts[NUM_FRAMES_IN_FLIGHT] = {};
        uint32 m_frameIndex = 0;
        uint32 m_fenceLastSignaledValue = 0;

        bool bShutDownCalled = false;

        std::vector<ImGuiPropertyPlaceHolder> m_ImGuiProperties;
    };
};