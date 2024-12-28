#include "Worldinstance.h"
#include "World.h"

namespace sb
{
    WorldInstance::WorldInstance()
    {
        s_instance = this;

        _world = CreateUPtr<World>();
    }

    WorldInstance::~WorldInstance()
    {
    }

    void WorldInstance::Tick(float delta)
    {
        _world->Tick(delta);
    }

    void WorldInstance::TEMP_start()
    {
        _world->SpawnActor(Vector3d(0.0, 0.0, 0.0));
    }
} // namespace sb