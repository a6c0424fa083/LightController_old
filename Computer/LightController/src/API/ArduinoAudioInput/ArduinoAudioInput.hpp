#ifndef ARDUINOAUDIOINPUT_HPP
#define ARDUINOAUDIOINPUT_HPP

#include <cstdlib>
#include <vector>

class ArduinoAudioInput
{
public:
    inline static size_t getIntervalSize() { return intervalSize; }
    inline static void   setIntervalSize(size_t newIntervalSize) { intervalSize = newIntervalSize; }

private:
    static size_t                intervalSize;
    static std::vector<uint16_t> audioData;
};

#endif  // ARDUINOAUDIOINPUT_HPP
