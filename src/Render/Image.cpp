#include "Image.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

namespace sb
{
    UPtr<Image> Image::Load(const std::string& in_filePath)
    {
        auto image = UPtr<Image>(new Image());
        return image->LoadWithStb(in_filePath) ? std::move(image) : nullptr;
    }

    UPtr<Image> Image::Create(int32 in_width, int32 in_height, int32 in_channelCount)
    {
        auto image = UPtr<Image>(new Image());
        return image->Allocate(in_width, in_height, in_channelCount) ? std::move(image) : nullptr;
    }

    Image::~Image()
    {
        if (m_data)
        {
            stbi_image_free(m_data);
        }
    }
   
    void Image::SetCheckImage(int32 in_grid_x, int32 in_grid_y)
    {
        for (int j = 0; j < m_height; j++)
        {
            for (int i = 0; i < m_width; i++)
            {
                int pos = (j * m_width + i) * m_channelCount;
                bool even = ((i / in_grid_x) + (j / in_grid_y)) % 2 == 0; // 한 줄에 4개의 픽셀을 차지하는 체커보드를 만들기 위함.
                uint8_t value = even ? 255 : 0;
                for (int k = 0; k < m_channelCount; k++)
                {
                    m_data[pos + k] = value;
                }
                if (m_channelCount > 3)
                {
                    m_data[3] = 255; // alpha값이 0이면 투명해지기 때문에 명시적으로 255를 대입.
                }
            }
        }
    }

    bool Image::LoadWithStb(const std::string& in_filePath)
    {
        stbi_set_flip_vertically_on_load(true);
        m_data = stbi_load(in_filePath.c_str(), &m_width, &m_height, &m_channelCount, 0);
        if (!m_data)
        {
            // LOG!
            return false;
        }
        return true;
    }
  
    bool Image::Allocate(int32 in_width, int32 in_height, int32 in_channelCount)
    {
        m_width = in_width;
        m_height = in_height;
        m_channelCount = in_channelCount;
        m_data = (uint8*)malloc(m_width * m_height * m_channelCount);
        return m_data ? true : false;
    }
} // namespace sb