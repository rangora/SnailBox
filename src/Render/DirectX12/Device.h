#pragma once

#include "corepch.h"

namespace sb
{
    class Device
    {
    public:
        void Init();

        ComPtr<IDXGIFactory4> GetDXGI() { return m_dxgi; }
        ComPtr<ID3D12Device> GetDevice() { return m_device; }

    private:
        ComPtr<ID3D12Debug> m_debugController;
        ComPtr<IDXGIFactory4> m_dxgi;
        ComPtr<ID3D12Device> m_device;
    };
} // namespace sb