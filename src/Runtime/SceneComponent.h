#pragma once

#include "coreMinimal.h"
#include "RenderProxy.h"

namespace sb
{
    class World;
    class Actor;

    class SceneComponent
    {
    public:
        SceneComponent();
        SceneComponent(Actor* owner);
        ~SceneComponent();

        virtual void Tick(float delta);

        World* GetWorld() const { return _world; }

        void SetPosition(const Vector3d position);
        void SetRotation(const Vector3d rotation);
        void SetScale(const Vector3d scale);

        Vector3d GetPosition() const { return _transform.m_translation; }
        Vector3d GetRotation() const { return _transform.m_rotation; }
        Vector3d GetScale() const { return _transform.m_scale; }
        Transform GetTransform() const { return _transform; }

        RenderProxy* GetRenderProxy() const { return _renderProxy; }

        void UpdateShaderReousourceKey(std::string keyName); // °³¼± ÇÊ

        bool _bRenderMarkDirty = false;

    private:
        void MarkForRenderUpdate();

        RenderProxy* _renderProxy = nullptr;
        World* _world = nullptr;
        Actor* _owner = nullptr;

        Transform _transform;
    };
} // namespace sb