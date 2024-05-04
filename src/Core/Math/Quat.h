#pragma once

namespace
{
    struct alignas(16) Quat
    {
        double X;
        double Y;
        double Z;
        double W;
    };
};