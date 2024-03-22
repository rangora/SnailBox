#include "OpenGLContext.h"

#include "Core/Application.h"
#include "Core/Common.h"
#include "Render/Shader.h"
#include <GLFW/glfw3.h>
#include <boost/foreach.hpp>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <imgui/imgui.h>
#include "Core/WinWindow.h"
#include "Render/BasicGeometry.h"

#include <glm/gtc/type_ptr.hpp>
#include <string>

namespace sb
{
    OpenGLContext::OpenGLContext(GLFWwindow* arg_window_handle, Window* in_window)
        : m_glWindow_handle(arg_window_handle), m_window_handle(in_window)
    {
        // Assert 필요.
    }

    void OpenGLContext::Initialize()
    {
        glfwMakeContextCurrent(m_glWindow_handle);
        int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

        // Assert 및 버전 체크, 정보 로그 출력 필요.
        {
            // VAO
            m_vertexBuffer = CreateUPtr<OpenglVertexBuffer>();

            // geometry(VBO)
            m_vertexObjectBuffer = CreateUPtr<OpenglBuffer>();
            m_vertexObjectBuffer->CreateBuffer(OpenglBufferType::VBO);
            m_vertexObjectBuffer->BindBuffer(GL_ARRAY_BUFFER);
            m_vertexObjectBuffer->AddData(cubeVertices, 1);
            m_vertexObjectBuffer->CommitData(GL_STATIC_DRAW);

            m_vertexBuffer->SetAttribute(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), 0);

            // color
            m_colorBuffer = CreateUPtr<OpenglBuffer>();
            m_colorBuffer->CreateBuffer(OpenglBufferType::VBO);
            m_colorBuffer->BindBuffer(GL_ARRAY_BUFFER);
            m_colorBuffer->AddData(cubeFaceColors, 1);
            m_colorBuffer->CommitData(GL_STATIC_DRAW);

            m_vertexBuffer->SetAttribute(1, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), 0);

            // EBO
            m_indexBuffer = CreateUPtr<OpenglBuffer>();
            m_indexBuffer->CreateBuffer(OpenglBufferType::EBO);
            m_indexBuffer->BindBuffer(GL_ELEMENT_ARRAY_BUFFER);
            m_indexBuffer->AddData(cubeVertexIndex, 1);
            m_indexBuffer->CommitData(GL_STATIC_DRAW);

            // Create programs.
            m_programs.emplace_back((OpenGLProgram::Create("../../resources/shader/simple.vs",
                                                           "../../resources/shader/simple.fs")));
        }

        // Use all programs.
        BOOST_FOREACH (const auto& program, m_programs)
        {
            program->Use();
        }

       glEnable(GL_DEPTH_TEST);

       glClearColor(m_clearColor.x, m_clearColor.y, m_clearColor.z, m_clearColor.w);
    }

    void OpenGLContext::SwapBuffers()
    {
        glfwSwapBuffers(m_glWindow_handle);
    }

    void OpenGLContext::Render()
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        std::vector<glm::vec3> cubePositions = {
            glm::vec3(0.0f, 0.0f, 0.0f),     glm::vec3(2.0f, 5.0f, -15.0f), glm::vec3(-1.5f, -2.2f, -2.5f),
            glm::vec3(-3.8f, -2.0f, -12.3f), glm::vec3(2.4f, -0.4f, -3.5f), glm::vec3(-1.7f, 3.0f, -7.5f),
            glm::vec3(1.3f, -2.0f, -2.5f),   glm::vec3(1.5f, 2.0f, -2.5f),  glm::vec3(1.5f, 0.2f, -1.5f),
            glm::vec3(-1.3f, 1.0f, -1.5f),
        };

        if (!m_window_handle)
        {
            return;
        }

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

        // camera
        WinsWindow* window = static_cast<WinsWindow*>(m_window_handle);
        window->m_cameraFront =
            glm::rotate(glm::mat4(1.0f), glm::radians(window->m_cameraYaw), glm::vec3(0.0f, 1.0f, 0.0f)) *
            glm::rotate(glm::mat4(1.0f), glm::radians(window->m_cameraPitch), glm::vec3(1.0f, 0.0f, 0.0f)) *
            glm::vec4(0.0f, 0.0f, -1.0f, 0.0f);

        const WinWindowData& WinData = window->GetWindowData();

        const glm::mat4 projection =
            glm::perspective(glm::radians(45.0f), (float)WinData.width / (float)WinData.height, 0.01f, 20.0f);
        const glm::mat4 view = glm::lookAt(window->m_cameraPos, window->m_cameraPos + window->m_cameraFront, window->m_cameraUp);
        // ~camera

        for (size_t i = 0; i < cubePositions.size(); i++)
        {
            auto& pos = cubePositions[i];
            auto model = glm::translate(glm::mat4(1.0f), pos);
            model = glm::rotate(model, glm::radians((float)glfwGetTime() * 120.0f + 20.0f * (float)i),
                                glm::vec3(1.0f, 0.5f, 0.0f));
            auto transform = projection * view * model;

            m_programs[0].get()->SetUniform("transform", transform);
            glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
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