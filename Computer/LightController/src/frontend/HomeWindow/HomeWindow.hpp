#ifndef HOMEWINDOW_HPP
#define HOMEWINDOW_HPP

#include <imgui.h>

#include "frontend/HomeWindow/AudioPlotWindow/AudioPlotWindow.hpp"
#include "frontend/other/BaseWindow/BaseWindow.hpp"

class HomeWindow : BaseWindow
{
public:
    HomeWindow() = delete;
    inline HomeWindow(ImVec2 pos, ImVec2 size) : pos(pos), size(size) {};

    void Draw();

    [[nodiscard]] inline ImVec2 getSize() const { return size; }
    [[nodiscard]] inline ImVec2 getPos() const { return pos; }

private:
    void DrawContent();

private:
    // HomeWindow
    ImVec2 pos;
    ImVec2 size;

    // AudioPlotWindow
    ImVec2 audioPlotSize = ImVec2(400.0F, 240.0F);
    ImVec2 audioPlotPos = ImVec2(io_width - ((2 * saveMargin) + audioPlotSize.x), io_height - size.y);

    AudioPlotWindow *audioPlotWindow = new AudioPlotWindow(audioPlotPos, audioPlotSize);
};

#endif  // HOMEWINDOW_HPP
