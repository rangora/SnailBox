#include "Common.h"

namespace sb
{
    std::optional<std::string> LoadTextFile(const std::string& arg_filename)
    {
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