#include "BasicGeometry.h"

namespace sb
{
    glm::vec3 cubeVertices[24] =
    {
        // 앞면
        glm::vec3(-0.5f,  -0.5f,  0.5f),
        glm::vec3(0.5f,  -0.5f,  0.5f),
        glm::vec3(0.5f,  0.5f,  0.5f),
        glm::vec3(-0.5f,  0.5f,  0.5f),

        // 뒷면
        glm::vec3(-0.5f, -0.5f, -0.5f),
        glm::vec3(0.5f, -0.5f, -0.5f),
        glm::vec3(0.5f, 0.5f, -0.5f),
        glm::vec3(-0.5f, 0.5f, -0.5f),

        // 왼쪽 면
        glm::vec3(-0.5f,  0.5f,  0.5f),
        glm::vec3(-0.5f,  0.5f,  -0.5f),
        glm::vec3(-0.5f,  -0.5f,  -0.5f),
        glm::vec3(-0.5f,  -0.5f,  0.5f),

        // 오른쪽 면
        glm::vec3(0.5f, 0.5f, 0.5f),
        glm::vec3(0.5f, 0.5f, -0.5f),
        glm::vec3(0.5f, -0.5f, -0.5f),
        glm::vec3(0.5f, -0.5f, 0.5f),

        // 상단
        glm::vec3(-0.5f, 0.5f, -0.5f),
        glm::vec3(0.5f, 0.5f, -0.5f),
        glm::vec3(0.5f, 0.5f, 0.5f),
        glm::vec3(-0.5f, 0.5f, 0.5f),

        // 하단
        glm::vec3(-0.5f, -0.5f, -0.5f),
        glm::vec3(0.5f, -0.5f, -0.5f),
        glm::vec3(0.5f, -0.5f, 0.5f),
        glm::vec3(-0.5f, -0.5f, 0.5f)
    };

    glm::vec3 cubeNormals[24] =
    {
        glm::vec3(0.f, 0.f, 1.f),
        glm::vec3(0.f, 0.f, 1.f),
        glm::vec3(0.f, 0.f, 1.f),
        glm::vec3(0.f, 0.f, 1.f),

        glm::vec3(0.f, 0.f, 1.f),
        glm::vec3(0.f, 0.f, 1.f),
        glm::vec3(0.f, 0.f, 1.f),
        glm::vec3(0.f, 0.f, 1.f),

        glm::vec3(1.f, 0.f, 0.f),
        glm::vec3(1.f, 0.f, 0.f),
        glm::vec3(1.f, 0.f, 0.f),
        glm::vec3(1.f, 0.f, 0.f),

        glm::vec3(1.f, 0.f, 0.f),
        glm::vec3(1.f, 0.f, 0.f),
        glm::vec3(1.f, 0.f, 0.f),
        glm::vec3(1.f, 0.f, 0.f),

        glm::vec3(1.f, 0.f, 0.f),
        glm::vec3(1.f, 0.f, 0.f),
        glm::vec3(1.f, 0.f, 0.f),
        glm::vec3(1.f, 0.f, 0.f),

        glm::vec3(1.f, 0.f, 0.f),
        glm::vec3(1.f, 0.f, 0.f),
        glm::vec3(1.f, 0.f, 0.f),
        glm::vec3(1.f, 0.f, 0.f),
    };

    glm::vec3 cubeFaceColors[24] =
    {
        glm::vec3(1.f, 0.f, 0.f),
        glm::vec3(1.f, 0.f, 0.f),
        glm::vec3(1.f, 0.f, 0.f),
        glm::vec3(1.f, 0.f, 0.f),

        glm::vec3(0.f, 1.f, 0.f),
        glm::vec3(0.f, 1.f, 0.f),
        glm::vec3(0.f, 1.f, 0.f),
        glm::vec3(0.f, 1.f, 0.f),

        glm::vec3(0.f, 0.f, 1.f),
        glm::vec3(0.f, 0.f, 1.f),
        glm::vec3(0.f, 0.f, 1.f),
        glm::vec3(0.f, 0.f, 1.f),

        glm::vec3(1.f, 1.f, 0.f),
        glm::vec3(1.f, 1.f, 0.f),
        glm::vec3(1.f, 1.f, 0.f),
        glm::vec3(1.f, 1.f, 0.f),

        glm::vec3(1.f, 1.f, 1.f),
        glm::vec3(1.f, 1.f, 1.f),
        glm::vec3(1.f, 1.f, 1.f),
        glm::vec3(1.f, 1.f, 1.f),

        glm::vec3(1.f, 0.f, 1.f),
        glm::vec3(1.f, 0.f, 1.f),
        glm::vec3(1.f, 0.f, 1.f),
        glm::vec3(1.f, 0.f, 1.f)
    };

    glm::vec2 cubeUVs[24] =
    {
        glm::vec2(0.f, 0.f),
        glm::vec2(1.f, 0.f),
        glm::vec2(1.f, 1.f),
        glm::vec2(0.f, 1.f),

        glm::vec2(0.f, 0.f),
        glm::vec2(1.f, 0.f),
        glm::vec2(1.f, 1.f),
        glm::vec2(0.f, 1.f),

        glm::vec2(1.f, 0.f),
        glm::vec2(1.f, 1.f),
        glm::vec2(0.f, 1.f),
        glm::vec2(0.f, 0.f),

        glm::vec2(1.f, 0.f),
        glm::vec2(1.f, 1.f),
        glm::vec2(0.f, 1.f),
        glm::vec2(0.f, 0.f),

        glm::vec2(0.f, 1.f),
        glm::vec2(1.f, 1.f),
        glm::vec2(1.f, 0.f),
        glm::vec2(0.f, 0.f),

        glm::vec2(0.f, 1.f),
        glm::vec2(1.f, 1.f),
        glm::vec2(1.f, 0.f),
        glm::vec2(0.f, 0.f),
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