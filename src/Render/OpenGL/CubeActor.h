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

        void Tick(float in_delta) final;

        StaticMeshComponent* StaticMeshComp = nullptr;
    };
} // namespace sb