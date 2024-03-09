#ifndef HOMEWINDOW_HPP
#define HOMEWINDOW_HPP

#include <imgui.h>

#include "frontend/HomeWindow/AudioPlotWindow/AudioPlotWindow.hpp"
#include "frontend/HomeWindow/BPMWindow/BPMWindow.hpp"
#include "frontend/HomeWindow/QuickControlWindow/QuickControlWindow.hpp"
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
    ImVec2           audioPlotSize   = ImVec2(400.0F, 240.0F);
    ImVec2           audioPlotPos    = ImVec2(io_width - (saveMargin + audioPlotSize.x), io_height - size.y - saveMargin);
    AudioPlotWindow *audioPlotWindow = new AudioPlotWindow(audioPlotPos, audioPlotSize);

    // BPMWindow
    ImVec2 bpmWindowSize = BPMWindow::calcWindowSize();
    ImVec2 bpmWindowPos =
        ImVec2(io_width - (saveMargin + audioPlotSize.x + saveMargin + bpmWindowSize.x), io_height - size.y - saveMargin);
    BPMWindow *bpmWindow = new BPMWindow(bpmWindowPos, bpmWindowSize);

    // QuickControlWindow
    ImVec2 quickControlWindowSize = QuickControlWindow::calcWindowSize();
    ImVec2 quickControlWindowPos =
        ImVec2(io_width - (saveMargin + audioPlotSize.x + saveMargin + bpmWindowSize.x + saveMargin + quickControlWindowSize.x),
               io_height - size.y - saveMargin);
    QuickControlWindow *quickControlWindow = new QuickControlWindow(quickControlWindowPos, quickControlWindowSize);
};

#endif  // HOMEWINDOW_HPP
