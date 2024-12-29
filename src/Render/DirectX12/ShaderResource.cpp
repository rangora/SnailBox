#include "ShaderResource.h"
#include "Core/Application.h"
#include "Render/ShaderGeometry.h"

namespace sb
{
    ShaderResource::ShaderResource(const ShaderResourceDesc& initializeData,
                                   const ShaderHeapInstruction& instruction)
    {
        // create desc for CBV.
        if (instruction._rangeType == D3D12_DESCRIPTOR_RANGE_TYPE_CBV)
        {
            D3D12_DESCRIPTOR_HEAP_DESC desc = {};
            desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
            desc.NumDescriptors = 1;
            desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
            if (sg_d3dDevice->CreateDescriptorHeap(&desc, IID_PPV_ARGS(_cbvHeap.GetAddressOf())) != S_OK)
            {
                spdlog::error("Failed to create constant buffer heap");
                return;
            }
        }

        // create root signature.
        CD3DX12_ROOT_SIGNATURE_DESC rdc;
        if (instruction._rootSignType == RootSignatureType::Descriptor)
        {
            D3D12_ROOT_DESCRIPTOR rootCBVDescriptor;
            rootCBVDescriptor.RegisterSpace = 0;
            rootCBVDescriptor.ShaderRegister = 0;

            D3D12_ROOT_PARAMETER rootParameters[1];
            rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
            rootParameters[0].Descriptor = rootCBVDescriptor;
            rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;

            rdc.Init(_countof(rootParameters), rootParameters, 0, nullptr,
                                   D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT |
                                       D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS |
                                       D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS |
                                       D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS |
                                       D3D12_ROOT_SIGNATURE_FLAG_DENY_PIXEL_SHADER_ROOT_ACCESS);


        }
        else if (instruction._bTable)
        {
            D3D12_DESCRIPTOR_RANGE descTableRanges[1]; // TODO : PARAM??
            descTableRanges[0].RangeType = instruction._rangeType;
            descTableRanges[0].NumDescriptors = instruction._numDescriptors;
            descTableRanges[0].BaseShaderRegister = 0;
            descTableRanges[0].RegisterSpace = 0;
            descTableRanges[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

            D3D12_ROOT_DESCRIPTOR_TABLE descTable;
            descTable.NumDescriptorRanges = _countof(descTableRanges);
            descTable.pDescriptorRanges = &descTableRanges[0];

            D3D12_ROOT_PARAMETER rootParams[1];
            rootParams[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
            rootParams[0].DescriptorTable = descTable;
            rootParams[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;

            rdc.Init(_countof(rootParams), rootParams, 0, nullptr,
                      D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT |
                          D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS |
                          D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS |
                          D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS |
                          D3D12_ROOT_SIGNATURE_FLAG_DENY_PIXEL_SHADER_ROOT_ACCESS);
        }
        else
        {
            rdc.Init(0, nullptr, 0, nullptr, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);
        }

        ID3DBlob* signature;
        HRESULT hr = D3D12SerializeRootSignature(&rdc, D3D_ROOT_SIGNATURE_VERSION_1, &signature, nullptr);
        if (FAILED(hr))
        {
            return;
        }

        hr = sg_d3dDevice->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(),
                                          IID_PPV_ARGS(&_rootSignature));

        if (FAILED(hr))
        {
            return;
        }

        CreateShader(initializeData, instruction);
    }

    ShaderResource::~ShaderResource()
    {
        if (_rootSignature)
        {
            _rootSignature.Reset();
        }

        if (_pipelineState)
        {
            _pipelineState.Reset();
        }

        if (_vBufferUploadHeap)
        {
            _vBufferUploadHeap.Reset();
        }

        if (_vBuffer)
        {
            _vBuffer.Reset();
        }

        if (_iBufferUploadHeap)
        {
            _iBufferUploadHeap.Reset();
        }

        if (_iBuffer)
        {
            _iBuffer.Reset();
        }

        if (_cbvHeap)
        {
            _cbvHeap.Reset();
        }

        if (_cbUploadHeap)
        {
            _cbUploadHeap.Reset();
        }
    }

    void ShaderResource::UpdateShaderRegister(const Transform& transform, const XMMATRIX& vpMatrix)
    {
        int32 frameIndex = sg_d3dDriver->GetCurrentFrameIndex();

        const Vector3d& proxyPos = transform.m_translation;
        const Vector3d& proxyRot = transform.m_rotation;

        XMMATRIX rotMat = XMMatrixRotationRollPitchYaw(proxyRot.X, proxyRot.Y, proxyRot.Z);

        FXMVECTOR posVector{(float)proxyPos.X, (float)proxyPos.Y, (float)proxyPos.Z};
        XMMATRIX translationMat = XMMatrixTranslationFromVector(posVector);
        XMMATRIX worldMat = rotMat * translationMat;
        XMMATRIX transposed = XMMatrixTranspose(worldMat * vpMatrix); // gpu에선 transpose
        XMStoreFloat4x4(&_cbData._wvpMat, transposed);

        memcpy(_cbGPUAddresses[frameIndex], &_cbData, sizeof(_cbData));
    }

    void ShaderResource::Render(ComPtr<ID3D12GraphicsCommandList> commandList)
    {
        ID3D12DescriptorHeap* cbvHeaps[] = {_cbvHeap.Get()};

        commandList->SetGraphicsRootSignature(_rootSignature.Get());

        if (false)
        {
            if (_cbGPUAddress) // TEMP
            {
                commandList->SetDescriptorHeaps(_countof(cbvHeaps), cbvHeaps);
                commandList->SetGraphicsRootDescriptorTable(0, _cbvHeap->GetGPUDescriptorHandleForHeapStart());
            }

            commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
            commandList->IASetVertexBuffers(0, 1, &_vBufferView);
            commandList->IASetIndexBuffer(&_iBufferView);
            commandList->DrawIndexedInstanced(6, 1, 0, 0, 0); // first quad
            commandList->SetPipelineState(_pipelineState.Get());

            {
                commandList->SetGraphicsRootSignature(_rootSignature.Get());
            }
        }
        else
        {
            commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
            commandList->IASetVertexBuffers(0, 1, &_vBufferView);
            commandList->IASetIndexBuffer(&_iBufferView);

            const int32 index = sg_d3dDriver->GetCurrentFrameIndex();
            commandList->SetGraphicsRootConstantBufferView(0, _cbUploadHeaps[index]->GetGPUVirtualAddress());
            commandList->DrawIndexedInstanced(36, 1, 0, 0, 0); // first quad
        }
    }

    void ShaderResource::CreateShader(const ShaderResourceDesc& initializeData,
                                      const ShaderHeapInstruction& instruction)
    {
        auto sDataPtr = sg_d3dDriver->GetShaderData(initializeData._shaderKey);
        if (sDataPtr == nullptr)
        {
            spdlog::error("There is so shaderGeometry data named : {}", initializeData._shaderKey);
            return;
        }

        // Compile shader
        std::string stringVsPath = initializeData._files._vsPath;
        std::wstring wstringVsPath(stringVsPath.size(), L'\0');
        std::mbstowcs(&wstringVsPath[0], stringVsPath.c_str(), stringVsPath.size());
        std::string stringPsPath = initializeData._files._fsPath;
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
        psoDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
        psoDesc.DSVFormat = DXGI_FORMAT_D32_FLOAT; // 필수??

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

        // constant buffer
        if (instruction._rootSignType == RootSignatureType::Descriptor)
        {
            _cbUploadHeaps.resize(SWAP_CHAIN_BUFFER_COUNT);
            _cbGPUAddresses.resize(SWAP_CHAIN_BUFFER_COUNT);


            for (int32 i = 0; i < SWAP_CHAIN_BUFFER_COUNT; ++i)
            {
                CD3DX12_HEAP_PROPERTIES heapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
                CD3DX12_RESOURCE_DESC resourceDesc = CD3DX12_RESOURCE_DESC::Buffer(1024 * 64);

                hr = sg_d3dDevice->CreateCommittedResource(&heapProp, D3D12_HEAP_FLAG_NONE, &resourceDesc,
                                                           D3D12_RESOURCE_STATE_GENERIC_READ, nullptr,
                                                           IID_PPV_ARGS(&_cbUploadHeaps[i]));
                _cbUploadHeaps[i]->SetName(L"Constant Buffer Upload Resource Heap");

                ZeroMemory(&_cbData, sizeof(_cbData));

                CD3DX12_RANGE readRange(0, 0);

                hr = _cbUploadHeaps[i]->Map(0, &readRange, reinterpret_cast<void**>(&_cbGPUAddresses[i]));

                memcpy(_cbGPUAddresses[i], &_cbData, sizeof(_cbData));
                memcpy(_cbGPUAddresses[i] + _cBufferObjectAlignedSize, &_cbData, sizeof(_cbData));
            }
        }

        if (instruction._rangeType == D3D12_DESCRIPTOR_RANGE_TYPE_CBV)
        {
            CD3DX12_HEAP_PROPERTIES heapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
            CD3DX12_RESOURCE_DESC resourceDesc = CD3DX12_RESOURCE_DESC::Buffer(1024 * 64);

            hr = sg_d3dDevice->CreateCommittedResource(&heapProp, D3D12_HEAP_FLAG_NONE, &resourceDesc,
                                                        D3D12_RESOURCE_STATE_GENERIC_READ, nullptr,
                                                        IID_PPV_ARGS(_cbUploadHeap.GetAddressOf()));
            _cbUploadHeap->SetName(L"Constant Buffer Upload Resource Heap");

            if (FAILED(hr))
            {
                assert(false);
                spdlog::error("Failed to create constant buffer upload resource heap");
                return;
            }

            // Create Constant Buffer View
            D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc = {};
            cbvDesc.BufferLocation = _cbUploadHeap->GetGPUVirtualAddress();
            cbvDesc.SizeInBytes = (sizeof(ConstantBuffer) + 255) & ~255;
            sg_d3dDevice->CreateConstantBufferView(&cbvDesc, _cbvHeap->GetCPUDescriptorHandleForHeapStart());

            ZeroMemory(&_cbData, sizeof(_cbData));
            CD3DX12_RANGE readRange(0, 0);
            hr = _cbUploadHeap->Map(0, &readRange, reinterpret_cast<void**>(&_cbGPUAddress));
            ::memcpy(_cbGPUAddress, &_cbData, sizeof(_cbData));
        }

        commandList->Close();

        ID3D12CommandList* ppCmdLists2[] = {commandList.Get()};
        commandQueue->ExecuteCommandLists(_countof(ppCmdLists2), ppCmdLists2);

        sg_d3dDriver->UpdateFenceValue();

        auto ctx = sg_d3dDriver->GetCurrentFrameContexts();
        commandList->Reset(ctx->CommandAllocator, _pipelineState.Get());
    }
} // namespace sb