#pragma once

#include "coreMinimal.h"
#include <string>

namespace sb
{
    class ShaderResource;

    struct RenderInfo
    {
        RenderInfo();
        ~RenderInfo();
        RenderInfo(RenderInfo&&) noexcept = default;
        RenderInfo& operator=(RenderInfo&&) noexcept = default;
        RenderInfo(const RenderInfo&) = delete;
        RenderInfo& operator=(const RenderInfo&) = delete;

        Transform _transform;
        std::string _shaderKey;
        ShaderResource* _shaderResource = nullptr;

    };
} // namespace sb