#include "NativeActor.h"
#include "StaticMeshComponent.h"

namespace sb
{
    NativeActor::NativeActor()
    {
        _staticMeshComponent = new StaticMeshComponent;
    }

    void NativeActor::Tick(float delta)
    {
        Actor::Tick(delta);

        Vector3d NewPos = _staticMeshComponent->GetPosition() + Vector3d(0.0, 0.0, 0.1);
        if (NewPos.Z > 3.0)
        {
            NewPos = _staticMeshComponent->GetPosition();
        }

        _staticMeshComponent->SetPosition(NewPos);
    }

} // namespace sb