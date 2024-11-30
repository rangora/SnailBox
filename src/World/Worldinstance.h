#pragma once

#include "core/Common.h"

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

    private:
        inline static WorldInstance* s_instance = nullptr;
        UPtr<class World> _world = nullptr;
    };
}