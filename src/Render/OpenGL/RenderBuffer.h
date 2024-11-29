#pragma once

#include "Render/GraphicsEnum.h"

namespace sb
{
    class RenderBuffer
    {
    public:
        VertexAttribute m_attribute = VertexAttribute::None;
        void* m_data = nullptr;
    };
}; // namespace sb