#include "OpenGLContext.h"

#include "Core/Common.h"
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

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);
    }
} // namespace sb