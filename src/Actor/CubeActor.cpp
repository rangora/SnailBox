#include "CubeActor.h"
#include "StaticMeshComponent.h"
#include "BaseComponent.h"
#include "Render/BasicGeometry.h"
#include "Enums.h"

namespace sb
{
    CubeActor::CubeActor()
    {
        StaticMeshComp = new StaticMeshComponent(this);
        RegisterComponent(StaticMeshComp);

        {
        // NewVetexRenderResource->m_vertices.assign(cubeVertices, cubeVertices + sizeof(cubeVertices) / sizeof(Vector3f));
        // NewVetexRenderResource->m_normals.assign(cubeNormals, cubeNormals + sizeof(cubeNormals) / sizeof(Vector3f));
        // NewVetexRenderResource->m_texCoords.assign(cubeUVs, cubeUVs + sizeof(cubeUVs) / sizeof(Vector2f));
        // NewVetexRenderResource->m_index.assign(cubeVertexIndex, cubeVertexIndex + sizeof(cubeVertexIndex) / sizeof(uint32));
        }

        // StaticMeshComp->m_sm_renderResource.m_vertexBuffer->m_data = cubeVertices;
        // StaticMeshComp->m_sm_renderResource.m_vertexBuffer->m_attribute = VertexAttribute::Geometry;

        // StaticMeshComp->m_sm_renderResource.m_vertexBuffer->m_data = cubeNormals;
        // StaticMeshComp->m_sm_renderResource.m_vertexBuffer->m_attribute = VertexAttribute::Normal;

        // StaticMeshComp->m_sm_renderResource.m_texCoords->m_data = cubeUVs;
        // StaticMeshComp->m_sm_renderResource.m_indexBuffer->m_data = cubeVertexIndex;
    }
}