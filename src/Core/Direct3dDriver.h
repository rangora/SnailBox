#pragma once

#include "Driver.h"
#include "Render/Camera.h"
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

        void* GetNativeWindow() final
        {
            return nullptr;
        }; // ??

        bool BindWinWindow(const HWND in_hwnd) final;
        void Update() final;
        void OnUpdate(float in_delta) final;
        void Render() final
        {
        } // ??
        void SwapBuffers() final;
        bool IsWindowShouldClosed() final;
        void CleanDriver() final;

        // TEMP RenderFunc
        void InitRenderInfo();

        void InitD3dDevice();
        ComPtr<ID3D12Device> GetDevice() const { return m_device; }
        IDXGISwapChain3* GetSwapChain() const { return _swapChain3.Get(); }
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
        void CreateSwapChain(const HWND in_hwnd);
        void CreateRtvDescriptorHeap();
        void CleanUpDevice();

        WinsWindow* GetTargetWindow() const;

        FrameContext* WaitForNextFrameResources();
        FrameContext* WaitForPreviousFrame();

        class Direct3dContext* m_direct3dContext = nullptr;
        class ShaderResource* _shaderResource = nullptr; // temp

        ComPtr<ID3D12CommandQueue> _commandQueue = nullptr;
        ComPtr<ID3D12GraphicsCommandList> _commandList = nullptr;
        ComPtr<ID3D12Debug> m_debugController = nullptr;
        ComPtr<IDXGIFactory4> m_dxgi = nullptr;
        ComPtr<ID3D12Device> m_device = nullptr;
        ComPtr<IDXGISwapChain3> _swapChain3 = nullptr;

        ComPtr<ID3D12DescriptorHeap> _mainRtvHeap = nullptr;
        D3D12_CPU_DESCRIPTOR_HANDLE _mainRtvCpuHandle[SWAP_CHAIN_BUFFER_COUNT] = {};
        ID3D12Resource* _mainRtvResource[SWAP_CHAIN_BUFFER_COUNT] = {};
        
        ComPtr<ID3D12DescriptorHeap> _srvHeap = nullptr;

        D3D12_VIEWPORT _viewport;
        D3D12_RECT _scissorRect;

        Camera m_camera;

        ComPtr<ID3D12Fence> m_fence;
        HANDLE m_fenceEvent = INVALID_HANDLE_VALUE;
        HANDLE m_hSwapChainWaitableObject = nullptr;

        FrameContext m_frameContexts[NUM_FRAMES_IN_FLIGHT] = {};
        uint32 m_frameIndex = 0;
        uint32 m_fenceLastSignaledValue = 0;

        bool bShutDownCalled = false;

        std::vector<ImGuiPropertyPlaceHolder> m_ImGuiProperties;
    };
}; // namespace sb