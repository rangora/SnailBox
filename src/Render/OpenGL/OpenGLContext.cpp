#include "OpenGLContext.h"

#include "Core/Common.h"
#include "Core/Application.h"
#include "Render/Shader.h"
#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <imgui/imgui.h>
#include <glm/glm.hpp>

#include <string>
#include <glm/gtc/type_ptr.hpp>

namespace sb
{
    bool LoadFile(const std::string& arg_filename, GLenum arg_shaderType)
    {
        auto result = LoadTextFile(arg_filename);
        if (!result.has_value())
        {
            // Should log
            return false;
        }

        auto& code = result.value();
        const char* codePtr = code.c_str();
        int32_t codeLength = (int32_t)code.length();

        uint32_t shader = glCreateShader(arg_shaderType);
        glShaderSource(shader, 1, (const GLchar* const*)&codePtr, &codeLength);
        glCompileShader(shader);

        int success = 0;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

        if(!success)
        {
            char infoLog[1024];
            glGetShaderInfoLog(shader, 1024, nullptr, infoLog);
            // Should log
            return false;
        }

        return true;
    }

    OpenGLContext::OpenGLContext(GLFWwindow* arg_window_handle) : m_window_handle(arg_window_handle)
    {
        // Assert 필요.
    }

    SPtr<OpenGLShader> OpenGLContext::CreateFromFile(const std::string& in_filename, GLenum in_shaderType)
    {
        auto shader = std::unique_ptr<OpenGLShader>(new OpenGLShader());

        if (!shader->LoadFile(in_filename, in_shaderType))
        {
            // Should log
            return nullptr;
        }

        return std::move(shader);
    }

    void OpenGLContext::Init()
    {
        glfwMakeContextCurrent(m_window_handle);
        int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

        glClearColor(1.f, 0.2f, 0.3f, 0.f);

        // Assert 및 버전 체크, 정보 로그 출력 필요.
    }

    void OpenGLContext::SwapBuffers()
    {
        glfwSwapBuffers(m_window_handle);
    }

    void OpenGLContext::AttachShader(const ShaderArchive& in_archive)
    {
        if (m_program != GL_FALSE)
        {
            return;
        }

        m_program = glCreateProgram();

        for (const auto& [_, shader] : in_archive.m_shaders)
        {
            if (shader->bCompiled)
            {
                glAttachShader(m_program, shader->GetShaderId());
            }
            glLinkProgram(m_program);
        }

        int32 success = GL_FALSE;
        glGetProgramiv(m_program, GL_LINK_STATUS, &success);
        if(success == GL_FALSE)
        {
            char infoLog[1024];
            glGetProgramInfoLog(m_program, 1024, nullptr, infoLog);
            return;
        }
    }

    void OpenGLContext::Render()
    {
        // imgui render
        if (ImGui::Begin("ui window"))
        {
            ImGui::Text("This is first text...");

            if (ImGui::ColorEdit4("clear color", glm::value_ptr(m_clearColor)))
            {
                glClearColor(m_clearColor.x, m_clearColor.y, m_clearColor.z, m_clearColor.w);
            }

            ImGui::Separator();
        }
        ImGui::End();

        // opengl render
        float vertices[] = {
            0.5f,  -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, // bottom right
            -0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, // bottom left
            0.0f,  0.5f,  0.0f, 0.0f, 0.0f, 1.0f  // top
        };

        // Test Render
        std::vector<glm::vec3> cubePositions = {glm::vec3(0.0f, 0.0f, 0.0f),    glm::vec3(2.0f, 5.0f, -15.0f),
                                                glm::vec3(-1.5f, -2.2f, -2.5f), glm::vec3(-3.8f, -2.0f, -12.3f),
                                                glm::vec3(2.4f, -0.4f, -3.5f),  glm::vec3(-1.7f, 3.0f, -7.5f),
                                                glm::vec3(1.3f, -2.0f, -2.5f),  glm::vec3(1.5f, 2.0f, -2.5f),
                                                glm::vec3(1.5f, 0.2f, -1.5f),   glm::vec3(-1.3f, 1.0f, -1.5f)};

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);
        // glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

