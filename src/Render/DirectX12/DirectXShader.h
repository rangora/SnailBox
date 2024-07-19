#pragma once

#include "corepch.h"

namespace sb
{
    class DirectXShader
    {
    public:
        void Init(const std::wstring& in_path);
        void Update(ComPtr<ID3D12GraphicsCommandList> in_commandList);

    private:
        void CreateShader(const std::wstring& in_path, const std::string& in_name, const std::string& in_version,
                          ComPtr<ID3DBlob>& in_blob, D3D12_SHADER_BYTECODE& in_shaderByteCode);
        void CreateVertexShader(const std::wstring& in_path, const std::string& in_name, const std::string& in_version);
        void CreatePixelShader(const std::wstring& in_path, const std::string& in_name, const std::string& in_version);

        ComPtr<ID3DBlob> m_vsBlob = nullptr;
        ComPtr<ID3DBlob> m_psBlob = nullptr;
        ComPtr<ID3DBlob> m_errBlob = nullptr;

        ComPtr<ID3D12PipelineState> m_pipelineState = nullptr;
        D3D12_GRAPHICS_PIPELINE_STATE_DESC m_pipelineDesc = {};
    };
}; // namespace sb