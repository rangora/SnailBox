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
} // namespace sb