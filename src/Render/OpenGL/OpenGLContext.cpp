#include "OpenGLContext.h"

#include "Core/Application.h"
#include "Core/Common.h"
#include "Render/Shader.h"
#include <GLFW/glfw3.h>
#include <boost/foreach.hpp>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <imgui/imgui.h>

#include <glm/gtc/type_ptr.hpp>
#include <string>

namespace sb
{
    OpenGLContext::OpenGLContext(GLFWwindow* arg_window_handle) : m_window_handle(arg_window_handle)
    {
        // Assert 필요.
    }

    void OpenGLContext::Initialize()
    {
        glfwMakeContextCurrent(m_window_handle);
        int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

        // glClearColor(1.f, 0.2f, 0.3f, 0.f);

        // Assert 및 버전 체크, 정보 로그 출력 필요.

        float vertices[] =
        {
            0.5f,  0.5f,  0.0f, // top right
            0.5f,  -0.5f, 0.0f, // bottom right
            -0.5f, -0.5f, 0.0f, // bottom left
            -0.5f, 0.5f,  0.0f, // top left
        };

        uint32_t indices[] =
        {
            // note that we start from 0!
            0, 1, 3, // first triangle
            1, 2, 3, // second triangle
        };

        // VAO
        m_vertexBuffer = OpenGLVertexBuffer::Create();

        // VBO
        m_vertexObjectBuffer = OpenGLBuffer::CreateWithData(GL_ARRAY_BUFFER, GL_STATIC_DRAW, vertices, sizeof(uint32) * 12);

        // Set VAO desc
        m_vertexBuffer->SetAttribute(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, 0);

        // EBO
        m_indexBuffer =
            OpenGLBuffer::CreateWithData(GL_ELEMENT_ARRAY_BUFFER, GL_STATIC_DRAW, indices, sizeof(uint32) * 6);

        // Create programs.
        m_programs.emplace_back(
            (OpenGLProgram::Create("../../resources/shader/simple.vs", "../../resources/shader/simple.fs")));

        // Use all programs.
        BOOST_FOREACH (const auto& program, m_programs)
        {
            program->Use();
        }
    }

    void OpenGLContext::SwapBuffers()
    {
        glfwSwapBuffers(m_window_handle);
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

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);

        BOOST_FOREACH(UPtr<OpenGLProgram>& program, m_programs)
        {
            program->Use();
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        }
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