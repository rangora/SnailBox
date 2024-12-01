#pragma once

#include "RenderResource.h"

namespace sb
{
    class RenderResourceStorage
    {
    public:
        RenderResourceStorage();
        RenderResourceStorage(const RenderResourceStorage&) = delete;
        RenderResourceStorage& operator=(const RenderResourceStorage&) = delete;
        ~RenderResourceStorage() = default;
    
        static RenderResourceStorage& Get() { return *s_instnace; }
        
        std::vector<CustomRenderResource> _customData;

    private:
        inline static RenderResourceStorage* s_instnace = nullptr;
    };

    // Temp helper.
    void FillBaseRenderResource(std::vector<CustomRenderResource>& data);
} // namespace sb