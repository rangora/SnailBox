﻿#pragma once

#include <glad/gl.h>
#include "ShaderArchive.h"


namespace sb::ShaderCompiler
{
    struct ShaderCompileTool
    {
        void Initialize();
        void Compile(ShaderArchive& in_archive);

    private:
        GLenum GetShaderType(const std::string& in_filename);
    };

    void Compile(ShaderArchive& in_archive);
}; // namespace sb::ShaderCompiler