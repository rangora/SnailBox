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

        Vector3d currentRot = GetActorRotation();
        currentRot.X += in_delta * 45.f;
        currentRot.X = std::fmod(currentRot.X, 360.0);
        SetActorRotation(currentRot);
    }
} // namespace sb