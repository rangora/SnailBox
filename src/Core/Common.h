﻿#pragma once

#include <memory>
#include <fstream>
#include <sstream>
#include <string>
#include <optional>

using int8 = __int8;
using int16 = __int16;
using int32 = __int32;
using int64 = __int64;
using uint8 = unsigned __int8;
using uint16 = unsigned __int16;
using uint32 = unsigned __int32;
using uint64 = unsigned __int64;

namespace sb
{
    template <typename T> 
    using UPtr = std::unique_ptr<T>;
    
    template <typename T, typename ... Args> 
    constexpr UPtr<T> CreateUPtr(Args&& ... args)
    {
        return std::make_unique<T>(std::forward<Args>(args)...);
    }

    template <typename T> 
    using SPtr = std::shared_ptr<T>;
    
    template <typename T, typename ... Args> 
    constexpr SPtr<T> CreateSPtr(Args&& ... args)
    {
        return std::make_shared<T>(std::forward<Args>(args)...);
    }

    // 나중에 FileSystem으로 빠져야 함
    // 템플릿 함수만 있도록 구성할 것..
    std::optional<std::string> LoadTextFile(const std::string& arg_filename);
} // namespace sb