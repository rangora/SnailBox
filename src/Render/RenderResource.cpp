#include "RenderResource.h"

namespace sb
{
    RenderResource::RenderResource()
    {
    }

    RenderResource::RenderResource(std::string name) : _name(name)
    {
    }

    CustomRenderResource::CustomRenderResource()
    {
    }

    CustomRenderResource::CustomRenderResource(std::string name) : RenderResource(name)
    {
    }

    void FillBaseRenderResource()
    {
    }
} // namespace sb