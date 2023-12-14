#include "OpenGLShader.h"

#include <memory>

#include "Core/Common.h"

namespace sb
{
    OpenGLShader::~OpenGLShader()
    {
        if (m_program)
        {
            glDeleteShader(m_program);
        }
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
        m_program = glCreateShader(shaderType);
        glShaderSource(m_program, 1, (const GLchar* const*)&codePtr, &codeLength);
        glCompileShader(m_program);

        // check compile error
        int success = 0;
        glGetShaderiv(m_program, GL_COMPILE_STATUS,
                      &success); // i : int, v : vector(pointer)
        if (!success)
        {
            char infoLog[1024];
            glGetShaderInfoLog(m_program, 1024, nullptr, infoLog);
            // SPDLOG_ERROR("failed to compile shader: \"{}\"", filename);
            // SPDLOG_ERROR("reason: {}", *infoLog);
            return false;
        }

        return true;
    }
} // namespace sb
