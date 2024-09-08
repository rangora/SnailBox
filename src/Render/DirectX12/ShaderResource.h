#pragma once

#include "corepch.h"

namespace sb
{
    struct Vertex1
    {
        XMFLOAT3 _pos;
    };

    class ShaderResource
    {
    public:
        ShaderResource() = default;
        void Init();
        ComPtr<ID3D12RootSignature> GetRootSignature() const { return _rootSignature; }
        ComPtr<ID3D12PipelineState> GetPipelineState() const { return _pipelineState; }
        ComPtr<ID3D12Resource> GetVertexBuffer() const { return _vBuffer; }
        ComPtr<ID3D12Resource> GetVertexBufferUploadHeap() const { return _vBufferUploadHeap; }
        D3D12_SUBRESOURCE_DATA* GetVertexBufferData() { return &_vData; }
        D3D12_VERTEX_BUFFER_VIEW* GetVertexBufferView() { return &_vBufferView; }

        void CreateVertexBufferView();

    private:
        void CreateRootSignature();
        void CreateShader();

        Vertex1 vList[3] = {{{0.0f, 0.5f, 0.5f}}, {{0.5f, -0.5f, 0.5f}}, {{-0.5f, -0.5f, 0.5f}}};

        ComPtr<ID3D12RootSignature> _rootSignature = nullptr;
        ComPtr<ID3D12PipelineState> _pipelineState = nullptr;
        ComPtr<ID3D12Resource> _vBufferUploadHeap = nullptr;
        ComPtr<ID3D12Resource> _vBuffer = nullptr;

        D3D12_SUBRESOURCE_DATA _vData = {};
        D3D12_VERTEX_BUFFER_VIEW _vBufferView = {};
        int32 _vBufferSize = 0;
    };
}; // namespace sb