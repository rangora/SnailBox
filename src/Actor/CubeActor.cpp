#include "CubeActor.h"
#include "StaticMeshComponent.h"
#include "BaseComponent.h"
#include "Render/BasicGeometry.h"
#include "Enums.h"

namespace sb
{
    CubeActor::CubeActor()
    {
        StaticMeshComp = new StaticMeshComponent(this);

        auto uniqueComponentPtr = CreateUPtr<StaticMeshComponent>(this);
        RegisterComponent(std::move(uniqueComponentPtr));
    }

    void CubeActor::Tick(float in_delta)
    {
        Actor::Tick(in_delta);

    
    }
} // namespace sb