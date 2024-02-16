#ifndef ARDUINOAUDIOINPUT_HPP
#define ARDUINOAUDIOINPUT_HPP

#include <cstdlib>
#include <utility>
#include <vector>

class ArduinoAudioInput
{
public:
    inline static size_t getIntervalSize() { return intervalSize; }
    inline static void   setIntervalSize(size_t newIntervalSize) { intervalSize = newIntervalSize; }

    inline static std::vector<uint16_t> getAudioDataL() {return audioDataL;}
    inline static std::vector<uint16_t> getAudioDataR() {return audioDataR;}

    inline static void setAudioDataL(std::vector<uint16_t> newAudioDataL) {audioDataL = std::move(newAudioDataL);}
    inline static void setAudioDataR(std::vector<uint16_t> newAudioDataR) {audioDataL = std::move(newAudioDataR);}

private:
    static size_t                intervalSize;
    static std::vector<uint16_t> audioDataL;
    static std::vector<uint16_t> audioDataR;
};

#endif  // ARDUINOAUDIOINPUT_HPP
