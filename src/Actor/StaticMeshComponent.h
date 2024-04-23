#pragma once

#include "BaseComponent.h"
#include "Render/RenderResource.h"

namespace sb
{
    class Actor;

    class StaticMeshComponent : public BaseComponent
    {
    public:
        StaticMeshComponent() = default;
        StaticMeshComponent(Actor* in_parentActor);
        ~StaticMeshComponent(){}

        void Draw() override;

        std::string m_name;
        StaticMeshRenderResource m_sm_renderResource;
    };
} // namespace sb