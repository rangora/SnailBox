#include "FrontWindow.h"
#include "imgui.h"

namespace sb
{
    void FrontWindow::Run()
    {
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO();

        unsigned char* tex_pixels = nullptr;
        int tex_w, tex_h;
        io.Fonts->GetTexDataAsRGBA32(&tex_pixels, &tex_w, &tex_h);

        for (int n = 0; n < 20; n)
        {
            io.DisplaySize = ImVec2(1920, 1080);
            io.DeltaTime = 1.0f / 60.0f;
            ImGui::NewFrame();
            ImGui::Begin("FrontWindow");
            ImGui::Text("Hello, world!");
            static int e = 0;
            ImGui::RadioButton("radio a", &e, 0);
            ImGui::SameLine();
            ImGui::RadioButton("radio b", &e, 1);
            ImGui::SameLine();
            ImGui::RadioButton("radio c", &e, 2);
            ImGui::End();
            ImGui::Render();
        }

        ImGui::DestroyContext();
    }
} // namespace sb