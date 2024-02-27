#include "BPMWindow.hpp"

void BPMWindow::Draw()
{
    PUSH_FONT(SUBTITLE)
    setWindowPosSize(pos, size);
    ImGui::BeginChild("BPMWindow", size, true, STATIC__NO_VISUALS);
    DrawContent();
    ImGui::EndChild();
    POP_FONT()
}

void BPMWindow::DrawContent()
{
    ImGui::SetCursorPos(ImVec2(size.x - (ImGui::CalcTextSize("BPM").x + (6 * saveMargin)), saveMargin));
    if (ImGui::Button("BPM", ImVec2(ImGui::CalcTextSize("BPM").x + (5 * saveMargin), ImGui::CalcTextSize("BPM").y + (3 * saveMargin))))
    {
        BPM_::setCurrentBeatToNow();
    }

    PUSH_FONT(NUMBER)
    float _halfSizeYNumberFontOffset = (size.y / 2.0F) - (ImGui::CalcTextSize("888.8").y / 2);
    if (BPM::getBPM().size() == 6)
        ImGui::SetCursorPos(ImVec2(saveMargin, _halfSizeYNumberFontOffset));
    else if (BPM::getBPM().size() == 5)
    {
        ImGui::SetCursorPos(ImVec2(saveMargin + ImGui::CalcTextSize("8").x, _halfSizeYNumberFontOffset));
    }
    else
    {
        fprintf(stderr, "Invalid BPM string length. Expected 6 or 5, got %zu instead.\n", BPM::getBPM().size());
    }
    ImGui::Text("%s", BPM::getBPM().c_str());
    POP_FONT()
}

ImVec2 BPMWindow::calcWindowSize()
{
    ImVec2 _size;
    PUSH_FONT(SUBTITLE)
    setWindowPosSize(ImVec2(0.0F, 0.0F), ImVec2(200.0F, 200.0F));
    ImGui::Begin("_BPMWindow", nullptr, STATIC__NO_VISUALS);
    _size = ImVec2(ImVec2(ImGui::CalcTextSize("888.88").x + ImGui::CalcTextSize("BPM").x + (8 * saveMargin),
                          ImGui::CalcTextSize("BPM").y + (5 * saveMargin)));
    ImGui::End();
    POP_FONT()
    return _size;
}