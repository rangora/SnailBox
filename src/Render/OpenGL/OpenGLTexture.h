#define pragma once

#include "Render/Image.h"

namespace sb
{
    class OpenglTexture
    {
    public:
        static UPtr<OpenglTexture> CreateTextureFromImage(const Image* in_image);

        OpenglTexture() = default;
        ~OpenglTexture();

        void Bind() const; // override?
        void SetFilter(int32 in_minFilter, int32 in_magFilter) const;
        void SetWrap(uint32 in_sWarp, uint32 in_tWrap);

    private:
        void CreateTexture();
        void SetTextureFromImage(const Image* in_image);

        uint32 m_texture = 0;
    };
} // namespace sb