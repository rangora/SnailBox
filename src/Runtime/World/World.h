#pragma once

#include <vector>
#include "coreMinimal.h"
#include "Runtime/SceneComponent.h"

class Actor;

namespace sb
{
    class World
    {
    public:
        World();
        ~World();

        void Tick(float delta);
        void SpawnActor(const Vector3d position);

        void RegisterComponent(SceneComponent* _component);
        void RegisterActor(Actor* _actor);
        void MarkForUpdate(SceneComponent* _component);

    private:
        void UpdateMarkedComponents();

        std::vector<Actor*> _registeredActors;
        std::vector<SceneComponent*> _registeredComponents;
        std::vector<SceneComponent*> _componentsThatNeedForUpdate;
    };
} // namespace sb