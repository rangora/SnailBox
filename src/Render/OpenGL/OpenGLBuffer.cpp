#include "OpenGLBuffer.h"

#include <iostream>
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
        buffer->Init(in_bufferType, in_usage, in_data, in_dataSize);
        return std::move(buffer);
    }

    void OpenGLBuffer::CreateVBO(const int32 in_ByteSize)
    {
        if (IsBufferCreated())
        {
            // TODO : Log
            return;
        }

        glGenBuffers(1, &m_buffer);
        m_byteBuffer.reserve(in_ByteSize > 0 ? in_ByteSize : 512);
    }

    void OpenGLBuffer::BindVBO(const GLenum in_bufferType)
    {
        if (!IsBufferCreated())
        {
            // TODO : Log
            return;
        }

        m_bufferType = in_bufferType;
        glBindBuffer(m_bufferType, m_buffer);
    }
    
    void OpenGLBuffer::CommitData(const GLenum in_usage)
    {
        if (!IsBufferCreated())
        {
            // TODO : Log
            return;
        }

        glBufferData(m_bufferType, m_byteSize, m_byteBuffer.data(), in_usage);
        m_uploadedByteSize = m_byteSize;
        m_byteSize = 0;

        GLenum err;
        while ((err = glGetError()) != GL_NO_ERROR)
        {
            // 에러 처리
            std::cerr << "OpenGL error: " << err << std::endl;
        }
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
    
    void OpenGLBuffer::AddData_Internal(const void* in_ptrData, uint32 in_byteSize, uint32 in_repeat)
    {
        const uint32 bytesToAdd = in_byteSize * in_repeat;
        const uint32 ReqCapacity = m_byteSize + bytesToAdd;

        if (ReqCapacity > m_byteBuffer.capacity())
        {
            // 우선 딱 맞게..
            // 보통은 좀 크게 잡을 듯
            uint32 newCapacity = ReqCapacity;
            std::vector<unsigned char> newByteBuffer;
            newByteBuffer.reserve(newCapacity);
            memcpy(newByteBuffer.data(), m_byteBuffer.data(), bytesToAdd);
            m_byteBuffer = std::move(newByteBuffer);
        }

        for (int32 i = 0; i < in_repeat; ++i)
        {
            memcpy(m_byteBuffer.data() + m_byteSize, in_ptrData, in_byteSize);
            m_byteSize += in_byteSize;
        }
    }

    /** ~OpenGLBuffer **/

    /** OpenGLVertexBuffer **/
    OpenGLVertexBuffer::~OpenGLVertexBuffer()
    {
        if (m_vertexArrayObject)
        {
            glDeleteVertexArrays(1, &m_vertexArrayObject);
        }
    }

    UPtr<OpenGLVertexBuffer> OpenGLVertexBuffer::Create()
    {
        auto VertexBuffer = CreateUPtr<OpenGLVertexBuffer>();
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