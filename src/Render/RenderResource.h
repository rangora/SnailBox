#pragma once

#include "ShaderGeometry.h"

namespace sb
{
    class RenderResource
    {
    public:
        RenderResource();
        RenderResource(std::string name);
        ~RenderResource() = default;

        std::string GetName() const { return _name; }

    private:
        std::string _name;
    };

    class CustomRenderResource : public RenderResource
    {
    public:
        CustomRenderResource();
        CustomRenderResource(std::string name);
        ~CustomRenderResource() = default;

        ShaderGeometryRawData _RawData;
        std::string _vsPath;
        std::string _psPath;
    };
} // namespace sb