#pragma once

#include "core/Common.h"
#include "World.h"

namespace sb
{
    class WorldInstance
    {
    public:
        WorldInstance();
        WorldInstance(const WorldInstance&) = delete;
        WorldInstance& operator=(const WorldInstance&) = delete;
        ~WorldInstance();

        static WorldInstance& Get() { return *s_instance; }

        void Tick(float delta);

        World* GetWorld() { return _world.get(); }

        void TEMP_start();

    private:
        inline static WorldInstance* s_instance = nullptr;
        UPtr<World> _world = nullptr;
    };
}