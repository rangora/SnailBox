#include "BasicGeometry.h"

namespace sb
{
    Vector3f cubeVertices[24] =
    {
        // 앞면
        Vector3f(-0.5f,  -0.5f,  0.5f),
        Vector3f(0.5f,  -0.5f,  0.5f),
        Vector3f(0.5f,  0.5f,  0.5f),
        Vector3f(-0.5f,  0.5f,  0.5f),

        // 뒷면
        Vector3f(-0.5f, -0.5f, -0.5f),
        Vector3f(0.5f, -0.5f, -0.5f),
        Vector3f(0.5f, 0.5f, -0.5f),
        Vector3f(-0.5f, 0.5f, -0.5f),

        // 왼쪽 면
        Vector3f(-0.5f,  0.5f,  0.5f),
        Vector3f(-0.5f,  0.5f,  -0.5f),
        Vector3f(-0.5f,  -0.5f,  -0.5f),
        Vector3f(-0.5f,  -0.5f,  0.5f),

        // 오른쪽 면
        Vector3f(0.5f, 0.5f, 0.5f),
        Vector3f(0.5f, 0.5f, -0.5f),
        Vector3f(0.5f, -0.5f, -0.5f),
        Vector3f(0.5f, -0.5f, 0.5f),

        // 상단
        Vector3f(-0.5f, 0.5f, -0.5f),
        Vector3f(0.5f, 0.5f, -0.5f),
        Vector3f(0.5f, 0.5f, 0.5f),
        Vector3f(-0.5f, 0.5f, 0.5f),

        // 하단
        Vector3f(-0.5f, -0.5f, -0.5f),
        Vector3f(0.5f, -0.5f, -0.5f),
        Vector3f(0.5f, -0.5f, 0.5f),
        Vector3f(-0.5f, -0.5f, 0.5f)
    };

    Vector3f cubeNormals[24] =
    {
        Vector3f(0.f, 0.f, 1.f),
        Vector3f(0.f, 0.f, 1.f),
        Vector3f(0.f, 0.f, 1.f),
        Vector3f(0.f, 0.f, 1.f),

        Vector3f(0.f, 0.f, 1.f),
        Vector3f(0.f, 0.f, 1.f),
        Vector3f(0.f, 0.f, 1.f),
        Vector3f(0.f, 0.f, 1.f),

        Vector3f(1.f, 0.f, 0.f),
        Vector3f(1.f, 0.f, 0.f),
        Vector3f(1.f, 0.f, 0.f),
        Vector3f(1.f, 0.f, 0.f),

        Vector3f(1.f, 0.f, 0.f),
        Vector3f(1.f, 0.f, 0.f),
        Vector3f(1.f, 0.f, 0.f),
        Vector3f(1.f, 0.f, 0.f),

        Vector3f(1.f, 0.f, 0.f),
        Vector3f(1.f, 0.f, 0.f),
        Vector3f(1.f, 0.f, 0.f),
        Vector3f(1.f, 0.f, 0.f),

        Vector3f(1.f, 0.f, 0.f),
        Vector3f(1.f, 0.f, 0.f),
        Vector3f(1.f, 0.f, 0.f),
        Vector3f(1.f, 0.f, 0.f),
    };

    Vector3f cubeFaceColors[24] =
    {
        Vector3f(1.f, 0.f, 0.f),
        Vector3f(1.f, 0.f, 0.f),
        Vector3f(1.f, 0.f, 0.f),
        Vector3f(1.f, 0.f, 0.f),

        Vector3f(0.f, 1.f, 0.f),
        Vector3f(0.f, 1.f, 0.f),
        Vector3f(0.f, 1.f, 0.f),
        Vector3f(0.f, 1.f, 0.f),

        Vector3f(0.f, 0.f, 1.f),
        Vector3f(0.f, 0.f, 1.f),
        Vector3f(0.f, 0.f, 1.f),
        Vector3f(0.f, 0.f, 1.f),

        Vector3f(1.f, 1.f, 0.f),
        Vector3f(1.f, 1.f, 0.f),
        Vector3f(1.f, 1.f, 0.f),
        Vector3f(1.f, 1.f, 0.f),

        Vector3f(1.f, 1.f, 1.f),
        Vector3f(1.f, 1.f, 1.f),
        Vector3f(1.f, 1.f, 1.f),
        Vector3f(1.f, 1.f, 1.f),

        Vector3f(1.f, 0.f, 1.f),
        Vector3f(1.f, 0.f, 1.f),
        Vector3f(1.f, 0.f, 1.f),
        Vector3f(1.f, 0.f, 1.f)
    };

    Vector2f cubeUVs[24] =
    {
        Vector2f(0.f, 0.f),
        Vector2f(1.f, 0.f),
        Vector2f(1.f, 1.f),
        Vector2f(0.f, 1.f),

        Vector2f(0.f, 0.f),
        Vector2f(1.f, 0.f),
        Vector2f(1.f, 1.f),
        Vector2f(0.f, 1.f),

        Vector2f(1.f, 0.f),
        Vector2f(1.f, 1.f),
        Vector2f(0.f, 1.f),
        Vector2f(0.f, 0.f),

        Vector2f(1.f, 0.f),
        Vector2f(1.f, 1.f),
        Vector2f(0.f, 1.f),
        Vector2f(0.f, 0.f),

        Vector2f(0.f, 1.f),
        Vector2f(1.f, 1.f),
        Vector2f(1.f, 0.f),
        Vector2f(0.f, 0.f),

        Vector2f(0.f, 1.f),
        Vector2f(1.f, 1.f),
        Vector2f(1.f, 0.f),
        Vector2f(0.f, 0.f),
    };

    uint32 cubeVertexIndex[36] =
    {
        0,  1,  2,
        2,  3,  0,

        4,  5,  6,
        6,  7,  4,

        8,  9,  10,
        10,  11,  8,

        12,  13,  14,
        14,  15,  12,

        16,  17,  18,
        18,  19,  16,

        20,  21,  22,
        22,  23,  20
    };
};