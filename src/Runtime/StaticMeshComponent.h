#pragma once

#include "SceneComponent.h"
#include "StaticMeshRenderProxy.h"

namespace sb
{
    class StaticMeshComponent : public SceneComponent
    {
    public:
        StaticMeshComponent();

        void Tick(float delta) final;

    private:
    };
} // namespace sb