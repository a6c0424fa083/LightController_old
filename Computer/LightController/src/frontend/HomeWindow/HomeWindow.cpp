#include "HomeWindow.hpp"

void HomeWindow::Draw()
{
    PUSH_FONT(NUMBER)
    setWindowPosSize(pos, size);
    ImGui::Begin("HomeWindow", nullptr, STATIC__NO_VISUALS);
    DrawContent();
    ImGui::End();
    POP_FONT()
}

void HomeWindow::DrawContent()
{
    ImGui::Text("Hello World!");
    audioPlotWindow->Draw();
}