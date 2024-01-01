#define pragma once

#include "Render/Image.h"

namespace sb
{
    class OpenGLTexture
    {
        static UPtr<OpenGLTexture> CreateTextureFromImage(const Image* in_image);
        ~OpenGLTexture();

        void Bind() const; // override?
        void SetFilter(int32 in_minFilter, int32 in_magFilter) const;
        void SetWrap(uint32 in_sWarp, uint32 in_tWrap);

    private:
        OpenGLTexture() = default;
        void CreateTexture();
        void SetTextureFromImage(const Image* in_image);

        int32 m_texture = 0;
    };
} // namespace sb