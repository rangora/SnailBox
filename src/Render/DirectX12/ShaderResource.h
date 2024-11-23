#pragma once

#include "corepch.h"

namespace sb
{
    // TEMP
    struct ConstantBuffer
    {
        XMFLOAT4 _colorMultiplier;
    };

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
        ShaderResource() = delete;
        ShaderResource(const ShaderResource&) = delete;
        ShaderResource& operator=(const ShaderResource&) = delete;
        ShaderResource(const ShaderResourceInitializeData& initializeData);

        ComPtr<ID3D12RootSignature> GetRootSignature() const { return _rootSignature; }
        ComPtr<ID3D12PipelineState> GetPipelineState() const { return _pipelineState; }
        ComPtr<ID3D12PipelineState> GetPipelineState2() const { return _pipelineState2; }
        D3D12_VERTEX_BUFFER_VIEW* GetVertexBufferView() { return &_vBufferView; }
        D3D12_INDEX_BUFFER_VIEW& GetIndexBufferView() { return _iBufferView; }

        // TEMP
        void Tick();

    private:
        void CreateRootSignature();
        void CreateShader(const ShaderResourceInitializeData& initializeData);

        ComPtr<ID3D12RootSignature> _rootSignature = nullptr;
        ComPtr<ID3D12PipelineState> _pipelineState = nullptr;
        ComPtr<ID3D12PipelineState> _pipelineState2 = nullptr;

        ComPtr<ID3D12Resource> _vBufferUploadHeap = nullptr;
        ComPtr<ID3D12Resource> _vBuffer = nullptr;
        D3D12_VERTEX_BUFFER_VIEW _vBufferView = {};

        ComPtr<ID3D12Resource> _iBufferUploadHeap = nullptr;
        ComPtr<ID3D12Resource> _iBuffer = nullptr;
        D3D12_INDEX_BUFFER_VIEW _iBufferView = {};

        // constnat buffer data
        ComPtr<ID3D12DescriptorHeap> _cbDescHeap = nullptr;
        ComPtr<ID3D12Resource> _cbUploadHeap = nullptr;
        UINT8* _cbGPUAddress = nullptr;
        ConstantBuffer _cbData;
    };
}; // namespace sb