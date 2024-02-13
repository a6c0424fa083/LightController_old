#ifndef PAGEWINDOW_HPP
#define PAGEWINDOW_HPP

#include <imgui.h>
#include <string>
#include <vector>

#include "API/PageSelect/PageSelect.hpp"
#include "frontend/other/BaseWindow/BaseWindow.hpp"

class PageWindow : BaseWindow
{
public:
    PageWindow() = delete;
    PageWindow(ImVec2 pos, std::vector<std::string> *titles) : pos(pos), titles(titles) {}

    void          Draw();
    inline ImVec2 getSize() const { return size; }
    inline ImVec2 getPos() const { return pos; }

private:
    inline void DrawContents();

private:
    ImVec2                    size;
    ImVec2                    pos;
    std::vector<std::string> *titles;
};

#endif  // PAGEWINDOW_HPP
