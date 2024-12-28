#pragma once

#include "coreMinimal.h"

namespace sb
{
    class World;

    class Actor
    {
    public:
        Actor();
        ~Actor();

        virtual void Tick(float delta);

        void SetPosition(const Vector3d position);
        void SetRotation(const Vector3d rotation);
        void SetScale(const Vector3d scale);

    private:
        World* _world = nullptr;
        Transform _transform;
    };
} // namespace sb