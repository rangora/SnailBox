#pragma once

#include "Core/Common.h"
#include "Core/Math/Vector.h"

namespace sb
{
    class Actor;

    class BaseComponent
    {
    public:
        BaseComponent() = default;
        BaseComponent(Actor* in_parentActor);
        BaseComponent(const BaseComponent&) = delete;
        BaseComponent& operator=(const BaseComponent&) = delete;
        virtual ~BaseComponent();

        virtual void Draw();

        Vector3d m_position = Vector3d::zeroVector;

        Actor* ParentActor = nullptr;
    };
} // namespace sb