#ifndef MASTERWINDOW_HPP
#define MASTERWINDOW_HPP

#include <string>
#include <vector>

#include "API/PageSelect/PageSelect.hpp"
#include "backend/other/defines.hpp"
#include "frontend/HomeWindow/HomeWindow.hpp"
#include "frontend/PageWindow/PageWindow.hpp"
#include "frontend/other/BaseWindow/BaseWindow.hpp"

class MasterWindow : BaseWindow
{
public:
    ~MasterWindow();

    void        Draw(ImVec2 pos, ImVec2 size);
    inline void DrawContents();

private:
    std::vector<std::string> titles     = { "Home", "Patch", "Location", "Effects", "Library", "Settings" };
    PageWindow              *pageWindow = new PageWindow(ImVec2(0.0F, 0.0F), &titles);
    HomeWindow              *homeWindow = nullptr;
};

#endif  // MASTERWINDOW_HPP
