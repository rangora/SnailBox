#pragma once

#include "Actor.h"
#include "corepch.h"

namespace sb
{
    class NativeActor : public Actor
    {
    public:
        NativeActor();

        void Tick(float delta) final;

        class StaticMeshComponent* _staticMeshComponent = nullptr;
    };
} // namespace sb