#pragma once

#include "Core/Common.h"

namespace sb
{
    enum class RenderFrameWork : uint8
    {
        OpenGL,
        Direct12
    };

    class Shader
    {
    public:
        virtual void Render() = 0;
        virtual int32 GetShaderId() = 0;

        bool bCompiled = false;
    };
}; // namespace sb