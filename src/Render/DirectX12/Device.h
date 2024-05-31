#pragma once

#include <Core/Common.h>
#include <dxgi.h>
#include <d3d12.h>

namespace sb
{
    class Device
    {
    public:
        void Init();

        ComPtr<IDXGIFactory> GetDXGI() { return m_dxgi; }
        ComPtr<ID3D12Device> GetDevice() { return m_device; }

    private:
        ComPtr<ID3D12Debug> m_debugController;
        ComPtr<IDXGIFactory> m_dxgi;
        ComPtr<ID3D12Device> m_device;
    };
} // namespace sb