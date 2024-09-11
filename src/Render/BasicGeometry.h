#pragma once

#include <glm/glm.hpp>
#include <Core/Common.h>
#include "Core/Math/Vector.h"

namespace sb
{
    // 2D
    extern Vector3f squareVertices[4];
    extern uint32 squareIndex[6];

    // 3D
    extern Vector3f cubeVertices[24];
    extern Vector3f cubeNormals[24];
    extern Vector3f cubeFaceColors[24];
    extern Vector2f cubeUVs[24];
    extern uint32 cubeVertexIndex[36];
};