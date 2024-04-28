#pragma once

#include "BaseComponent.h"
#include "Core/Common.h"
#include "Core/Math/Vector.h"
#include "Render/Opengl/OpenGLBuffer.h"
#include "Render/RenderResource.h"
#include <vector>

namespace sb
{
    class Actor
    {
    public:
        Actor() = default;
        Actor(const Vector3d in_position);
        virtual ~Actor();

        virtual void Tick(float in_delta);

        void RegisterComponent(UPtr<BaseComponent> in_BaseComp);
        void DrawActor();

        Vector3d m_position = Vector3d::zeroVector;

        std::vector<UPtr<BaseComponent>> m_components;
    };
} // namespace sb