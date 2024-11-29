#pragma once

#include "BaseComponent.h"
#include "Core/Common.h"
#include "Core/Math/Vector.h"
#include "Core/Math/Transform.h"
#include "Render/Opengl/OpenGLBuffer.h"
#include "Render/Opengl/RenderResource.h"
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

        void SetActorLocation(const Vector3d in_location);
        void SetActorRotation(const Vector3d in_rotation);
        void SetActorLocationAndRotation(const Vector3d in_location, const Vector3d in_rotation);
        Vector3d GetActorLocation() const { return m_transform.m_translation; }
        Vector3d GetActorRotation() const { return m_transform.m_rotation; }
        const Transform& GetTransform() const { return m_transform; }

        std::vector<UPtr<BaseComponent>> m_components;

    private:
        Transform m_transform;
    };
} // namespace sb