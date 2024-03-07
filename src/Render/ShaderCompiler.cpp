#include "ShaderCompiler.h"

#include <iostream>
#include <vector>
#include "OpenGL/OpenGLShader.h"
#include "Core/Common.h"
#include "Enums.h"
#include "boost/algorithm/string.hpp"

#ifdef _WIN32
    #define ensure(context) assert(context)
#endif

namespace sb::ShaderCompiler
{
    void ShaderCompileTool::Initialize()
    {
        // Ready opengl shader tools

    }

    GLenum ShaderCompileTool::GetShaderType(const std::string& in_filename)
    {
        std::vector<std::string> tokens;
        boost::split(tokens, in_filename, boost::is_any_of("."));

        std::string& token = tokens.back();
        if (token == "vs")
        {
            return GL_VERTEX_SHADER;
        }
        else if (token == "fs")
        {
            return GL_FRAGMENT_SHADER;
        }

        return GL_FALSE;
    }

    void ShaderCompileTool::Compile(ShaderArchive& in_archive)
    {
        if (in_archive.GetGraphcisLibType() == GraphicsLibrary::OpenGL)
        {
            for (auto& [filename, shader] : in_archive.m_shaders)
            {
                OpenGLShader* openglShader = static_cast<OpenGLShader*>(shader.get());

                auto text = LoadTextFile(filename);
                if (!text.has_value())
                {
                    continue;
                }

                const GLenum shaderType = GetShaderType(filename);
                if (shaderType == GL_FALSE)
                {
                    continue;
                }

                const std::string& code = text.value();
                const char* codePtr = code.c_str();
                int32 codeLength = static_cast<int32>(code.length());

                // create and compile program.
                openglShader->m_shader = glCreateShader(shaderType);
                glShaderSource(openglShader->m_shader, 1, (const GLchar* const*)&codePtr, &codeLength);
                glCompileShader(openglShader->m_shader);

                int32 success = GL_FALSE;
                glGetShaderiv(openglShader->m_shader, GL_COMPILE_STATUS, &success);
                if(success == GL_FALSE)
                {
                    char infoLog[1024];
                    glGetShaderInfoLog(openglShader->m_shader, 1024, nullptr, infoLog);
                    return;
                }

                openglShader->bCompiled = true;
            }
        }
    }

    void Compile(ShaderArchive& in_archive)
    {
        ShaderCompileTool CompileTool;
        CompileTool.Initialize();
        CompileTool.Compile(in_archive);
        // in_archive.Add()
    }
} // namespace sb::ShaderCompiler