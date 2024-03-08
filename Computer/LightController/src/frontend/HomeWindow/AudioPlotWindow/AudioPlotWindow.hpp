#ifndef AUDIOPLOTWINDOW_HPP
#define AUDIOPLOTWINDOW_HPP

#include <imgui.h>
#include <implot.h>

#include "backend/ArduinoCommunication/ArduinoCommunication.hpp"
#include "frontend/other/BaseWindow/BaseWindow.hpp"

class AudioPlotWindow : BaseWindow
{
public:
    AudioPlotWindow(ImVec2 pos, ImVec2 size);

    void Draw();

private:
    void DrawContent();

private:
    ImVec2 pos;
    ImVec2 size;
};

#endif  // AUDIOPLOTWINDOW_HPP
