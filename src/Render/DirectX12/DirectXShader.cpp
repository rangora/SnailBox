#include "DirectXShader.h"
#include "Core/Application.h"

namespace sb
{
    void DirectXShader::Init(const std::wstring& in_path)
    {
        CreateVertexShader(in_path, "VS_Main", "vs_5_0");
        CreatePixelShader(in_path, "PS_Main", "ps_5_0");

        // InputLayout
        D3D12_INPUT_ELEMENT_DESC desc[] = {
            {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
            {"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0}};

        m_pipelineDesc.InputLayout = {desc, _countof(desc)};
        m_pipelineDesc.pRootSignature = sg_d3dDriver->GetRootSignature()->GetSignature().Get();
        m_pipelineDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
        m_pipelineDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
        m_pipelineDesc.DepthStencilState.DepthEnable = FALSE;
        m_pipelineDesc.DepthStencilState.StencilEnable = FALSE;
        m_pipelineDesc.SampleMask = UINT_MAX;
        m_pipelineDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
        m_pipelineDesc.NumRenderTargets = 1;
        m_pipelineDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
        m_pipelineDesc.SampleDesc.Count = 1;

        HRESULT Result = sg_d3dDevice->CreateGraphicsPipelineState(&m_pipelineDesc, IID_PPV_ARGS(&m_pipelineState));
        if (Result == FALSE)
        {
            assert(false);
        }
    }

    void DirectXShader::Update(ComPtr<ID3D12GraphicsCommandList> in_commandList)
    {
        in_commandList->SetPipelineState(m_pipelineState.Get());
    }

    void DirectXShader::CreateShader(const std::wstring& in_path, const std::string& in_name,
                                     const std::string& in_version, ComPtr<ID3DBlob>& in_blob,
                                     D3D12_SHADER_BYTECODE& in_shaderByteCode)
    {
        uint32 compileFlag = 0;
#ifdef _DEBUG
        compileFlag = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

        if (FAILED(::D3DCompileFromFile(in_path.c_str(), nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, in_name.c_str(),
                                        in_version.c_str(), compileFlag, 0, &in_blob, &m_errBlob)))
        {
            assert(false);
        }

        in_shaderByteCode = {in_blob->GetBufferPointer(), in_blob->GetBufferSize()};
    }

    void DirectXShader::CreateVertexShader(const std::wstring& in_path, const std::string& in_name,
                                           const std::string& in_version)
    {
        CreateShader(in_path, in_name, in_version, m_vsBlob, m_pipelineDesc.VS);
    }

    void DirectXShader::CreatePixelShader(const std::wstring& in_path, const std::string& in_name,
                                          const std::string& in_version)
    {
        CreateShader(in_path, in_name, in_version, m_vsBlob, m_pipelineDesc.PS);
    }
} // namespace sb