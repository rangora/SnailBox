#include "Device.h"

namespace sb
{

    void Device::Init()
    {
        // 디버깅 기능 활성화
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
#endif
    }
} // namespace sb
