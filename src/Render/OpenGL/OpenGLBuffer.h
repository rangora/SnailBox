﻿#pragma once

#include "Core/Common.h"
#include <Enums.h>
#include <glad/glad.h>
#include <vector>

namespace sb
{
    class OpenglBuffer
    {
    public:
        virtual ~OpenglBuffer();

        void BindBuffer(const GLenum in_bufferType);
        void CommitData(const GLenum in_usage);

        template<typename T>
        void AddData(const T& in_ptrObj, uint32 in_repeat = 1)
        {
            AddData_Internal(in_ptrObj, static_cast<uint32>(sizeof(T)), in_repeat);
        }

        uint32 Get() const { return m_bufferId; }

    protected:
        OpenglBuffer() = default;
        OpenglBuffer(const uint32 in_bufferSize);
        bool IsBufferCreated() const { return m_bufferId != 0; }
        void AddData_Internal(const void* in_ptrData, uint32 in_byteSize, uint32 in_repeat);

        std::vector<unsigned char> m_byteBuffer;
        GLuint m_bufferId = 0;
        GLenum m_targetBufferType = 0;
        uint32 m_usage = 0;
        uint32 m_byteSize = 0;
        uint32 m_uploadedByteSize = 0;
    };

    class OpenglVertexBuffer : public OpenglBuffer
    {
    public:
        OpenglVertexBuffer();
        ~OpenglVertexBuffer();

        void SetAttribute(int32 in_attribIndex, int in_count, int32 in_type, bool in_normalized, size_t in_stride,
                          uint64_t in_offset) const;
    };

    class OpenglObjectBuffer : public OpenglBuffer
    {
    public:
        OpenglObjectBuffer(const uint32 in_bufferSize = 512);
        ~OpenglObjectBuffer();
    };
} // namespace sb