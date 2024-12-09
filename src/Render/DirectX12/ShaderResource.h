#pragma once

#include "corepch.h"
#include "Render/GraphicsEnum.h"
#include <vector>

namespace sb
{
    // TEMP
    struct ConstantBuffer
    {
        //XMFLOAT4 _colorMultiplier;
        XMFLOAT4X4 _wvpMat;
    };

    struct ShaderResourceFile
    {
        std::string _vsPath;
        std::string _fsPath;
    };

    struct ShaderHeapInstruction
    {
        D3D12_DESCRIPTOR_RANGE_TYPE _rangeType;
        int32 _numDescriptors = 0;
        bool _bTable = false;
        RootSignatureType _rootSignType;
    };

    struct ShaderResourceDesc
    {
        std::string _shaderKey;
        ShaderResourceFile _files;
    };

    class ShaderResource
    {
    public:
        ShaderResource() = delete;
        //ShaderResource(const ShaderResource&) = delete;
        ShaderResource& operator=(const ShaderResource&) = delete;
        ShaderResource(const ShaderResourceDesc& initializeData, const ShaderHeapInstruction& instruction);
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
        void CreateShader(const ShaderResourceDesc& initializeData, const ShaderHeapInstruction& instruction);

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
        std::vector<ComPtr<ID3D12Resource>> _cbUploadHeaps;
        UINT8* _cbGPUAddress = nullptr;
        std::vector<UINT8*> _cbGPUAddresses;
        ConstantBuffer _cbData;

        int32 _cBufferObjectAlignedSize = (sizeof(ConstantBuffer) + 255) & ~255;
    };
}; // namespace sb