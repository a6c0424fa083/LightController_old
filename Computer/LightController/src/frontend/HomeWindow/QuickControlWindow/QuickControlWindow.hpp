#ifndef QUICKCONTROL_HPP
#define QUICKCONTROL_HPP

#include <imgui.h>

#include "frontend/other/BaseWindow/BaseWindow.hpp"
#include "backend/ArduinoCommunication/ArduinoCommunication.hpp"

class QuickControlWindow : BaseWindow
{
public:
    QuickControlWindow(ImVec2 pos, ImVec2 size);

    void Draw();

    static ImVec2 calcWindowSize();

private:
    void DrawContent();

private:
    ImVec2 pos;
    ImVec2 size;

    std::vector<uint8_t> dmxValues = std::vector<uint8_t>(ArduinoCommunication::getDMXChannelCount(), 0);
};

#endif  // QUICKCONTROL_HPP
