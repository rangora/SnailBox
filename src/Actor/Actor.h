#pragma once

#include "Core/Math/Vector.h"
#include "Render/Opengl/OpenGLBuffer.h"
#include "Render/RenderResource.h"
#include "Actor/BaseComponent.h"
#include "Core/Common.h"
#include <vector>

namespace sb
{
    class Actor
    {
    public:
        Actor() = default;
        Actor(const Vector3d in_position);
        virtual ~Actor();

        void RegisterComponent(BaseComponent* in_BaseComp);
        void DrawActor();

        Vector3d m_position = Vector3d::zeroVector;

        std::vector<BaseComponent*> m_components;
    };
} // namespace sb