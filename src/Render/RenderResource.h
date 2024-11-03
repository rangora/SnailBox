#pragma once

#include "corepch.h"
#include "Core/Math/Vector.h"
#include "Enums.h"
#include "Opengl/OpenGLBuffer.h"
#include "Opengl/OpenGLContext.h"
#include "RenderBuffer.h"
#include <map>
#include <vector>

namespace sb
{
    class GraphicsContext;

    class RenderResource
    {
    public:
        virtual void RenderProcess(GraphicsContext* in_processor) = 0;

        std::string m_assetName;
    };

    class VertexRenderResource : public RenderResource
    {
    public:
        void RenderProcess(GraphicsContext* in_processor) override;

        // std::vector<Vector3f> m_vertices;
        // std::vector<Vector3f> m_normals;
        // std::vector<Vector2f> m_texCoords;
        // std::vector<uint32> m_index;

        RenderBuffer m_RenderBuffer;
    };

    class StaticMeshRenderResource : public RenderResource
    {
    public:
        void RenderProcess(GraphicsContext* in_processor) override;

        // void AddVertexRenderResource(const VertexBufferType in_type, const void* in_data);

        // std::map<VertexBufferType, SPtr<VertexRenderResource>> VertexRenderResources;
        // SPtr<VertexRenderResource> VertexLayout;

        UPtr<RenderBuffer> m_vertexDeclaration;
        SPtr<RenderBuffer> m_vertexBuffer;
        SPtr<RenderBuffer> m_indexBuffer;
        SPtr<RenderBuffer> m_normalBuffer;
        SPtr<RenderBuffer> m_texCoords;

        uint32 m_vertexCount = 0;
        uint32 m_indexCount = 0;

    private:
        void OpenglRenderProcess_Internal(class OpenglContext* in_openglContext);
    };
} // namespace sb