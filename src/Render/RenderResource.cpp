#include "RenderResource.h"
#include "GraphicsContext.h"
#include "Opengl/OpenGLContext.h"
#include "spdlog/spdlog.h"
#include "BasicGeometry.h"

namespace sb
{
    void StaticMeshRenderResource::RenderProcess(GraphicsContext* in_processor)
    {
        if (GraphicsLibrary::OpenGL == in_processor->m_graphicsFramework)
        {
            OpenglRenderProcess_Internal(static_cast<OpenglContext*>(in_processor));
        }
    }

    void StaticMeshRenderResource::OpenglRenderProcess_Internal(OpenglContext* in_openglContext)
    {
        auto result =
            in_openglContext->m_staticMeshData.insert(std::make_pair(m_assetName, OpenglStaticMeshData()));

        if (!result.second)
        {
            spdlog::error("Failed to insert staticMeshData, name:{}", m_assetName);
            return;
        }

        OpenglStaticMeshData& staticMeshData = result.first->second;
        // glGenVertexArrays(1, &staticMeshData.m_vertexDeclationId);
        // glBindVertexArray(staticMeshData.m_vertexDeclationId);

        // vao
        staticMeshData.m_vertexDeclaration = new OpenglVertexBuffer();

        // geometry
        staticMeshData.m_vertexObjectBuffer = new OpenglObjectBuffer();
        staticMeshData.m_vertexObjectBuffer->BindBuffer(GL_ARRAY_BUFFER);
        staticMeshData.m_vertexObjectBuffer->AddData(cubeVertices);
        staticMeshData.m_vertexObjectBuffer->CommitData(GL_STATIC_DRAW);

        staticMeshData.m_vertexDeclaration->SetAttribute(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), 0);

        // ebo
        staticMeshData.m_indexBuffer = new OpenglObjectBuffer();
        staticMeshData.m_indexBuffer->BindBuffer(GL_ELEMENT_ARRAY_BUFFER);
        staticMeshData.m_indexBuffer->AddData(cubeVertexIndex);
        staticMeshData.m_indexBuffer->CommitData(GL_STATIC_DRAW);

        // texCoord
        staticMeshData.m_TexCoordBuffer = new OpenglObjectBuffer();
        staticMeshData.m_TexCoordBuffer->BindBuffer(GL_ARRAY_BUFFER);
        staticMeshData.m_TexCoordBuffer->AddData(cubeUVs);
        staticMeshData.m_TexCoordBuffer->CommitData(GL_STATIC_DRAW);

        staticMeshData.m_vertexDeclaration->SetAttribute(1, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), 0);
        staticMeshData.m_vertexDeclaration->SetAttribute(2, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), 0);
    }

    void VertexRenderResource::RenderProcess(GraphicsContext* in_processor)
    {
    }
}; // namespace sb