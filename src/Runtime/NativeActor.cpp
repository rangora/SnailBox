#include "NativeActor.h"
#include "StaticMeshComponent.h"

namespace sb
{
    NativeActor::NativeActor()
    {
        _staticMeshComponent = new StaticMeshComponent;
    }

    void NativeActor::Tick(float dleta)
    {
        _staticMeshComponent->SetPosition(Vector3d(0.0, 0.0, 6.0));
    }

} // namespace sb