//
// Created by Jannis Sauer on 15.02.24.
//

#include "ArduinoAudioInput.hpp"


size_t ArduinoAudioInput::intervalSize = 32;

std::vector<uint16_t> ArduinoAudioInput::audioDataL = std::vector<uint16_t>(intervalSize, 522);
std::vector<uint16_t> ArduinoAudioInput::audioDataR = std::vector<uint16_t>(intervalSize, 502);
