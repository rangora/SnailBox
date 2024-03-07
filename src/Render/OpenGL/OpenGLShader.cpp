#include "OpenGLShader.h"

#include <memory>

#include "Core/Common.h"
#include "OpenGLBuffer.h"
#include "OpenGLTexture.h"
#include "OpenGLContext.h"

namespace sb
{
    SPtr<OpenGLShader> OpenGLShader::CreateFromFile(const std::string& in_filename, GLenum in_shaderType)
    {
        auto shader = std::unique_ptr<OpenGLShader>(new OpenGLShader());

        if (!shader->LoadFile(in_filename, in_shaderType))
        {
            // Should log
            return nullptr;
        }

        return std::move(shader);
    }

    OpenGLShader::~OpenGLShader()
    {
        if (m_shader)
        {
            glDeleteShader(m_shader);
        }
    }

    void OpenGLShader::Render()
    {
    }

    bool OpenGLShader::LoadFile(const std::string& filename, GLenum shaderType)
    {
        auto result = LoadTextFile(filename);
        if (!result.has_value())
        {
            return false;
        }
        
        auto& code = result.value();
        const char* codePtr = code.c_str();
        int32_t codeLength = (int32_t)code.length();

        // create and compile shader
        m_shader = glCreateShader(shaderType);
        glShaderSource(m_shader, 1, (const GLchar* const*)&codePtr, &codeLength);
        glCompileShader(m_shader);

        // check compile error
        int success = 0;
        glGetShaderiv(m_shader, GL_COMPILE_STATUS,
                      &success); // i : int, v : vector(pointer)
        if (!success)
        {
            char infoLog[1024];
            glGetShaderInfoLog(m_shader, 1024, nullptr, infoLog);
            // SPDLOG_ERROR("failed to compile shader: \"{}\"", filename);
            // SPDLOG_ERROR("reason: {}", *infoLog);
            return false;
        }

        m_openGLShaderIds.push_back(m_shader);

        return true;
    }

    bool OpenGLShader::CreateProgram()
    {
        m_shader = glCreateProgram();
        for (auto id : m_openGLShaderIds)
        {
            glAttachShader(m_shader, id);
        }
        glLinkProgram(m_shader);

        int success = 0;

        glGetProgramiv(m_shader, GL_LINK_STATUS, &success);
        if (!success)
        {
            // LOG!!
            assert(false);
            return false;
        }

        return true;
    }
} // namespace sb
