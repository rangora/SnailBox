#pragma once

#include "Driver.h"
#include "Render/Camera.h"
#include "corepch.h"
#include "imgui.h"
#include <variant>
#include "Render/RenderResource.h"
#include "boost/container/flat_map.hpp"

namespace sb
{
    struct FrameContext
    {
        ID3D12CommandAllocator* CommandAllocator;
        uint64 FenceValue;
    };

    class Direct3dDriver : public Driver
    {
    public:
        Direct3dDriver(Window* in_window);
        Direct3dDriver();
        ~Direct3dDriver();

        void* GetNativeWindow() final
        {
            return nullptr;
        }; // ??

        bool BindWinWindow(const HWND in_hwnd) final;
        bool InitializeResources() final;
        void Update() final;
        void OnUpdate(float in_delta) final;
        void Render() final;
        void SwapBuffers() final;
        bool IsWindowShouldClosed() final;
        void CleanDriver() final;

        void InitD3dDevice();

        void UpdateFenceValue();

        ComPtr<ID3D12Device> GetDevice() const { return m_device; }
        IDXGISwapChain3* GetSwapChain() const { return _swapChain3.Get(); }
        ComPtr<ID3D12Fence> GetFence() const { return m_fence; }
        ComPtr<ID3D12GraphicsCommandList> GetCommandList() const { return _commandList; }
        ComPtr<ID3D12CommandQueue> GetCommandQueue() const { return _commandQueue; }

        HANDLE GetFenceEvent() const { return m_fenceEvent; }
        HANDLE GetSwapChainWaitableObject() const { return m_hSwapChainWaitableObject; }
        struct ShaderGeometry* GetShaderData(const std::string& key) const;
        uint32 GetViewportWidth() const { return _vpWidth; }
        uint32 GetViewportHeight() const { return _vpHeight; }
        uint32 GetCurrentFrameIndex() const { return m_frameIndex; }

        void SetSwapChainWaitableObject(HANDLE in_handle) { m_hSwapChainWaitableObject = in_handle; }

        Camera& GetCameraRef() { return m_camera; }
        FrameContext* GetFrameContexts() { return m_frameContexts; }
        FrameContext* GetCurrentFrameContexts() { return &m_frameContexts[m_frameIndex]; }

        void WaitForLastSubmittedFrame();
        void CreateRenderTarget();
        void CleanUpRenderTarget();
        void CreateDepthStencil();

    private:
        void CreateSwapChain(const HWND in_hwnd);
        void CreateRtvDescriptorHeap();
        void CleanUpDevice();

        void UpdateViewMatrix();
        void RenderBegin();
        void RenderEnd();
        void RenderImgui();

        class WinsWindow* GetTargetWindow() const;

        FrameContext* WaitForNextFrameResources();
        FrameContext* WaitForPreviousFrame();

        void UpdateTick(); // temp

        class Direct3dContext* m_direct3dContext = nullptr;
        class ShaderResource* _shaderResource = nullptr; // temp

        // Direct objects
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
        ComPtr<ID3D12DescriptorHeap> _dsHeap = nullptr;
        ComPtr<ID3D12Resource> _dsBuffer = nullptr;

        // Viewport
        D3D12_VIEWPORT _viewport;
        D3D12_RECT _scissorRect;
        uint32 _vpWidth = 0;
        uint32 _vpHeight = 0;

        // geometry
        boost::container::flat_map<std::string, UPtr<struct ShaderGeometry>> _shaderGeoData;

        // Shaders
        boost::container::flat_map<std::string, UPtr<ShaderResource>> _shaderData;

        Camera m_camera;

        ComPtr<ID3D12Fence> m_fence;
        HANDLE m_fenceEvent = INVALID_HANDLE_VALUE;
        HANDLE m_hSwapChainWaitableObject = nullptr;

        FrameContext m_frameContexts[NUM_FRAMES_IN_FLIGHT] = {};
        int32 _backBufferIndex = 0;
        uint32 m_frameIndex = 0;
        uint32 m_fenceLastSignaledValue = 0;
        double _PrevTime = 0.0;
        float _tick = 0.f;

        bool bShutDownCalled = false;

    public:
        // [TEMP] camera data
        XMFLOAT4X4 _cameraProjMat;
        XMFLOAT4X4 _cameraViewMat;

        XMVECTOR _cameraPosition;
        XMVECTOR _cameraTarget;
        XMVECTOR _cameraUp;
        XMVECTOR _cameraForward;

        XMFLOAT4 _cameraRight;

        XMFLOAT4X4 _cube1WorldMat;
        XMFLOAT4X4 _cube1RotMat;
        XMFLOAT4 _cube1Position;

        XMFLOAT4X4 _cube2WorldMat;
        XMFLOAT4X4 _cube2RotMat;
        XMFLOAT4 _cube2PositionOffset;

        Camera _camera;

        void UpdateViewMatrix2();

        int _numCubeIndices = 36;
        // ~[TEMP]
    };
}; // namespace sb