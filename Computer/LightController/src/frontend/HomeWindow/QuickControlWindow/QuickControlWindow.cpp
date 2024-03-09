#include "QuickControlWindow.hpp"
#include "frontend/HomeWindow/AudioPlotWindow/AudioPlotWindow.hpp"

QuickControlWindow::QuickControlWindow(ImVec2 pos, ImVec2 size) : pos(pos), size(size) {}

void QuickControlWindow::Draw()
{
    PUSH_FONT(SUBTITLE)
    setWindowPosSize(pos, size);
    ImGui::BeginChild("QuickControl", size, true, STATIC__NO_VISUALS);
    DrawContent();
    ImGui::EndChild();
    POP_FONT()
}

void QuickControlWindow::DrawContent()
{
    ImGui::SetCursorPos(ImVec2(saveMargin, saveMargin));
    if (ImGui::Button("Black",
                      ImVec2(ImGui::CalcTextSize("Black").x + 5 * saveMargin, ImGui::CalcTextSize("Black").y + 3 * saveMargin)))
    {
        dmxValues = std::vector<uint8_t>(ArduinoCommunication::getDMXChannelCount(), 0);
        ArduinoCommunication::setDmxValues(dmxValues);
    }

    ImGui::SetCursorPos(ImVec2(ImGui::CalcTextSize("Black").x + 7 * saveMargin, saveMargin));
    if (ImGui::Button("White",
                      ImVec2(ImGui::CalcTextSize("White").x + 5 * saveMargin, ImGui::CalcTextSize("White").y + 3 * saveMargin)))
    {
        dmxValues = std::vector<uint8_t>(ArduinoCommunication::getDMXChannelCount(), 255);
        ArduinoCommunication::setDmxValues(dmxValues);
    }
}

ImVec2 QuickControlWindow::calcWindowSize()
{
    ImVec2 _size;
    PUSH_FONT(SUBTITLE)
    setWindowPosSize(ImVec2(0.0F, 0.0F), ImVec2(200.0F, 200.0F));
    ImGui::Begin("_QuickControl", nullptr, STATIC__NO_VISUALS);
    _size = ImVec2(ImGui::CalcTextSize("Black").x + ImGui::CalcTextSize("White").x + 13 * saveMargin,
                   ImGui::CalcTextSize("XXX").y + 5 * saveMargin);
    ImGui::End();
    POP_FONT()
    return _size;
}
