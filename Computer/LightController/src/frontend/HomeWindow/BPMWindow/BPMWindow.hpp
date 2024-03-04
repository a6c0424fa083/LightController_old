#ifndef BPMWINDOW_HPP
#define BPMWINDOW_HPP

#include <GLFW/glfw3.h>
#include <imgui.h>

#include "backend/BPM/BPM.hpp"
#include "frontend/other/BaseWindow/BaseWindow.hpp"

class BPMWindow : BaseWindow
{
public:
    BPMWindow(ImVec2 pos, ImVec2 size) : pos(pos), size(size) {}

    void Draw();

    static ImVec2 calcWindowSize();

private:
    void DrawContent() const;

private:
    ImVec2 pos;
    ImVec2 size = ImVec2(0.0F, 0.0F);
};

#endif  // BPMWINDOW_HPP
