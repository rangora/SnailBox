#include "Actor.h"

namespace sb
{
    Actor::Actor(const Vector3d in_position) : m_position(in_position)
    {}

    Actor::~Actor()
    {
    }

    void Actor::RegisterComponent(BaseComponent* in_BaseComp)
    {
        m_components.emplace_back(in_BaseComp);
    }

    void Actor::DrawActor()
    {
        for (const auto& Comp : m_components)
        {
            Comp->Draw();
        }
    }

} // namespace sb