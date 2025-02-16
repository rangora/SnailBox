#pragma once

#include "corepch.h"
#include "Core/Common.h"

namespace sb
{
    struct TransformBuffer
    {
        SimpleMath::Matrix _wvpMat;
    };

    enum class CBV_Register : uint8
    {
        b0,
        b1,
        b2,
        b3,
        b4,
        END,
    };

    enum
    {
        CBV_REGISTER_COUNT = CBV_Register::END,
        SRV_REGISTER_COUNT = 0,
        UAV_REGISTER_COUNT = 0,
    };
}