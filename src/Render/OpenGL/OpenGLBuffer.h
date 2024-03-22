#pragma once

#include "Core/Common.h"
#include <vector>
#include <glad/glad.h>

namespace sb
{
    class OpenGLBuffer
    {
    public:
        OpenGLBuffer() = default;
        ~OpenGLBuffer();
        static UPtr<OpenGLBuffer> CreateWithData(uint32 in_bufferType, uint32 in_usage, const void* in_data,
                                                 size_t in_dataSize);

        void CreateVBO(const int32 in_ByteSize);
        void BindVBO(const GLenum in_bufferType);
        void CommitData(const GLenum in_usage);

        template<typename T>
        void AddData(const T& in_ptrObj, uint32 in_repeat)
        {
            AddData_Internal(in_ptrObj, static_cast<uint32>(sizeof(T)), in_repeat);
        }

        void Bind() const;

        uint32 Get() const { return m_buffer; }

    private:
        void Init(uint32 in_bufferType, uint32 in_usage, const void* in_data, size_t in_dataSize);
        bool IsBufferCreated() const { return m_buffer != 0; }
        void AddData_Internal(const void* in_ptrData, uint32 in_byteSize, uint32 in_repeat);

        std::vector<unsigned char> m_byteBuffer;
        GLuint m_buffer = 0;
        GLenum m_bufferType = 0;
        uint32 m_usage = 0;
        uint32 m_byteSize = 0;
        uint32 m_uploadedByteSize = 0;
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