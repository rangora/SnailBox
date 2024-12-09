#pragma once

#include "Core/Common.h"

namespace sb
{
    enum class GraphicsDevice : uint8
    {
        None,
        OpenGL,
        DirectX12,
    };

    enum class VertexBufferType : uint8
    {
        None,
        VertexBuffer,
        ObjectBuffer,
        IndexBuffer,
    };

    enum class VertexAttribute : uint8
    {
        None,
        Geometry,
        Normal,
        Color,
    };

    enum class RootSignatureType : uint8
    {
        None,
        Descriptor,
        DescriptorTable,
    };

}; // namespace sb