#pragma once

#include "corepch.h"

namespace sb
{
    struct ShaderParameters
    {
        std::string _vsPath;
        std::string _fsPath;
    };

    struct ShaderResourceInitializeData
    {
        std::string _shaderKey;
        ShaderParameters _parameters;
    };

    class ShaderResource
    {
    public:
        ShaderResource() = default;
        ShaderResource(const ShaderResourceInitializeData& initializeData);
        void Init(); // will be deprecated

        ComPtr<ID3D12RootSignature> GetRootSignature() const { return _rootSignature; }
        ComPtr<ID3D12PipelineState> GetPipelineState() const { return _pipelineState; }
        D3D12_VERTEX_BUFFER_VIEW* GetVertexBufferView() { return &_vBufferView; }
        D3D12_INDEX_BUFFER_VIEW& GetIndexBufferView() { return _iBufferView; }

    private:
        void CreateRootSignature();
        void CreateShader(const ShaderResourceInitializeData& initializeData);

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