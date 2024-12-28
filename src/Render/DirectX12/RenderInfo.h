#pragma once

#include "coreMinimal.h"
#include "corepch.h"
#include <string>

namespace sb
{
    class ShaderResource;
    class Direct3dDriver;

    struct RenderInfo
    {
        friend Direct3dDriver;

        RenderInfo();
        ~RenderInfo();
        RenderInfo(RenderInfo&&) noexcept = default;
        RenderInfo& operator=(RenderInfo&&) noexcept = default;
        RenderInfo(const RenderInfo&) = delete;
        RenderInfo& operator=(const RenderInfo&) = delete;

        Transform _transform;
        std::string _shaderKey;
        ShaderResource* _shaderResource = nullptr;

        void UpdateTransform(const Transform& transform);

    private:
        void UpdateWorldMatrix();

        XMMATRIX _worldMatrix = XMMatrixIdentity();
    };
} // namespace sb