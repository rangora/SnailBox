#include "StaticMeshComponent.h"


namespace sb
{
    StaticMeshComponent::StaticMeshComponent()
    {
        UpdateShaderReousourceKey("sample4");
    }

    void StaticMeshComponent::Tick(float delta)
    {
        SceneComponent::Tick(delta);
        _bRenderMarkDirty = true;
    }
} // namespace sb