#pragma once

namespace sb
{
    enum class GraphicsLibrary : uint8
    {
        None,
        OpenGL,
        DirectX12,
    };

    enum class OpenglBufferType : uint8
    {
        None,
        VAO,
        VBO,
        EBO
    };
}; // namespace sb