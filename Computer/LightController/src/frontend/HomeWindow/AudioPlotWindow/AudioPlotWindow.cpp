#include "AudioPlotWindow.hpp"

AudioPlotWindow::AudioPlotWindow(ImVec2 pos, ImVec2 size) : pos(pos), size(size) {}

void AudioPlotWindow::Draw()
{
    PUSH_FONT(NUMBER)
    setWindowPosSize(pos, size);
    ImGui::Begin("AudioPlotWindow", nullptr, STATIC__NO_VISUALS);
    DrawContent();
    ImGui::End();
    POP_FONT()
}

void AudioPlotWindow::DrawContent()
{
    ImGui::SetCursorPos(ImVec2(0.0F, 0.0F));
    ImPlot::SetNextAxisLimits(ImAxis_X1, 0.0, static_cast<double>(ArduinoAudioInput::getIntervalSize() - 1));
    ImPlot::SetNextAxisLimits(ImAxis_Y1, 0.0, 1023.0);
    ImPlot::BeginPlot("AudioPlotGraph",
                      ImVec2(size.x, size.y),
                      ImPlotFlags_NoBoxSelect | ImPlotFlags_NoInputs | ImPlotFlags_NoMouseText | ImPlotFlags_NoTitle |
                          ImPlotFlags_CanvasOnly);

    ImPlot::PlotLine("Left", ArduinoAudioInput::getAudioDataL().data(), static_cast<int>(ArduinoAudioInput::getIntervalSize()));
    ImPlot::PlotLine("Right", ArduinoAudioInput::getAudioDataR().data(), static_cast<int>(ArduinoAudioInput::getIntervalSize()));
    ImPlot::EndPlot();
}
