#pragma once

#include "Core/Math/Vector.h"

namespace sb
{
    struct Camera
    {
        Vector3d m_cameraPos = Vector3d::zeroVector;
        Vector3d m_cameraFront = Vector3d::zeroVector;
        Vector3d m_cameraUp = Vector3d::zeroVector;

        float m_cameraPitch = 0.f;
        float m_cameraYaw = 0.f;
    };
};