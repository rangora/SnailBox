#pragma once

#include <glad/gl.h>
#include "Core/Common.h"
#include "Render/RenderBuffer.h"
#include <Enums.h>
#include <vector>

namespace sb
{
    // 임시
    struct OpenglBufferParams
    {
        int32 item = 0;
    };

    class OpenglBuffer : public RenderBuffer
    {
    public:
        virtual ~OpenglBuffer();

        virtual void BindBuffer(const GLenum in_bufferType);
        virtual void CommitData(const GLenum in_usage);

        template<typename T>
        void AddData(const T& in_ptrObj, uint32 in_repeat = 1)
        {
            AddData_Internal(in_ptrObj, static_cast<uint32>(sizeof(T)), in_repeat);
        }

        uint32 Get() const { return m_bufferId; }

    
    protected:
        OpenglBuffer() = default;
        OpenglBuffer(const uint32 in_bufferSize);

        virtual void PostCommitData() {}

        bool IsBufferCreated() const { return m_bufferId != 0; }
        void AddData_Internal(const void* in_ptrData, uint32 in_byteSize, uint32 in_repeat);

        OpenglBufferParams m_bufferParams;
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
        ~OpenglVertexBuffer() final;

        void BindBuffer(const GLenum in_bufferType) final;
        void CommitData(const GLenum in_usage) final;

        void SetAttribute(int32 in_attribIndex, int in_count, int32 in_type, bool in_normalized, size_t in_stride,
                          uint64_t in_offset) const;
    };

    class OpenglObjectBuffer : public OpenglBuffer
    {
    public:
        OpenglObjectBuffer(const uint32 in_bufferSize = 512);
        ~OpenglObjectBuffer() final;

        void BindBuffer(const GLenum in_bufferType) final;
        void CommitData(const GLenum in_usage) final;
    };

    class OpenglUniformBuffer : public OpenglBuffer
    {
    public:
        OpenglUniformBuffer(const uint32 in_bufferSize = 512);
        ~OpenglUniformBuffer() final;

        void BindBuffer(const GLenum in_bufferType) final;
        void CommitData(const GLenum in_usage) final;

    private:
        void PostCommitData() final;
    };
} // namespace sb