#pragma once

#include "Core/Common.h"
#include "OpenGLBuffer.h"
#include "Render/Shader.h"

#include <vector>
#include <glm/glm.hpp>
#include <glad/glad.h>

struct GLFWwindow;

namespace sb
{
    class OpenGLShader : public Shader
    {
    public:
        static SPtr<OpenGLShader> CreateFromFile(const std::string& in_filename, GLenum in_shaderType);
        OpenGLShader() = default;
        ~OpenGLShader();

        void Render() final;
        int32 GetShaderId() final { return m_shader; }

        bool LoadFile(const std::string& filename, GLenum shaderType);
        bool CreateProgram();

        uint32_t Get() const { return m_shader; }

        // 애매..
        // void Use() const;

        void SetUniform(const std::string& name, int value) const;
        void SetUniform(const std::string& name, float value) const;
        void SetUniform(const std::string& name, const glm::vec3& value) const;
        void SetUniform(const std::string& name, const glm::vec2& value) const;
        void SetUniform(const std::string& name, const glm::mat4& value) const;
        void SetUniform(const std::string& name, const glm::vec4& value) const;

        uint32_t m_shader = 0;

        GLenum m_shaderType;
        UPtr<OpenGLBuffer> m_buffer = nullptr;
        UPtr<OpenGLBuffer> m_indexBuffer = nullptr;

    private:
        std::vector<GLuint> m_openGLShaderIds;
    };
}; // namespace sb