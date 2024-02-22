#include "MasterWindow.hpp"

MasterWindow::~MasterWindow()
{
    if (pageWindow != nullptr)
    {
        delete pageWindow;
        pageWindow = nullptr;
    }
    if (homeWindow != nullptr)
    {
        delete homeWindow;
        homeWindow = nullptr;
    }
}

void MasterWindow::Draw(ImVec2 pos, ImVec2 size)
{
    io.FontGlobalScale = fontSize;

    PUSH_FONT(SUBTITLE)
    setWindowPosSize(pos, size);
    style.WindowBorderSize = 0.0F;
    style.WindowRounding   = 0.0F;
    style.FrameBorderSize  = 0.0F;
    style.FrameRounding    = 0.0F;
    ImGui::Begin("MasterWindow", nullptr, STATIC__NO_VISUALS | ImGuiWindowFlags_NoBringToFrontOnFocus);
    style.WindowBorderSize = default_WindowBorderSize;
    style.WindowRounding   = default_WindowRounding;
    style.FrameBorderSize  = default_FrameBorderSize;
    style.FrameRounding    = default_FrameRounding;
    DrawContents();
    ImGui::End();
    POP_FONT()
}



void MasterWindow::DrawContents()
{
    pageWindow->Draw();

    // construction
    switch (PageSelect::getActivePageID())
    {
        case page::HOME:
            if (homeWindow == nullptr)
                homeWindow =
                    new HomeWindow(ImVec2(saveMargin, pageWindow->getPos().y + pageWindow->getSize().y + saveMargin),
                                   ImVec2(io_width - (2 * saveMargin),
                                          io_height - (pageWindow->getPos().y + pageWindow->getSize().y + (2 * saveMargin))));
            homeWindow->Draw();
            break;
        default:
            break;
    }

    // destruction
    if (PageSelect::getActivePageID() != page::HOME)
    {
        if (homeWindow != nullptr)
        {
            delete homeWindow;
            homeWindow = nullptr;
        }
    }
}
