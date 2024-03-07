#include "ShaderArchive.h"

#include "Shader.h"
#include "OpenGL/OpenGLShader.h"

namespace sb
{
    ShaderArchive::ShaderArchive(GraphicsLibrary in_type) : GraphicsLib(in_type) {}

    void ShaderArchive::Add(std::string in_path)
    {
        m_shaders.emplace(in_path, SPtr<OpenGLShader>(new OpenGLShader()));
    }
}; // namespace sb