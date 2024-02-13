//
// Created by Jannis Sauer on 13.02.24.
//

#include "PageWindow.hpp"

void PageWindow::Draw()
{
    PUSH_FONT(SUBTITLE)
    size = ImVec2(io_width, 4.0F * saveMargin + ImGui::CalcTextSize("XXX").y);
    setWindowPosSize(pos, size);
    style.WindowBorderSize = 0.0F;
    style.WindowRounding   = 0.0F;
    style.FrameBorderSize  = 0.0F;
    style.FrameRounding    = 0.0F;
    style.ChildBorderSize  = 0.0F;
    style.ChildRounding    = 0.0F;
    ImGui::Begin("HeaderWindow", nullptr, STATIC__NO_VISUALS);
    style.WindowBorderSize = default_WindowBorderSize;
    style.WindowRounding   = default_WindowRounding;
    style.FrameBorderSize  = default_FrameBorderSize;
    style.FrameRounding    = default_FrameRounding;
    style.ChildBorderSize  = default_ChildBorderSize;
    style.ChildRounding    = default_ChildRounding;
    DrawContents();
    ImGui::End();
    POP_FONT()
}



void PageWindow::DrawContents()
{
    // draw borderlines
    ImGui::GetWindowDrawList()->AddLine(ImVec2(borderMargin, pos.y + borderMargin),
                                        ImVec2(io_width - borderMargin, pos.y + borderMargin),
                                        ImGui::GetColorU32(ImGuiCol_TextDisabled),
                                        0.2F);

    ImGui::GetWindowDrawList()->AddLine(ImVec2(borderMargin, pos.y + size.y - borderMargin - 1.0F /*line thickness*/),
                                        ImVec2(io_width - borderMargin, pos.y + size.y - borderMargin),
                                        ImGui::GetColorU32(ImGuiCol_TextDisabled),
                                        0.2F);

    // draw individual buttons leading to the main 'areas' of the program
    for (size_t i = 0; i < titles->size(); i++)
    {
        // set new cursor position based on i
        ImGui::SetCursorPos(
            ImVec2((static_cast<float>(i) + 1.0F) * saveMargin +
                       static_cast<float>(i) * ((io_width - (static_cast<float>(titles->size()) + 1.0F) * saveMargin) /
                                                static_cast<float>(titles->size())),
                   saveMargin));

        // draw button using index i of header_titles vector
        if (ImGui::Button(
                titles->at(i).c_str(),
                ImVec2((io_width - (static_cast<float>(titles->size()) + 1.0F) * saveMargin) / static_cast<float>(titles->size()),
                       ImGui::CalcTextSize("XXX").y + 2.0F * saveMargin)))
            PageSelect::setActivePageID(i);
    }
}