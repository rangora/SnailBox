#pragma once

#include <glad/glad.h>

#include "Render/ShaderArchive.h"
#include "src/Render/GraphicsContext.h"
#include "OpenGLShader.h"
#include "OpenGLBuffer.h"
#include "OpenGLProgram.h"

struct GLFWwindow;

namespace sb
{
    class Window;

    class OpenGLContext : public GraphicsContext
    {
    public:
        OpenGLContext() {}
        OpenGLContext(GLFWwindow* in_window_handle, Window* in_window);

        static SPtr<OpenGLShader> CreateFromFile(const std::string& in_filename, GLenum in_shaderType);

        virtual void Initialize() override;
        virtual void SwapBuffers() override;

        // TEMP?
        void Render() final;

        uint32 m_program = 0;

    private:
        static UPtr<OpenGLShader> CreateShader(const std::string& in_vsFilename, const std::string& in_fsFilename);

        GLFWwindow* m_glWindow_handle = nullptr;
        Window* m_window_handle = nullptr;

        // Temp values
        glm::vec4 m_clearColor{glm::vec4(0.1f, 0.2f, 0.3f, 0.0f)};

        UPtr<OpenGLVertexBuffer> m_vertexBuffer = nullptr;
        UPtr<OpenGLBuffer> m_vertexObjectBuffer = nullptr;
        UPtr<OpenGLBuffer> m_indexBuffer = nullptr;

        UPtr<OpenGLVertexBuffer> m_c_vertexBuffer = nullptr;
        UPtr<OpenGLBuffer> m_c_vertexObjectBuffer = nullptr;
        UPtr<OpenGLBuffer> m_c_indexBuffer = nullptr;

        std::vector<UPtr<OpenGLProgram>> m_programs;
    };
} // namespace sb