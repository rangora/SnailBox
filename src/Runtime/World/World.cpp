#include "World.h"
#include "Core/Application.h"
#include "Runtime/NativeActor.h"
#include "Runtime/Actor.h"
#include "Runtime/SceneComponent.h"

namespace sb
{
    World::World()
    {
    }

    World::~World()
    {
    }

    void World::Tick(float delta)
    {
        // Actors, component tick first.
        for (auto actor : _registeredActors)
        {
            actor->Tick(delta);
        }

        for (auto component : _registeredComponents)
        {
            component->Tick(delta);
        }

        UpdateMarkedComponents();
    }

    void World::SpawnActor(const Vector3d position)
    {
        Actor* newActor = new NativeActor;
        newActor->SetPosition(position);
    }

    void World::RegisterComponent(SceneComponent* _component)
    {
        _registeredComponents.push_back(std::move(_component));
    }

    void World::RegisterActor(Actor* _actor)
    {
        _registeredActors.push_back(std::move(_actor));
    }

    void World::MarkForUpdate(SceneComponent* _component)
    {
        _componentsThatNeedForUpdate.push_back(std::move(_component));
    }

    void World::UpdateMarkedComponents()
    {
        for (auto component : _componentsThatNeedForUpdate)
        {
            RenderProxy* renderProxy = component->GetRenderProxy();
            if (renderProxy == nullptr)
            {
                spdlog::warn("RenderProxy is not valid.");
                continue;
            }

            sg_d3dDriver->BatchRenderInfo(renderProxy);

            component->_bRenderMarkDirty = false;
        }

        _componentsThatNeedForUpdate.clear();
    }

} // namespace sb