#include "OpenGLBuffer.h"

#include <iostream>
#include <glad/glad.h>

namespace sb
{
    /** OpenGLBuffer **/
    OpenGLBuffer::~OpenGLBuffer()
    {
        if (m_bufferId)
        {
            glDeleteBuffers(1, &m_bufferId);
        }
    }

    void OpenGLBuffer::CreateBuffer(const OpenglBufferType in_type)
    {
        if (IsBufferCreated())
        {
            // TODO : Log
            return;
        }

        switch (in_type)
        {
            case OpenglBufferType::VAO:
                break;

            case OpenglBufferType::VBO:
            {
                CreateVBO(512);
                break;
            }
            case OpenglBufferType::EBO:
            {
                glGenBuffers(1, &m_bufferId);
                m_byteBuffer.reserve(512);
                break;
            }
            case OpenglBufferType::None:
            default:
                // TODO : Assert & log
                break;
        }
    }

    void OpenGLBuffer::CreateVBO(const int32 in_ByteSize)
    {
        glGenBuffers(1, &m_bufferId);
        m_byteBuffer.reserve(in_ByteSize > 0 ? in_ByteSize : 512);
    }

    void OpenGLBuffer::BindBuffer(const GLenum in_bufferType)
    {
        if (!IsBufferCreated())
        {
            // TODO : Log
            return;
        }

        glBindBuffer(in_bufferType, m_bufferId);
        m_targetBufferType = in_bufferType;
    }

    void OpenGLBuffer::CommitData(const GLenum in_usage)
    {
        if (!IsBufferCreated())
        {
            // TODO : Log
            return;
        }

        glBufferData(m_targetBufferType, m_byteSize, m_byteBuffer.data(), in_usage);
        m_usage = in_usage;
        m_uploadedByteSize = m_byteSize;
        m_byteSize = 0;

        GLenum err;
        while ((err = glGetError()) != GL_NO_ERROR)
        {
            // 에러 처리
            std::cerr << "OpenGL error: " << err << std::endl;
        }
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

    /** OpenglVertexBuffer **/
    OpenglVertexBuffer::OpenglVertexBuffer()
    {
        glGenVertexArrays(1, &m_bufferId);
        glBindVertexArray(m_bufferId);
    }

    OpenglVertexBuffer::~OpenglVertexBuffer()
    {
    }
    void OpenglVertexBuffer::SetAttribute(int32 in_attribIndex, int in_count, int32 in_type, bool in_normalized,
                                          size_t in_stride, uint64_t in_offset) const
    {
        glEnableVertexAttribArray(in_attribIndex);
        glVertexAttribPointer(in_attribIndex, in_count, in_type, in_normalized, in_stride, (const void*)in_offset);
    }
    /** ~OpenglVertexBuffer **/
}