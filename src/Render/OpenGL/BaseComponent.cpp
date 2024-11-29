#include "BaseComponent.h"
#include "Actor.h"
#include "spdlog/spdlog.h"

namespace sb
{
    BaseComponent::BaseComponent(Actor* in_parentActor)
    : ParentActor(in_parentActor)
    {
    }

    BaseComponent::~BaseComponent()
    {
    }

    void BaseComponent::Draw()
    {
    }

    Vector3d BaseComponent::GetWorldLocation()
    {
        if (ParentActor != nullptr)
        {
            return ParentActor->GetActorLocation() + m_trnasform.m_translation;
        }

        spdlog::error("Invalid parent actor during getWorldLocation.");
        return m_trnasform.m_translation;
    }

    Vector3d BaseComponent::GetRelativeLocation()
    {
        return m_trnasform.m_translation;
    }
} // namespace sb
