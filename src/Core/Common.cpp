#include "Common.h"
#include <filesystem>
#include <iostream>

#ifdef ON_PLATFORM_WINDOW
    #include <windows.h>
    #pragma comment(lib, "winmm.lib")
#endif

namespace sb
{
#ifdef ON_PLATFORM_WINDOW
    double GetSystemTime()
    {
        return static_cast<double>(timeGetTime());
    }
#endif

    std::optional<std::string> LoadTextFile(const std::string& arg_filename)
    {
        std::filesystem::path currentPath = std::filesystem::current_path();
        std::cout << "현재 경로: " << currentPath.string() << std::endl;

        std::ifstream fin(arg_filename);
        if (!fin.is_open())
        {
            // Should print Log..
            return {};
        }

        std::stringstream text;
        text << fin.rdbuf();

        return text.str();
    }
} // namespace sb