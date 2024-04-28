#include "OpenglContext.h"

#include <windows.h> // supress warning C4005 : 'APIENTRY' 매크로 재정의
#include <glad/gl.h> // glfw3 앞에 위치 해야 함
#include <GLFW/glfw3.h>

#include "Core/Application.h"
#include "Core/Common.h"
#include "Core/WinWindow.h"
#include "Render/BasicGeometry.h"
#include "Render/Shader.h"
#include <boost/foreach.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <imgui/imgui.h>
#include <spdlog/spdlog.h>
#include <string>
#include "Render/RenderResource.h"

#include "Actor/CubeActor.h"

namespace sb
{
    namespace OpenglContextConstants
    {
        constexpr float zNear = 0.1f;
        constexpr float zFar = 100.f;
    }

    OpenglContext::OpenglContext(GLFWwindow* arg_window_handle, Window* in_window)
        : m_glWindow_handle(arg_window_handle), m_window_handle(in_window)
    {
        // Assert 필요.
        m_graphicsFramework = GraphicsLibrary::OpenGL;
    }

    void OpenglContext::Initialize()
    {
        glfwMakeContextCurrent(m_glWindow_handle);
        int status = gladLoadGL(glfwGetProcAddress);

        m_window_handle->m_actors.emplace_back(CreateUPtr<CubeActor>());

        for (auto& actor : m_window_handle->m_actors)
        {
            actor->DrawActor();
        }

        // Assert 및 버전 체크, 정보 로그 출력 필요.
        {
            for (RenderResource* RenderResource : m_targetRenderResources)
            {
                RenderResource->RenderProcess(this);
            }

            this->m_staticMeshData;

            // texture
            UPtr<Image> image1 = Image::Load("../../resources/texture/container.jpg");
            if (!image1)
            {
                SPDLOG_INFO("Image load failed");
                return;
            }

            // materials
            UPtr<Material> newMaterial = CreateUPtr<Material>();
            newMaterial->m_diffuse =
                OpenglTexture::CreateTextureFromImage(Image::Load("../../resources/texture/container2.png").get());
            newMaterial->m_specular =
                OpenglTexture::CreateTextureFromImage(Image::Load("../../resources/texture/container2_specular.png").get());

            m_materials.emplace_back(std::move(newMaterial));

            // Create programs.
            // 1.cube
            // m_programs.emplace(
            //     "Cube", OpenglProgram::Create("../../resources/shader/simple.vs", "../../resources/shader/simple.fs"));

            // m_texture = OpenglTexture::CreateTextureFromImage(image1.get());
            // glActiveTexture(GL_TEXTURE0);
            // glBindTexture(GL_TEXTURE_2D, m_texture->Get());

            // 2.cube - light[UBO]
            m_programs.emplace("Light", OpenglProgram::Create("../../resources/shader/simple3.vert",
                                                              "../../resources/shader/simple3.frag"));

            auto& targetShader = m_programs["Light"]->m_shaders[0];
            GLuint programId = m_programs["Light"]->Get();
            GLuint UBOIndex = glGetUniformBlockIndex(programId, "MatrixBlock");

            glUniformBlockBinding(programId, UBOIndex, 0);

            m_UniformBlockBuffer = CreateUPtr<OpenglUniformBuffer>(static_cast<uint32>(2 * sizeof(glm::mat4)));
            m_UniformBlockBuffer->BindBuffer(GL_UNIFORM_BUFFER);
            m_UniformBlockBuffer->CommitData(GL_STATIC_DRAW);
            // ~UBO
        }

        for (const auto& [name, program] : m_programs)
        {
            program->Use();
        }

       glEnable(GL_DEPTH_TEST);

       glClearColor(m_clearColor.x, m_clearColor.y, m_clearColor.z, m_clearColor.w);
    }

    void OpenglContext::SwapBuffers()
    {
        glfwSwapBuffers(m_glWindow_handle);
    }

