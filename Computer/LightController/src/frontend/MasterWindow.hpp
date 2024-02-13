#ifndef MASTERWINDOW_HPP
#define MASTERWINDOW_HPP

#include <string>
#include <vector>

#include "backend/other/defines.hpp"
#include "frontend/other/BaseWindow/BaseWindow.hpp"

class MasterWindow : BaseWindow
{
public:
    void        Draw(ImVec2 pos, ImVec2 size);
    inline void DrawContents();
};

#endif  // MASTERWINDOW_HPP
