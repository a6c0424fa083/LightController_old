#include "MasterWindow.hpp"

void MasterWindow::Draw(ImVec2 pos, ImVec2 size)
{
    io.FontGlobalScale = fontSize;

    PUSH_FONT(SUBTITLE)
    setWindowPosSize(pos, size);
    style.WindowBorderSize = 0.0F;
    style.WindowRounding   = 0.0F;
    style.FrameBorderSize  = 0.0F;
    style.FrameRounding    = 0.0F;
    ImGui::Begin("MasterWindow", nullptr, STATIC__NO_VISUALS);
    style.WindowBorderSize = default_WindowBorderSize;
    style.WindowRounding   = default_WindowRounding;
    style.FrameBorderSize  = default_FrameBorderSize;
    style.FrameRounding    = default_FrameRounding;
    DrawContents();
    ImGui::End();
    POP_FONT()
}



void MasterWindow::DrawContents() { pageWindow->Draw(); }
