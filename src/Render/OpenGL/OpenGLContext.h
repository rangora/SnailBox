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
    class OpenGLContext : public GraphicsContext
    {
    public:
        OpenGLContext() {}
        OpenGLContext(GLFWwindow* arg_window_handle);

        static SPtr<OpenGLShader> CreateFromFile(const std::string& in_filename, GLenum in_shaderType);

        virtual void Initialize() override;
        virtual void SwapBuffers() override;

        // TEMP?
        void Render() final;

        // WinWindowsInput으로 빠져야 함.
        void ProcessInput(GLFWwindow* arg_window);
        void Reshape(int arg_width, int arg_height);
        void MouseMove(double arg_x, double arg_y);
        void MouseButton(int arg_button, int arg_action, double arg_x, double arg_y);
        // ~여기까지

        uint32 m_program = 0;

    private:
        static UPtr<OpenGLShader> CreateShader(const std::string& in_vsFilename, const std::string& in_fsFilename);

        GLFWwindow* m_window_handle = nullptr;

        // Temp values
        glm::vec4 m_clearColor{glm::vec4(0.1f, 0.2f, 0.3f, 0.0f)};

        UPtr<OpenGLVertexBuffer> m_vertexBuffer = nullptr;
        UPtr<OpenGLBuffer> m_vertexObjectBuffer = nullptr;
        UPtr<OpenGLBuffer> m_indexBuffer = nullptr;

        std::vector<UPtr<OpenGLProgram>> m_programs;
    };
} // namespace sb