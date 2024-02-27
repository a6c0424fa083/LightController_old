#ifndef BASEWINDOW_HPP
#define BASEWINDOW_HPP

#include <imgui.h>

#include "backend/other/defines.hpp"



class BaseWindow
{
public:
    inline static ImVec4 default_ImGuiCol_TableBorderStrong = ImVec4(1.0, 1.0, 1.0, 1.0);
    inline static ImVec4 default_ImGuiCol_TableBorderLight  = ImVec4(1.0, 1.0, 1.0, 1.0);
    inline static ImVec4 default_ImGuiCol_Button            = ImVec4(0.11F, 0.11F, 0.11F, 1.0F);
    inline static ImVec4 default_ImGuiCol_ButtonHovered     = ImVec4(0.14F, 0.14F, 0.14F, 1.0F);
    inline static ImVec4 default_ImGuiCol_ButtonActive      = ImVec4(0.23F, 0.23F, 0.23F, 1.0F);
    inline static ImVec4 default_ImGuiCol_WindowBg          = ImVec4(0.0F, 0.0F, 0.0F, 1.0F);
    inline static ImVec4 default_ImGuiCol_ChildBg           = ImVec4(0.0F, 0.0F, 0.0F, 1.0F);
    inline static ImVec4 default_ImGuiCol_FrameBg           = ImVec4(0.11F, 0.11F, 0.11F, 1.0F);
    inline static ImVec2 default_WindowPadding              = ImVec2(0.0F, 0.0F);
    inline static ImVec2 default_FramePadding               = ImVec2(0.0F, 0.0F);
    inline static float  default_WindowBorderSize           = 1.0F;
    inline static float  default_WindowRounding             = 5.0F;
    inline static float  default_FrameBorderSize            = 1.0F;
    inline static float  default_FrameRounding              = 5.0F;
    inline static float  default_ChildBorderSize            = 1.0F;
    inline static float  default_ChildRounding              = 5.0F;

    inline static float fontSize     = 0.5F;
    inline static float saveMargin   = 5.0F;
    inline static float borderMargin = (2.0F / 5.0F) * saveMargin;

public:
    // inline void        draw_border(ImVec2 pos, ImVec2 size) const;
    static inline void setWindowPosSize(ImVec2 pos, ImVec2 size)
    {
        ImGui::SetNextWindowPos(pos);
        ImGui::SetNextWindowSize(size);
    }
};



#endif  // BASEWINDOW_HPP
