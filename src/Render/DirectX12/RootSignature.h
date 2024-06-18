#pragma once

#include "corepch.h"

namespace sb
{
    class RootSignature
    {
    public:
        void Init(ComPtr<ID3D12Device> in_device);
        ComPtr<ID3D12RootSignature> GetSignature() { return m_signature; }

    private:
        ComPtr<ID3D12RootSignature> m_signature;
    };
};