#include "FrontWindow.h"
#include "imgui.h"

namespace sb
{
    FrontWindow::FrontWindow(const WindowContext& in_windowContext) : WinsWindow(in_windowContext)
    {
    }

    FrontWindow::FrontWindow(const WindowContext& in_windowContext, Application* in_app)
        : WinsWindow(in_windowContext, in_app)
    {
    }

    void FrontWindow::ImGuiUpdate()
    {
        ImGui::Begin("FrontWindow", 0);
        ImGui::Text("Hello, world!");
        static int e = 0;
        ImGui::RadioButton("radio a", &e, 0);
        ImGui::SameLine();
        ImGui::RadioButton("radio b", &e, 1);
        ImGui::SameLine();
        ImGui::RadioButton("radio c", &e, 2);
        ImGui::End();
    }
} // namespace sb