#include "OpenglBuffer.h"

#include <glad/gl.h>
#include <assert.h>
#include <iostream>
#include <spdlog/spdlog.h>

namespace sb
{
    /** OpenglBuffer **/
    OpenglBuffer::OpenglBuffer(const uint32 in_bufferSize)
    {
        glGenBuffers(1, &m_bufferId);
        m_byteBuffer.reserve(in_bufferSize);
    }

    OpenglBuffer::~OpenglBuffer()
    {
        if (m_bufferId)
        {
            glDeleteBuffers(1, &m_bufferId);
        }
    }

    void OpenglBuffer::BindBuffer(const GLenum in_bufferType)
    {
        if (!IsBufferCreated())
        {
            spdlog::warn("OpenglBuffer already created.");
            return;
        }

        glBindBuffer(in_bufferType, m_bufferId);
        m_targetBufferType = in_bufferType;
    }

    void OpenglBuffer::CommitData(const GLenum in_usage)
    {
        if (!IsBufferCreated())
        {
            spdlog::warn("OpenglBuffer already created.");
            return;
        }

        glBufferData(m_targetBufferType, m_byteSize, m_byteBuffer.data(), in_usage);
        m_usage = in_usage;
        m_uploadedByteSize = m_byteSize;
        m_byteSize = 0;
    }

    void OpenglBuffer::AddData_Internal(const void* in_ptrData, uint32 in_byteSize, uint32 in_repeat)
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

    void OpenglVertexBuffer::BindBuffer(const GLenum in_bufferType)
    {
        OpenglBuffer::BindBuffer(in_bufferType);
    }
    /** ~OpenglVertexBuffer **/

    /** OpenglObjectBuffer **/
    void OpenglVertexBuffer::SetAttribute(int32 in_attribIndex, int in_count, int32 in_type, bool in_normalized,
                                          size_t in_stride, uint64_t in_offset) const
    {
        glEnableVertexAttribArray(in_attribIndex);
        glVertexAttribPointer(in_attribIndex, in_count, in_type, in_normalized, in_stride, (const void*)in_offset);
    }

    OpenglObjectBuffer::OpenglObjectBuffer(const uint32 in_bufferSize)
    : OpenglBuffer(in_bufferSize)
    {
    }

    OpenglObjectBuffer::~OpenglObjectBuffer()
    {
    }

    void OpenglObjectBuffer::BindBuffer(const GLenum in_bufferType)
    {
        OpenglBuffer::BindBuffer(in_bufferType);
    }
    /** ~OpenglObjectBuffer **/


    /** OpenglUniformBuffer **/
    OpenglUniformBuffer::OpenglUniformBuffer(const uint32 in_bufferSize)
    : OpenglBuffer(in_bufferSize)
    {
    }

    OpenglUniformBuffer::~OpenglUniformBuffer()
    {
    }

    void OpenglUniformBuffer::BindBuffer(const GLenum in_bufferType)
    {
        if (!IsBufferCreated())
        {
            spdlog::warn("OpengUniformlBuffer already created.");
            return;
        }

        OpenglBuffer::BindBuffer(in_bufferType);

        glBufferData(in_bufferType, m_byteBuffer.capacity(), NULL, GL_STATIC_DRAW);
        glBindBuffer(in_bufferType, 0);
    }

    void OpenglUniformBuffer::BindToBindingPoint(const GLuint in_bindingPoint)
    {
        if (!IsBufferCreated())
        {
            spdlog::warn("OpenglUniformBuffer doesn't created.");
            return;
        }

        glBindBufferBase(GL_UNIFORM_BUFFER, in_bindingPoint, m_bufferId);
    }
    /** ~OpenglUniformBuffer **/
}