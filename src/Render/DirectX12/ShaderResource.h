#pragma once

#include "DxFormat.h"
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
        D3D12_INDEX_BUFFER_VIEW& GetIndexBufferView() { return _iBufferView; }

    private:
        void CreateRootSignature();
        void CreateShader();

        //Vertex1 vList[3] = {{{0.0f, 0.5f, 0.5f}}, {{0.5f, -0.5f, 0.5f}}, {{-0.5f, -0.5f, 0.5f}}};
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

        D3D12_SUBRESOURCE_DATA _vData = {};
        D3D12_VERTEX_BUFFER_VIEW _vBufferView = {};
        int32 _vBufferSize = 0;

        ComPtr<ID3D12Resource> _iBufferUploadHeap = nullptr;
        ComPtr<ID3D12Resource> _iBuffer = nullptr;
        D3D12_INDEX_BUFFER_VIEW _iBufferView = {};
        D3D12_SUBRESOURCE_DATA _iData = {};
    };
}; // namespace sb