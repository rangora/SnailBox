#pragma once

#include "Core/Common.h"

#include <glm/glm.hpp>
#include <glad/glad.h>

struct GLFWwindow;

namespace sb
{
    class Shader
    {
    public:
        ~Shader();

        uint32_t Get() const
        {
            return m_shader;
        }

    // private:
        Shader()
        {
        }
        bool LoadFile(const std::string& filename, GLenum shaderType);
        uint32_t m_shader{0};
    };

    class OpenGLShader
    {
    public:
        ~OpenGLShader();
        
        bool LoadFile(const std::string& filename, GLenum shaderType);

        uint32_t Get() const { return m_program; }

        // 애매..
        // void Use() const;

        void SetUniform(const std::string& name, int value) const;
        void SetUniform(const std::string& name, float value) const;
        void SetUniform(const std::string& name, const glm::vec3& value) const;
        void SetUniform(const std::string& name, const glm::vec2& value) const;
        void SetUniform(const std::string& name, const glm::mat4& value) const;
        void SetUniform(const std::string& name, const glm::vec4& value) const;

    // private:
        OpenGLShader(){};

        uint32_t m_program = 0;
    };
}; // namespace sb