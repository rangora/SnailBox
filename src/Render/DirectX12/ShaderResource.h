#pragma once

#include "DxFormat.h"
#include "corepch.h"

namespace sb
{
    class ShaderResource
    {
    public:
        ShaderResource() = default;
        void Init();

        ComPtr<ID3D12RootSignature> GetRootSignature() const { return _rootSignature; }
        ComPtr<ID3D12PipelineState> GetPipelineState() const { return _pipelineState; }
        D3D12_VERTEX_BUFFER_VIEW* GetVertexBufferView() { return &_vBufferView; }
        D3D12_INDEX_BUFFER_VIEW& GetIndexBufferView() { return _iBufferView; }

    private:
        void CreateRootSignature();
        void CreateShader();

        dxFormat::Vertex vList2[4] = 
        {
            XMFLOAT3(-0.5f, 0.5f, 0.5f), XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f),
            XMFLOAT3(0.5f, -0.5f, 0.5f), XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f),
            XMFLOAT3(-0.5f, -0.5f, 0.5f), XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f),
            XMFLOAT3(0.5f,  0.5f, 0.5f), XMFLOAT4(1.0f, 0.0f, 1.0f, 1.0f)
        };
        DWORD iList[6] = {0, 1, 2, 0, 3, 1};


        ComPtr<ID3D12RootSignature> _rootSignature = nullptr;
        ComPtr<ID3D12PipelineState> _pipelineState = nullptr;

        ComPtr<ID3D12Resource> _vBufferUploadHeap = nullptr;
        ComPtr<ID3D12Resource> _vBuffer = nullptr;
        D3D12_VERTEX_BUFFER_VIEW _vBufferView = {};

        ComPtr<ID3D12Resource> _iBufferUploadHeap = nullptr;
        ComPtr<ID3D12Resource> _iBuffer = nullptr;
        D3D12_INDEX_BUFFER_VIEW _iBufferView = {};
    };
}; // namespace sb