#include "RootSignature.h"

namespace sb
{
    void RootSignature::Init(ComPtr<ID3D12Device> in_device)
    {
        // 5 대신 상수로 대체..
        CD3DX12_DESCRIPTOR_RANGE ranges[] = { CD3DX12_DESCRIPTOR_RANGE(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 5, 0) };

        CD3DX12_ROOT_PARAMETER param[1];
        param[0].InitAsDescriptorTable(_countof(ranges), ranges);

        D3D12_ROOT_SIGNATURE_DESC sigDesc = CD3DX12_ROOT_SIGNATURE_DESC(_countof(param), param);
        sigDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT; // 입력 조립 단계

        ComPtr<ID3DBlob> blobSignature;
        ComPtr<ID3DBlob> blobError;
        ::D3D12SerializeRootSignature(&sigDesc, D3D_ROOT_SIGNATURE_VERSION_1, &blobSignature, &blobError);
        in_device->CreateRootSignature(0, blobSignature->GetBufferPointer(), blobSignature->GetBufferSize(),
                                       IID_PPV_ARGS(&m_signature));
    };
} // namespace sb