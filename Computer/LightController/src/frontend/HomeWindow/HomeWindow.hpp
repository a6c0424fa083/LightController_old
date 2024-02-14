#ifndef HOMEWINDOW_HPP
#define HOMEWINDOW_HPP

#include <imgui.h>

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
    ImVec2 pos;
    ImVec2 size;
};

#endif  // HOMEWINDOW_HPP
