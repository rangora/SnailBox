#pragma once

#include "Core/Common.h"
#include "Enums.h"

#include <map>
#include <string>

namespace sb
{
    struct ShaderArchive
    {
        ShaderArchive() = delete;
        ShaderArchive(GraphicsLibrary in_type);

        void Add(std::string in_path);

        GraphicsLibrary GetGraphcisLibType() const { return GraphicsLib; }

        std::map<std::string, SPtr<class Shader>> m_shaders;
    
    private:
        GraphicsLibrary GraphicsLib = GraphicsLibrary::None;
    };
}; // namespace sb