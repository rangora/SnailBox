#pragma once

#include "Core/Common.h"
#include "Render/Opengl/OpenGLTexture.h"

namespace sb
{
    struct Material
    {
        // TODO : OpenglTexture 대신 Texture를 쓰고 dx12도 받을 수 있도록 해야 함.
        UPtr<OpenglTexture> m_diffuse = nullptr;
        UPtr<OpenglTexture> m_specular = nullptr;
        float shininess = 32.f;
    };
}; // namespace sb