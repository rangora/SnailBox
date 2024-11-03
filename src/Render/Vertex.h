#pragma once

#include "corepch.h"
#include "core/Math/Vector.h"

namespace sb
{
    struct Vertex
    {
        Vector3f _pos;
        Vector4f _color;
    };

    struct DxVertex
    {
        XMFLOAT3 _pos;
        XMFLOAT4 _color;
    };
}