#include "OpenGLProgram.h"

#include "OpenGLShader.h"
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace sb
{
    UPtr<OpenGLProgram> OpenGLProgram::CreateInternal(const std::vector<SPtr<Shader>>& in_shaders)
    {
        auto program = std::unique_ptr<OpenGLProgram>(new OpenGLProgram());
        if (!program->LinkProgram(in_shaders))
        {
            return nullptr;
        }

        return std::move(program);
    }

    UPtr<OpenGLProgram> OpenGLProgram::Create(const std::string& in_vertexFilename, const std::string& in_fragFilename)
    {
        SPtr<OpenGLShader> vs = OpenGLShader::CreateFromFile(in_vertexFilename, GL_VERTEX_SHADER);
        SPtr<OpenGLShader> fs = OpenGLShader::CreateFromFile(in_fragFilename, GL_FRAGMENT_SHADER);

        if (!vs || !fs)
        {
            return nullptr;
        }

        return std::move(CreateInternal({vs, fs}));
    }

    OpenGLProgram::~OpenGLProgram()
    {
        if (m_program)
        {
            glDeleteProgram(m_program);
        }
    }

    void OpenGLProgram::Use()
    {
        glUseProgram(m_program);
    }

    void OpenGLProgram::SetUniform(const std::string& in_name, int in_value) const
    {
        auto loc = glGetUniformLocation(m_program, in_name.c_str());
        glUniform1i(loc, in_value);
    }

    void OpenGLProgram::SetUniform(const std::string& in_name, float in_value) const
    {
        auto loc = glGetUniformLocation(m_program, in_name.c_str());
        glUniform1f(loc, in_value);
    }

    void OpenGLProgram::SetUniform(const std::string& in_name, const glm::vec2& in_value) const
    {
        auto loc = glGetUniformLocation(m_program, in_name.c_str());
        glUniform2fv(loc, 1, glm::value_ptr(in_value));
    }

    void OpenGLProgram::SetUniform(const std::string& in_name, const glm::vec3& in_value) const
    {
        auto loc = glGetUniformLocation(m_program, in_name.c_str());
        glUniform3fv(loc, 1, glm::value_ptr(in_value));
    }

    void OpenGLProgram::SetUniform(const std::string& in_name, const glm::vec4& in_value) const
    {
        auto loc = glGetUniformLocation(m_program, in_name.c_str());
        glUniform4fv(loc, 1, glm::value_ptr(in_value));
    }

    void OpenGLProgram::SetUniform(const std::string& in_name, const glm::mat4& in_value) const
    {
        auto loc = glGetUniformLocation(m_program, in_name.c_str());
        glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(in_value));
    }

    bool OpenGLProgram::LinkProgram(const std::vector<SPtr<Shader>>& in_shaders)
    {
        m_program = glCreateProgram();
        for (auto& shader : in_shaders)
        {
            glAttachShader(m_program, shader->GetShaderId());
        }
        glLinkProgram(m_program);

        int success = 0;

        glGetProgramiv(m_program, GL_LINK_STATUS, &success);
        if (!success)
        {
            char infoLog[1024];
            glGetProgramInfoLog(m_program, 1024, nullptr, infoLog);
            return false;
        }

        return true;
    }
} // namespace sb