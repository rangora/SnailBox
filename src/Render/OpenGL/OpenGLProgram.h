#pragma once

#include "Core/Common.h"
#include "Render/Shader.h"
#include <glm/glm.hpp>
#include <vector>

namespace sb
{
    class OpenGLProgram
    {
    public:
        static UPtr<OpenGLProgram> CreateInternal(const std::vector<SPtr<Shader>>& in_shaders);
        static UPtr<OpenGLProgram> Create(const std::string& in_vertexFilename, const std::string& in_fragFilename);

    ~OpenGLProgram();

    void Use();

    int32 Get() const { return m_program; }

    void SetUniform(const std::string& in_name, int in_value) const;
    void SetUniform(const std::string& in_name, float in_value) const;
    void SetUniform(const std::string& in_name, const glm::vec2& in_value) const;
    void SetUniform(const std::string& in_name, const glm::vec3& in_value) const;
    void SetUniform(const std::string& in_name, const glm::vec4& in_value) const;
    void SetUniform(const std::string& in_name, const glm::mat4& in_value) const;

private:
    OpenGLProgram() {}
    bool LinkProgram(const std::vector<SPtr<Shader>>& in_shaders);

    int32 m_program = 0;
    };
}; // namespace sb