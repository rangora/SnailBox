#include "ShaderArchive.h"

#include "Shader.h"
#include "OpenGL/OpenGLShader.h"

namespace sb
{
    ShaderArchive::ShaderArchive(GraphicsDevice in_type) : GraphicsLib(in_type) {}

    void ShaderArchive::Add(std::string in_path)
    {
        m_shaders.emplace(in_path, SPtr<OpenglShader>(new OpenglShader()));
    }
}; // namespace sb