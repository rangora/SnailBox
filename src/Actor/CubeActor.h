#pragma once

#include "Actor.h"

namespace sb
{
    class StaticMeshComponent;

    class CubeActor : public Actor
    {
    public:
        CubeActor();
        ~CubeActor() = default;

        StaticMeshComponent* StaticMeshComp = nullptr;
    };
} // namespace sb