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
        spdlog::info("tickDelta:{}", in_delta);
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

} // namespace sb