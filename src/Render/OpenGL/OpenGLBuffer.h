#include "OpenGLBuffer.h"

#include <glad/glad.h>

namespace sb
{
    /** OpenGLBuffer **/
    OpenGLBuffer::~OpenGLBuffer()
    {
        if (m_buffer)
        {
            glDeleteBuffers(1, &m_buffer);
        }
    }

    UPtr<OpenGLBuffer> OpenGLBuffer::CreateWithData(uint32 in_bufferType, uint32 in_usage, const void* in_data,
                                                    size_t in_dataSize)
    {
        auto buffer = UPtr<OpenGLBuffer>(new OpenGLBuffer());
        buffer->Init(in_bufferType, in_usage, in_data, in_dataSize) ? std::move(buffer) : nullptr;
    }

    void OpenGLBuffer::Bind() const
    {
        glBindBuffer(m_bufferType, m_buffer);
    }

    void OpenGLBuffer::Init(uint32 in_bufferType, uint32 in_usage, const void* in_data, size_t in_dataSize)
    {
        m_bufferType = in_bufferType;
        m_usage = in_usage;
        glGenBuffers(1, &m_buffer);
        Bind();
        glBufferData(in_bufferType, in_dataSize, in_data, in_usage);
    }
    /** ~OpenGLBuffer **/

    /** OpenGLVertexBuffer **/
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
    /** ~OpenGLVertexBuffer **/
}