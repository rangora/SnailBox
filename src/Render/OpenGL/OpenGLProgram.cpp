#include "OpenglProgram.h"

#include "OpenGLShader.h"
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <spdlog/spdlog.h>

namespace sb
{
    UPtr<OpenglProgram> OpenglProgram::CreateInternal(const std::vector<SPtr<Shader>>& in_shaders)
    {
        auto program = std::unique_ptr<OpenglProgram>(new OpenglProgram());
        if (!program->LinkProgram(in_shaders))
        {
            return nullptr;
        }

        return std::move(program);
    }

    UPtr<OpenglProgram> OpenglProgram::Create(const std::string& in_vertexFilename, const std::string& in_fragFilename)
    {
        SPtr<OpenglShader> vs = OpenglShader::CreateFromFile(in_vertexFilename, GL_VERTEX_SHADER);
        SPtr<OpenglShader> fs = OpenglShader::CreateFromFile(in_fragFilename, GL_FRAGMENT_SHADER);

        if (!vs || !fs)
        {
            return nullptr;
        }

        return std::move(CreateInternal({vs, fs}));
    }

    OpenglProgram::~OpenglProgram()
    {
        if (m_program)
        {
            glDeleteProgram(m_program);
        }
    }

    void OpenglProgram::Use()
    {
        glUseProgram(m_program);
    }

    void OpenglProgram::SetUniform(const std::string& in_name, int in_value) const
    {
        auto loc = glGetUniformLocation(m_program, in_name.c_str());
        glUniform1i(loc, in_value);
    }
    
    void OpenglProgram::SetUniform(const std::string& in_name, float in_value) const
    {
        // auto loc = glad_glGetUniformLocation(m_program, in_name.c_str());
        auto loc = glGetUniformLocation(m_program, in_name.c_str());
        glUniform1f(loc, in_value);
    }

    void OpenglProgram::SetUniform(const std::string& in_name, const glm::vec2& in_value) const
    {
        auto loc = glGetUniformLocation(m_program, in_name.c_str());
        glUniform2fv(loc, 1, glm::value_ptr(in_value));
    }

    void OpenglProgram::SetUniform(const std::string& in_name, const glm::vec3& in_value) const
    {
        auto loc = glGetUniformLocation(m_program, in_name.c_str());
        glUniform3fv(loc, 1, glm::value_ptr(in_value));
    }

    void OpenglProgram::SetUniform(const std::string& in_name, const glm::vec4& in_value) const
    {
        auto loc = glGetUniformLocation(m_program, in_name.c_str());
        glUniform4fv(loc, 1, glm::value_ptr(in_value));
    }

    void OpenglProgram::SetUniform(const std::string& in_name, const glm::mat4& in_value) const
    {
        auto loc = glGetUniformLocation(m_program, in_name.c_str());
        glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(in_value));
    }

    void OpenglProgram::BindUniformToBindingPoint(const std::string& in_uniformBlockName,
                                                  const GLuint in_bindingPoint) const
    {
        const GLuint blockIndex = glGetUniformBlockIndex(m_program, in_uniformBlockName.c_str());
        if (blockIndex == GL_INVALID_INDEX)
        {
            spdlog::error("Invalid blockIndex.");
            return;
        }

        glUniformBlockBinding(m_program, blockIndex, in_bindingPoint);
    }

    bool OpenglProgram::LinkProgram(const std::vector<SPtr<Shader>>& in_shaders)
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

        m_shaders.insert(m_shaders.end(), in_shaders.begin(), in_shaders.end());

        return true;
    }
} // namespace sb