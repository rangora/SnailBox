#pragma once

#include <memory>
#include <fstream>
#include <sstream>
#include <string>
#include <optional>

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