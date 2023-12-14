#pragma once

#include <glad/glad.h>

#include "src/Render/GraphicsContext.h"
#include "OpenGLShader.h"

struct GLFWwindow;

namespace sb
{
    class OpenGLContext : public GraphicsContext
    {
    public:
        OpenGLContext() {}
        OpenGLContext(GLFWwindow* arg_window_handle);

        // Create는 GraphicsContext에서 해야하지만 임시로 우선 여기서 해준다.
        static UPtr<OpenGLContext> Create();

        static SPtr<OpenGLShader> CreateFromFile(const std::string& in_filename, GLenum in_shaderType);

        virtual void Init() override;
        virtual void SwapBuffers() override;

        // TEMP?
        void Render() final;

        // WinWindowsInput으로 빠져야 함.
        void ProcessInput(GLFWwindow* arg_window);
        void Reshape(int arg_width, int arg_height);
        void MouseMove(double arg_x, double arg_y);
        void MouseButton(int arg_button, int arg_action, double arg_x, double arg_y);
        // ~여기까지

    private:
        GLFWwindow* m_window_handle = nullptr;

        // Temp values
        glm::vec4 m_clearColor{glm::vec4(0.1f, 0.2f, 0.3f, 0.0f)};
    };
} // namespace sb