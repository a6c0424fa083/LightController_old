#include "BPMWindow.hpp"

void BPMWindow::Draw()
{
    PUSH_FONT(SUBTITLE)
    setWindowPosSize(pos, size);
    ImGui::Begin("BPMWindow", nullptr, STATIC__NO_VISUALS);
    DrawContent();
    ImGui::End();
    POP_FONT()
}

void BPMWindow::DrawContent()
{
    PUSH_FONT(NUMBER)
    float _halfSizeYNumberFontOffset = (size.y / 2.0F) - (ImGui::CalcTextSize("888.8").y / 2);
    ImGui::SetCursorPos(ImVec2(saveMargin, _halfSizeYNumberFontOffset));
    ImGui::Text("121.0");
    POP_FONT()
    ImGui::SetCursorPos(ImVec2(size.x - (ImGui::CalcTextSize("BPM").x + (6 * saveMargin)), saveMargin));
    ImGui::Button("BPM", ImVec2(ImGui::CalcTextSize("BPM").x + (5 * saveMargin), ImGui::CalcTextSize("BPM").y + (3 * saveMargin)));
}

ImVec2 BPMWindow::calcWindowSize()
{
    ImVec2 _size;
    PUSH_FONT(SUBTITLE)
    setWindowPosSize(ImVec2(0.0F, 0.0F), ImVec2(200.0F, 200.0F));
    ImGui::Begin("_BPMWindow", nullptr, STATIC__NO_VISUALS);
    _size = ImVec2(ImVec2(ImGui::CalcTextSize("888.8").x + ImGui::CalcTextSize("BPM").x + (8 * saveMargin),
                          ImGui::CalcTextSize("BPM").y + (5 * saveMargin)));
    ImGui::End();
    POP_FONT()
    return _size;
}