#include "ShaderResource.h"
#include "Core/Application.h"
#include "Render/ShaderGeometry.h"

namespace sb
{
    ShaderResource::ShaderResource(const ShaderResourceInitializeData& initializeData)
    {
        CreateRootSignature();
        CreateShader(initializeData);
    }

    void ShaderResource::CreateRootSignature()
    {
        CD3DX12_ROOT_SIGNATURE_DESC desc = {};
        desc.Init(0, nullptr, 0, nullptr, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

        ID3DBlob* signature;

        HRESULT hr = D3D12SerializeRootSignature(&desc, D3D_ROOT_SIGNATURE_VERSION_1, &signature, nullptr);
        if (FAILED(hr))
        {
            return;
        }

        sg_d3dDevice->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(),
                                          IID_PPV_ARGS(&_rootSignature));

        if (FAILED(hr))
        {
            return;
        }
    }

    void ShaderResource::CreateShader(const ShaderResourceInitializeData& initializeData)
    {
        auto sDataPtr = sg_d3dDriver->GetShaderData(initializeData._shaderKey);
        if (sDataPtr == nullptr)
        {
            spdlog::error("There is so shaderGeometry data named : {}", initializeData._shaderKey);
            return;
        }

        // Compile shader
        std::string stringVsPath = initializeData._parameters._vsPath;
        std::wstring wstringVsPath(stringVsPath.size(), L'\0');
        std::mbstowcs(&wstringVsPath[0], stringVsPath.c_str(), stringVsPath.size());
        std::string stringPsPath = initializeData._parameters._fsPath;
        std::wstring wstringPsPath(stringPsPath.size(), L'\0');
        std::mbstowcs(&wstringPsPath[0], stringPsPath.c_str(), stringPsPath.size());

        ID3DBlob* vsBlob = nullptr;
        ID3DBlob* errBlob = nullptr;

        HRESULT hr = D3DCompileFromFile(wstringVsPath.c_str(), nullptr, nullptr, "main", "vs_5_0",
                                        D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, 0, &vsBlob, &errBlob);
        if (FAILED(hr))
        {
            OutputDebugStringA((char*)errBlob->GetBufferPointer());
            return;
        }

        D3D12_SHADER_BYTECODE vsShaderByteCode = {};
        vsShaderByteCode.BytecodeLength = vsBlob->GetBufferSize();
        vsShaderByteCode.pShaderBytecode = vsBlob->GetBufferPointer();

        ID3DBlob* psBlob = nullptr;
        hr = D3DCompileFromFile(wstringPsPath.c_str(), nullptr, nullptr, "main", "ps_5_0",
                                D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, 0, &psBlob, &errBlob);
        if (FAILED(hr))
        {
            OutputDebugStringA((char*)errBlob->GetBufferPointer());
            return;
        }

        D3D12_SHADER_BYTECODE psShaderByteCode = {};
        psShaderByteCode.BytecodeLength = psBlob->GetBufferSize();
        psShaderByteCode.pShaderBytecode = psBlob->GetBufferPointer();

        // Create inputLayout
        D3D12_INPUT_ELEMENT_DESC inputLayout[] = {
            {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0}, 
            {"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0}};

        D3D12_INPUT_LAYOUT_DESC inputLayoutDesc = {};
        inputLayoutDesc.NumElements = sizeof(inputLayout) / sizeof(D3D12_INPUT_ELEMENT_DESC);
        inputLayoutDesc.pInputElementDescs = inputLayout;

        // Create PSO
        DXGI_SAMPLE_DESC sampleDesc = {};
        sampleDesc.Count = 1;

        D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
        psoDesc.InputLayout = inputLayoutDesc;
        psoDesc.pRootSignature = _rootSignature.Get();
        psoDesc.VS = vsShaderByteCode;
        psoDesc.PS = psShaderByteCode;
        psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
        psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
        psoDesc.SampleDesc = sampleDesc;
        psoDesc.SampleMask = 0xffffffff;
        psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
        psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
        psoDesc.NumRenderTargets = 1;

        hr = sg_d3dDevice->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&_pipelineState));
        if (FAILED(hr))
        {
            assert(false);
            return;
        }
        
        const uint32 _vBufferSize = sDataPtr->GetVertexByteSize();
        const uint32 iBufferSize = sDataPtr->GetIndexByteSize();

        // Vertex Buffer
        {
            // Create defaultHeap
            D3D12_HEAP_PROPERTIES defaultHeapProps = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
            D3D12_RESOURCE_DESC defaultResourceDesc = CD3DX12_RESOURCE_DESC::Buffer(_vBufferSize);

            sg_d3dDevice->CreateCommittedResource(&defaultHeapProps, D3D12_HEAP_FLAG_NONE, &defaultResourceDesc,
                                                  D3D12_RESOURCE_STATE_COPY_DEST, nullptr, IID_PPV_ARGS(&_vBuffer));
            _vBuffer->SetName(L"VertexBuffer DefaultResourceHeap");

            // CreateUploadHeap
            D3D12_HEAP_PROPERTIES uploadHeapProps = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
            D3D12_RESOURCE_DESC uploadResoourceDesc = CD3DX12_RESOURCE_DESC::Buffer(_vBufferSize);

            sg_d3dDevice->CreateCommittedResource(&uploadHeapProps, D3D12_HEAP_FLAG_NONE, &uploadResoourceDesc,
                                                  D3D12_RESOURCE_STATE_GENERIC_READ, nullptr,
                                                  IID_PPV_ARGS(&_vBufferUploadHeap));
            _vBufferUploadHeap->SetName(L"VertexBuffer UploadeResourceHeap");
        }

        // Index Buffer
        {
            // Create default heap
            D3D12_HEAP_PROPERTIES defaultHeapProps = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
            D3D12_RESOURCE_DESC defaultResoourceDesc = CD3DX12_RESOURCE_DESC::Buffer(iBufferSize);
            sg_d3dDevice->CreateCommittedResource(&defaultHeapProps, D3D12_HEAP_FLAG_NONE, &defaultResoourceDesc,
                                                  D3D12_RESOURCE_STATE_COPY_DEST, nullptr,
                                                  IID_PPV_ARGS(_iBuffer.GetAddressOf()));
            _iBuffer->SetName(L"Index Buffer Upload Resource Heap");

            // Create upload heap
            D3D12_HEAP_PROPERTIES uploadHeapProps = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
            D3D12_RESOURCE_DESC uploadResoourceDesc = CD3DX12_RESOURCE_DESC::Buffer(_vBufferSize);
            sg_d3dDevice->CreateCommittedResource(&uploadHeapProps, D3D12_HEAP_FLAG_NONE, &uploadResoourceDesc,
                                                  D3D12_RESOURCE_STATE_GENERIC_READ, nullptr,
                                                  IID_PPV_ARGS(_iBufferUploadHeap.GetAddressOf()));
            _iBufferUploadHeap->SetName(L"Index Buffer Upload Resource Heap");
        }

        D3D12_SUBRESOURCE_DATA vData = {};
        vData.pData = reinterpret_cast<BYTE*>(sDataPtr->GetVertexPointer());
        vData.RowPitch = _vBufferSize;
        vData.SlicePitch = _vBufferSize;

        D3D12_SUBRESOURCE_DATA iData = {};
        iData.pData = reinterpret_cast<BYTE*>(sDataPtr->GetIndexPointer());
        iData.RowPitch = iBufferSize;
        iData.SlicePitch = iBufferSize;
  
        auto commandList = sg_d3dDriver->GetCommandList();
        auto commandQueue = sg_d3dDriver->GetCommandQueue();

        UpdateSubresources(commandList.Get(), _vBuffer.Get(), _vBufferUploadHeap.Get(), 0, 0, 1, &vData);
        UpdateSubresources(commandList.Get(), _iBuffer.Get(), _iBufferUploadHeap.Get(), 0, 0, 1, &iData);

        _vBufferView.BufferLocation = _vBuffer->GetGPUVirtualAddress();
        _vBufferView.StrideInBytes = sizeof(DxVertex);
        _vBufferView.SizeInBytes = _vBufferSize;

        _iBufferView.BufferLocation = _iBuffer->GetGPUVirtualAddress();
        _iBufferView.Format = DXGI_FORMAT_R32_UINT;
        _iBufferView.SizeInBytes = iBufferSize;

        D3D12_RESOURCE_BARRIER barriers[] = {
            CD3DX12_RESOURCE_BARRIER::Transition(_vBuffer.Get(), D3D12_RESOURCE_STATE_COPY_DEST,
                                                 D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER),
            CD3DX12_RESOURCE_BARRIER::Transition(_iBuffer.Get(), D3D12_RESOURCE_STATE_COPY_DEST,
                                                 D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER)};

        commandList->ResourceBarrier(_countof(barriers), barriers);
        commandList->Close();

        ID3D12CommandList* ppCmdLists2[] = {commandList.Get()};
        commandQueue->ExecuteCommandLists(_countof(ppCmdLists2), ppCmdLists2);

        sg_d3dDriver->UpdateFenceValue();
    }
} // namespace sb