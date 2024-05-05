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
    }

    void StaticMeshComponent::Draw()
    {
        Application& app = Application::Get();
        Window& Window = app.GetOpenglWindow();

        // TEMP
        Window.m_canvas->GetGraphcisContext()->m_targetRenderResources.emplace_back(&m_sm_renderResource);
    }
} // namespace sb
