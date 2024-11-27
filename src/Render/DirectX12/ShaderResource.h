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

    struct ShaderHeapInstruction
    {
        D3D12_DESCRIPTOR_RANGE_TYPE _rangeType;
        int32 _numDescriptors = 0;
        bool _bTable = false;
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
        ShaderResource(const ShaderResourceInitializeData& initializeData, const ShaderHeapInstruction& instruction);
        ~ShaderResource();

        ComPtr<ID3D12RootSignature> GetRootSignature() const { return _rootSignature; }
        ComPtr<ID3D12PipelineState> GetPipelineState() const { return _pipelineState; }
        D3D12_VERTEX_BUFFER_VIEW* GetVertexBufferView() { return &_vBufferView; }
        D3D12_INDEX_BUFFER_VIEW& GetIndexBufferView() { return _iBufferView; }
        ComPtr<ID3D12DescriptorHeap> GetCbvHeap() const { return _cbvHeap; }

        // TEMP
        void Tick(float delta);
        void Render(ComPtr<ID3D12GraphicsCommandList> commandList);

    private:
        void CreateHeap(const ShaderHeapInstruction& instruction);
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

        // constnat buffer data
        ComPtr<ID3D12DescriptorHeap> _cbvHeap = nullptr;
        ComPtr<ID3D12Resource> _cbUploadHeap = nullptr;
        UINT8* _cbGPUAddress = nullptr;
        ConstantBuffer _cbData;

    };
}; // namespace sb