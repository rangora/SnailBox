#pragma once

#include "coreMinimal.h"

namespace sb
{
    class Actor
    {
    public:
        virtual void Tick(float delta);

        void SetPosition(const Vector3d position);
        void SetRotation(const Vector3d rotation);
        void SetScale(const Vector3d scale);

    private:
        Transform _transform;
    };
} // namespace sb