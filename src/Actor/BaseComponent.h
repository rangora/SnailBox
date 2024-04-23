#pragma once

#include "Core/Common.h"
#include "Core/Math/Vector.h"

namespace sb
{
    class Actor;

    class BaseComponent
    {
    public:
        BaseComponent() = delete;
        BaseComponent(Actor* in_parentActor);

        virtual void Draw() = 0;

        Vector3d m_position = Vector3d::zeroVector;

        SPtr<Actor> ParentActor = nullptr;
    };
} // namespace sb