#ifndef BPM_HPP_
#define BPM_HPP_

#include <cmath>
#include <cstdlib>
#include <ctime>
#include <iomanip>
#include <pthread.h>
#include <sstream>
#include <string>
#include <unistd.h>

#include "API/BPM/BPM.hpp"
#include "backend/other/defines.hpp"

class BPM_
{
public:
    ~BPM_();
    inline static void setCurrentBeatToNow()
    {
        currentBeat = std::chrono::high_resolution_clock::now();
        newBeat     = true;
    }
    static void createBPMThreads();
    static void joinBPMThreads();

private:
    static void              *BPMUpdateHandler(void *args);
    static void              *BPMButtonStateUpdater(void *args);
    inline static bool        newBeatHandler();
    inline static std::string floatToString(float value)
    {
        std::ostringstream stream;
        stream << std::fixed << std::setprecision(2) << value;
        return stream.str();
    }

private:
    inline static std::chrono::time_point<std::chrono::high_resolution_clock> currentBeat;
    inline static std::chrono::time_point<std::chrono::high_resolution_clock> lastBeat =
        std::chrono::high_resolution_clock::now();
    inline static long long       durationUS;
    inline static pthread_t       bpmThread;
    inline static pthread_t       buttonStateThread;
    inline static pthread_mutex_t mutex           = PTHREAD_MUTEX_INITIALIZER;
    inline static bool            newBeat         = false;
    inline static uint16_t        continuousBeats = 0;
    inline static long long       averageBeatDurationUS;  // calculate bpm with: 60/avgBeatDur
    inline static long long       BPMIntTimesHun           = 0;
    inline static long long       maxBeatOffsetToleranceUS = 75000;
    inline static bool            threadsShouldJoin        = false;
    inline static std::string     BPMString;
    inline static long long       buttonBlinkDurationUS = 100000;
    inline static bool            buttonState           = false;
    inline static std::chrono::time_point<std::chrono::high_resolution_clock> buttonCurrentBeat;
    inline static long long                                                   buttonBeatDurationUS = 0;
};

#endif  // BPM_HPP_
