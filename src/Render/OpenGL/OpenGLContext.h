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

    class OpenglContext : public GraphicsContext
    {
    public:
        OpenglContext() {}
        OpenglContext(GLFWwindow* in_window_handle, Window* in_window);

        static SPtr<OpenglShader> CreateFromFile(const std::string& in_filename, GLenum in_shaderType);

        virtual void Initialize() override;
        virtual void SwapBuffers() override;

        // TEMP?
        void Render() final;

        uint32 m_program = 0;

    private:
        static UPtr<OpenglShader> CreateShader(const std::string& in_vsFilename, const std::string& in_fsFilename);

        GLFWwindow* m_glWindow_handle = nullptr;
        Window* m_window_handle = nullptr;

        // Temp values
        glm::vec4 m_clearColor{glm::vec4(0.1f, 0.2f, 0.3f, 0.0f)};

        UPtr<OpenglVertexBuffer> m_vertexBuffer = nullptr;
        UPtr<OpenglBuffer> m_vertexObjectBuffer = nullptr;
        UPtr<OpenglBuffer> m_colorBuffer = nullptr;
        UPtr<OpenglBuffer> m_indexBuffer = nullptr;

        UPtr<OpenglBuffer> m_c_vertexObjectBuffer = nullptr;
        UPtr<OpenglBuffer> m_c_indexBuffer = nullptr;

        std::vector<UPtr<OpenglProgram>> m_programs;
    };
} // namespace sb