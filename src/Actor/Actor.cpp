#include "Actor.h"
#include "spdlog/spdlog.h"

namespace sb
{
    Actor::Actor(const Vector3d in_position) : m_position(in_position)
    {}

    Actor::~Actor()
    {
    }

    void Actor::Tick(float in_delta)
    {
    

    }

    void Actor::RegisterComponent(UPtr<BaseComponent> in_BaseComp)
    {
        m_components.emplace_back(std::move(in_BaseComp));
    }

    void Actor::DrawActor()
    {
        for (const auto& Comp : m_components)
        {
            Comp->Draw();
        }
    }

    void Actor::SetActorLocation(const Vector3d in_location)
    {
        m_transform.m_translation = in_location;
    }

    void Actor::SetActorRotation(const Vector3d in_rotation)
    {
        m_transform.m_rotation = in_rotation;
    }

    void Actor::SetActorLocationAndRotation(const Vector3d in_location, const Vector3d in_rotation)
    {
        m_transform.m_translation = in_location;
        m_transform.m_rotation = in_rotation;
    }

} // namespace sb