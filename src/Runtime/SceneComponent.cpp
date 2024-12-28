#include "SceneComponent.h"
#include "RenderProxy.h"
#include "World/Worldinstance.h"
#include "World/World.h"

namespace sb
{
    SceneComponent::SceneComponent()
    {
        _renderProxy = new RenderProxy(this);

        WorldInstance& wInstance = WorldInstance::Get();
        _world = wInstance.GetWorld();
        _world->RegisterComponent(this);
    }

    SceneComponent::SceneComponent(Actor* owner)
    {
    }

    SceneComponent::~SceneComponent()
    {
         delete _renderProxy;
    }

    void SceneComponent::Tick(float delta)
    {
        if (_bRenderMarkDirty)
        {
            MarkForRenderUpdate();
        }
    }

    void SceneComponent::SetPosition(const Vector3d position)
    {
        _transform.SetTranslation(position);
    }

    void SceneComponent::SetRotation(const Vector3d rotation)
    {
        _transform.SetRotation(rotation);
    }

    void SceneComponent::SetScale(const Vector3d scale)
    {
        _transform.SetScale(scale);
    }

    void SceneComponent::UpdateShaderReousourceKey(std::string keyName)
    {

        _renderProxy->_shaderKey = keyName;
    }

    void SceneComponent::MarkForRenderUpdate()
    {
        _world->MarkForUpdate(this);
    }
} // namespace sb