        // glUseProgram(m_program);

        // ~Test Render

    }

    void OpenGLContext::TestInit()
    {
        float vertices[] = {
            // pos.xyz, normal.xyz, texcoord.uv
            -0.5f, -0.5f, -0.5f, 0.0f,  0.0f,  -1.0f, 0.0f, 0.0f, 0.5f,  -0.5f, -0.5f, 0.0f,  0.0f,  -1.0f, 1.0f, 0.0f,
            0.5f,  0.5f,  -0.5f, 0.0f,  0.0f,  -1.0f, 1.0f, 1.0f, -0.5f, 0.5f,  -0.5f, 0.0f,  0.0f,  -1.0f, 0.0f, 1.0f,

            -0.5f, -0.5f, 0.5f,  0.0f,  0.0f,  1.0f,  0.0f, 0.0f, 0.5f,  -0.5f, 0.5f,  0.0f,  0.0f,  1.0f,  1.0f, 0.0f,
            0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f, 1.0f, -0.5f, 0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f, 1.0f,

            -0.5f, 0.5f,  0.5f,  -1.0f, 0.0f,  0.0f,  1.0f, 0.0f, -0.5f, 0.5f,  -0.5f, -1.0f, 0.0f,  0.0f,  1.0f, 1.0f,
            -0.5f, -0.5f, -0.5f, -1.0f, 0.0f,  0.0f,  0.0f, 1.0f, -0.5f, -0.5f, 0.5f,  -1.0f, 0.0f,  0.0f,  0.0f, 0.0f,

            0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.5f,  0.5f,  -0.5f, 1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
            0.5f,  -0.5f, -0.5f, 1.0f,  0.0f,  0.0f,  0.0f, 1.0f, 0.5f,  -0.5f, 0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f,

            -0.5f, -0.5f, -0.5f, 0.0f,  -1.0f, 0.0f,  0.0f, 1.0f, 0.5f,  -0.5f, -0.5f, 0.0f,  -1.0f, 0.0f,  1.0f, 1.0f,
            0.5f,  -0.5f, 0.5f,  0.0f,  -1.0f, 0.0f,  1.0f, 0.0f, -0.5f, -0.5f, 0.5f,  0.0f,  -1.0f, 0.0f,  0.0f, 0.0f,

            -0.5f, 0.5f,  -0.5f, 0.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.5f,  0.5f,  -0.5f, 0.0f,  1.0f,  0.0f,  1.0f, 1.0f,
            0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f, -0.5f, 0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f,
        };

        uint32_t indices[] = {
            0,  2,  1,  2,  0,  3,  4,  5,  6,  6,  7,  4,  8,  9,  10, 10, 11, 8,
            12, 14, 13, 14, 12, 15, 16, 17, 18, 18, 19, 16, 20, 22, 21, 22, 20, 23,
        };

        m_vertexBuffer = OpenGLVertexBuffer::Create();
        m_buffer = OpenGLBuffer::CreateWithData(GL_ARRAY_BUFFER, GL_STATIC_DRAW, vertices, sizeof(float) * 8 * 6 * 4);

        m_vertexBuffer->SetAttribute(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 8, 0);
        m_vertexBuffer->SetAttribute(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 8, sizeof(float) * 3);
        m_vertexBuffer->SetAttribute(2, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 8, sizeof(float) * 6);

        m_indexBuffer =
            OpenGLBuffer::CreateWithData(GL_ELEMENT_ARRAY_BUFFER, GL_STATIC_DRAW, indices, sizeof(uint32_t) * 36);

        auto shader = CreateShader("../../resources/shader/simple.vs", "../../resources/shader/simple.fs");

    }

    UPtr<OpenGLShader> OpenGLContext::CreateShader(const std::string& in_vsFilename, const std::string& in_fsFilename)
    {
        auto Shader = UPtr<OpenGLShader>(new OpenGLShader());
        Shader->LoadFile(in_vsFilename, GL_VERTEX_SHADER);
        Shader->LoadFile(in_fsFilename, GL_FRAGMENT_SHADER);
        Shader->CreateProgram();

        return std::move(Shader);
    }
} // namespace sb