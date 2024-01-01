#include "OpenGLBuffer.h"

#include <glad/glad.h>

namespace sb
{
    inline OpenGLVertexBuffer::~OpenGLVertexBuffer()
    {
        if (m_vertexArrayObject)
        {
            glDeleteVertexArrays(1, &m_vertexArrayObject);
        }
    }

    inline UPtr<OpenGLVertexBuffer> OpenGLVertexBuffer::Create()
    {
        auto VertexBuffer = UPtr<OpenGLVertexBuffer>(new OpenGLVertexBuffer());
        VertexBuffer->Init();
        return std::move(VertexBuffer);
    }

    void OpenGLVertexBuffer::Bind() const
    {
        glBindVertexArray(m_vertexArrayObject);
    }

    void OpenGLVertexBuffer::SetAttribute(int32 in_attribIndex, int in_count, int32 in_type, bool in_normalized,
                                          size_t in_stride, uint64_t in_offset) const
    {
        glEnableVertexAttribArray(in_attribIndex);
        glVertexAttribPointer(in_attribIndex, in_count, in_type, in_normalized, in_stride, (const void*)in_offset);
    }

    void OpenGLVertexBuffer::Init()
    {
        glGenVertexArrays(1, &m_vertexArrayObject);
        Bind();
    }
}