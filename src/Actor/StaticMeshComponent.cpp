#include "StaticMeshComponent.h"
#include "Core/Application.h"
#include "Core/Window.h"

namespace sb
{
    StaticMeshComponent::StaticMeshComponent(Actor* in_parentActor)
    : BaseComponent(in_parentActor)
    {
        m_name = "CubeComp";
        m_sm_renderResource.m_assetName = "Cube";
        // m_sm_renderResource.m_vertexDeclaration = CreateUPtr<OpenglVertexBuffer>();
        // m_sm_renderResource.m_vertexBuffer = CreateSPtr<OpenglObjectBuffer>();
        // m_sm_renderResource.m_indexBuffer = CreateSPtr<OpenglObjectBuffer>();
        // m_sm_renderResource.m_texCoords = CreateSPtr<OpenglObjectBuffer>();
        // m_sm_renderResource.m_normalBuffer = CreateSPtr<OpenglObjectBuffer>();
    }

    void StaticMeshComponent::Draw()
    {
        Application& app = Application::Get();
        Window& Window = app.GetOpenglWindow();
        Window.m_graphicContext->m_targetRenderResources.emplace_back(&m_sm_renderResource);
    }
} // namespace sb
