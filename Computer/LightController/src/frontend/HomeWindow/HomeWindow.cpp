#include "HomeWindow.hpp"

void HomeWindow::Draw()
{
    PUSH_FONT(NUMBER)
    setWindowPosSize(pos, size);
    style.WindowBorderSize = 0.0F;
    ImGui::Begin("HomeWindow", nullptr, STATIC__NO_VISUALS);
    style.WindowBorderSize = default_WindowBorderSize;
    DrawContent();
    ImGui::End();
    POP_FONT()
}

void HomeWindow::DrawContent()
{
    // ImGui::Text("Hello World!");
    audioPlotWindow->Draw();
    bpmWindow->Draw();
}