    void OpenglContext::CreateBuffer()
    {
    }

    void OpenglContext::BindBuffer()
    {
    }

    void OpenglContext::AddData(const void * in_data)
    {
    }

    void OpenglContext::CommitData()
    {
    }

    void OpenglContext::VertexProcess(SPtr<VertexRenderResource>& in_vertexResource)
    {
        // ??
    }

    void OpenglContext::StaticMeshProcess(SPtr<StaticMeshRenderResource>& in_staticMeshResource)
    {
        // ??
    }

    void OpenglContext::Render()
    {
        UpdateFPS();

        const float deltaTick = m_elapsedTime * 0.001;

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
        if (ImGui::Begin("opengl setting"))
        {
            ImGui::Text("This is smaple text...");

            if (ImGui::ColorEdit4("clearColor", glm::value_ptr(m_clearColor)))
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

        const glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)WinData.width / (float)WinData.height,
                                                      OpenglContextConstants::zNear, OpenglContextConstants::zFar);
        const glm::mat4 view = glm::lookAt(window->m_cameraPos, window->m_cameraPos + window->m_cameraFront, window->m_cameraUp);
        // ~camera

        // lights
        m_programs["Light"]->Use();
        m_programs["Light"]->SetUniform("viewPos", window->m_cameraPos);
        m_programs["Light"]->SetUniform("directionalLight.direction", glm::vec3(-0.2f, -1.f, -0.3f));
        m_programs["Light"]->SetUniform("directionalLight.diffuse", glm::vec3(0.5f, 0.5f, 0.5f));
        m_programs["Light"]->SetUniform("directionalLight.specular", glm::vec3(1.f, 0.6f, 0.6f));

        // materials
        m_programs["Light"]->SetUniform("material.shiniess", m_materials[0]->shininess);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, m_materials[0]->m_diffuse->Get());
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, m_materials[0]->m_specular->Get());

        // cubes
        // m_programs["Cube"]->Use();
        // glActiveTexture(GL_TEXTURE0);
        // glBindTexture(GL_TEXTURE_2D, m_texture->Get());

        for (size_t i = 0; i < cubePositions.size(); i++)
        {
            for (const auto& actor : m_window_handle->m_actors)
            {
                if (actor.get())
                {
                    actor->Tick(deltaTick);
                }
            }

            auto& pos = cubePositions[i];
            auto model = glm::translate(glm::mat4(1.0f), pos);
            model = glm::rotate(model, glm::radians((float)glfwGetTime() * 120.0f + 20.0f * (float)i),
                                glm::vec3(1.0f, 0.5f, 0.0f));
            auto transform = projection * view * model;

            // UBO
            GLuint UBOId = m_UniformBlockBuffer->Get();
            glBindBuffer(GL_UNIFORM_BUFFER, UBOId);
            glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(transform));
            glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(model));

            glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
        }
    }

    UPtr<OpenglShader> OpenglContext::CreateShader(const std::string& in_vsFilename, const std::string& in_fsFilename)
    {
        auto Shader = UPtr<OpenglShader>(new OpenglShader());
        Shader->LoadFile(in_vsFilename, GL_VERTEX_SHADER);
        Shader->LoadFile(in_fsFilename, GL_FRAGMENT_SHADER);
        Shader->CreateProgram();

        return std::move(Shader);
    }

    void OpenglContext::UpdateFPS()
    {
        static int32 m_frameCount = 0;
        static double m_accTime = 0;
        static double m_lastTime = 0.0;
        static double m_currentTime = 0.0;

        m_currentTime = GetSystemTime();
        m_elapsedTime = m_currentTime - m_lastTime;
        m_accTime += m_elapsedTime;
        m_frameCount++;

        if ((m_accTime * 0.001) > 1.0)
        {
            m_fps = m_frameCount;
            m_frameCount = 0;
            m_accTime = 0.0;
        }

        m_lastTime = m_currentTime;
    }
} // namespace sb