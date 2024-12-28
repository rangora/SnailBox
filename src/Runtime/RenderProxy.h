#pragma once

#include <string>

namespace sb
{
    class SceneComponent;

    struct RenderProxy
    {
        RenderProxy() = delete;
        RenderProxy(SceneComponent* sceneComponent);
        ~RenderProxy();

        std::string _shaderKey;
        class ShaderResource* _shaderResource = nullptr;
        SceneComponent* _sceneComponent = nullptr;
    };
}