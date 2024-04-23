#pragma once

#include "Actor.h"

namespace sb
{
    class CubeActor : public Actor
    {
    public:
        CubeActor();
        
        class StaticMeshComponent* StaticMeshComp = nullptr;
    };
} // namespace sb