#include "OpenglTexture.h"

#include <glad/glad.h>

namespace sb
{
    UPtr<OpenglTexture> OpenglTexture::CreateTextureFromImage(const Image* in_image)
    {
        auto texture = UPtr<OpenglTexture>(new OpenglTexture());
        texture->CreateTexture();
        texture->SetTextureFromImage(in_image);
        return std::move(texture);
    }

    OpenglTexture::~OpenglTexture()
    {
        if (m_texture)
        {
            glDeleteTextures(1, &m_texture);
        }
    }

    void OpenglTexture::Bind() const
    {
        glBindTexture(GL_TEXTURE_2D, m_texture);
    }

    void OpenglTexture::SetFilter(int32 in_minFilter, int32 in_magFilter) const
    {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, in_minFilter);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, in_magFilter);
    }

    void OpenglTexture::SetWrap(uint32 in_sWarp, uint32 in_tWrap)
    {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, in_sWarp);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, in_tWrap);
    }

    void OpenglTexture::CreateTexture()
    {
        glGenTextures(1, &m_texture);
        Bind();
        SetFilter(GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
        SetWrap(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
    }

    void OpenglTexture::SetTextureFromImage(const Image* in_image)
    {
        GLenum format = GL_RGBA;
        switch (in_image->GetChannelCount())
        {
            default:
                break;
            case 1:
                format = GL_RED;
                break;
            case 2:
                format = GL_RG;
                break;
            case 3:
                format = GL_RGB;
                break;
        }

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, in_image->GetWidth(), in_image->GetHeight(), 0, format, GL_UNSIGNED_BYTE,
                     in_image->GetData());

        glGenerateMipmap(GL_TEXTURE_2D);
    }
} // namespace sb