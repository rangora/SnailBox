#pragma once

#include "Core/Common.h"

namespace sb
{
    class OpenGLVertexBuffer
    {
    public:
        ~OpenGLVertexBuffer();
        static UPtr<OpenGLVertexBuffer> Create();

        int32 Get() const { return m_vertexArrayObject; }

        void Bind() const; // override?
        void SetAttribute(int32 in_attribIndex, int in_count, int32 in_type, bool in_normalized, size_t in_stride,
                          uint64_t in_offset) const; // override?

private:
    OpenGLVertexBuffer() = default;
    void Init();

    int32 m_vertexArrayObject = 0;
    };
} // namespace sb