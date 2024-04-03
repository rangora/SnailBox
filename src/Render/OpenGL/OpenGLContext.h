#pragma once

#include <glad/gl.h>
#include "OpenGLBuffer.h"
#include "OpenGLProgram.h"
#include "OpenGLShader.h"
#include "OpenglTexture.h"
#include "Render/GraphicsContext.h"
#include "Render/Light.h"
#include "Render/Material.h"
#include "Render/ShaderArchive.h"

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

        glm::vec4 m_clearColor{glm::vec4(0.1f, 0.2f, 0.3f, 0.0f)};

        // Mesh proerties
        UPtr<OpenglVertexBuffer> m_vertexBuffer = nullptr;
        UPtr<OpenglBuffer> m_vertexObjectBuffer = nullptr;
        UPtr<OpenglBuffer> m_colorBuffer = nullptr;
        UPtr<OpenglBuffer> m_indexBuffer = nullptr;
        UPtr<OpenglBuffer> m_TexCoordBuffer = nullptr;
        UPtr<OpenglBuffer> m_UniformBlockBuffer = nullptr;
        UPtr<OpenglTexture> m_texture = nullptr;
        std::vector<UPtr<Material>> m_materials; // 애매??
        // ~mesh properties

        UPtr<OpenglBuffer> m_c_vertexObjectBuffer = nullptr;
        UPtr<OpenglBuffer> m_c_indexBuffer = nullptr;

        std::vector<UPtr<Light>> m_lights;

        std::map<std::string, UPtr<OpenglProgram>> m_programs;
    };
} // namespace sb