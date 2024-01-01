#pragma once

#include "Core/Common.h"

namespace sb
{
    class Image
    {
        public:
            static UPtr<Image> Load(const std::string& in_filePath);
            static UPtr<Image> Create(int32 in_width, int32 in_height, int32 in_channelCount = 4);
            ~Image();

            const uint8* GetData() const { return m_data; }
            int32 GetWidth() const { return m_width; }
            int32 GetHeight() const { return m_height; }
            int32 GetChannelCount() const { return m_channelCount; }

            void SetCheckImage(int32 in_grid_x, int32 in_grid_y); // SAMPLE // 

        private:
            Image() = default;
            bool LoadWithStb(const std::string& in_filePath);
            bool Allocate(int32 in_width, int32 in_height, int32 in_channelCount);
            
            int32 m_width = 0;
            int32 m_height = 0;
            int32 m_channelCount = 0;
            uint8* m_data = nullptr;
    };
} // namespace sb