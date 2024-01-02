#pragma once

#include "Core/Common.h"

namespace sb
{
    class OpenGLBuffer
    {
    public:
        ~OpenGLBuffer();
        static UPtr<OpenGLBuffer> CreateWithData(uint32 in_bufferType, uint32 in_usage, const void* in_data,
                                                 size_t in_dataSize);

        void Bind() const;

        uint32 Get() const { return m_buffer; }

    private:
        OpenGLBuffer() = default;
        void Init(uint32 in_bufferType, uint32 in_usage, const void* in_data, size_t in_dataSize);

        uint32 m_buffer = 0;
        uint32 m_bufferType = 0;
        uint32 m_usage = 0;
    };

    class OpenGLVertexBuffer
    {
    public:
        static UPtr<OpenGLVertexBuffer> Create();
        OpenGLVertexBuffer() = default;
        ~OpenGLVertexBuffer();

        int32 Get() const { return m_vertexArrayObject; }

        void Bind() const; // override?
        void SetAttribute(int32 in_attribIndex, int in_count, int32 in_type, bool in_normalized, size_t in_stride,
                          uint64_t in_offset) const; // override?

    private:
        void Init();

        uint32 m_vertexArrayObject = 0;
    };
} // namespace sb