#ifndef DEFINES_HPP
#define DEFINES_HPP

#include <imgui.h>

#define CLOCKS_PER_SEC_OWN CLOCKS_PER_SEC

#define MASTER_WINDOW_WIDTH  800
#define MASTER_WINDOW_HEIGHT 480

#define style ImGui::GetStyle()

#define io ImGui::GetIO()

#define io_width  io.DisplaySize.x
#define io_height io.DisplaySize.y

#define STATIC__NO_VISUALS                                                                                            \
    ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | \
        ImGuiWindowFlags_NoScrollbar

enum USER_FONT
{
    PAGE_FILLING,
    TITLE,
    SUBTITLE,
    TEXT,
    INFORMATION,
    NUMBER
};

#define PAGEFILLING io.Fonts->Fonts[UserFont::PAGE_FILLING]
#define TITLE       io.Fonts->Fonts[USER_FONT::TITLE]
#define SUBTITLE    io.Fonts->Fonts[USER_FONT::SUBTITLE]
#define TEXT        io.Fonts->Fonts[USER_FONT::TEXT]
#define INFORMATION io.Fonts->Fonts[USER_FONT::INFORMATION]
#define NUMBER      io.Fonts->Fonts[USER_FONT::NUMBER]

#define PUSH_FONT(FONT) ImGui::PushFont(FONT);

#define POP_FONT() ImGui::PopFont();

#define MONACO_FONT_PATH     "../data/fonts/Monaco.ttf"      // relative to executable
#define ARIALBLACK_FONT_PATH "../data/fonts/ArialBlack.ttf"  // relative to executable
#define ARIAL_FONT_PATH      "../data/fonts/Arial.ttf"       // relative to executable
#define ARIALBOLD_FONT_PATH  "../data/fonts/ArialBold.ttf"   // relative to executable

#define BEAT_BUTTON GLFW_KEY_B

#define ARDUINO_BAUD_RATE 115200

#endif  // DEFINES_HPP
