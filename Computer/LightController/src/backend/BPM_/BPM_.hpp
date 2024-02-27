#ifndef BPM_HPP_
#define BPM_HPP_

#include <cmath>
#include <cstdlib>
#include <ctime>
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
        currentBeat = clock();
        newBeat     = true;
    }
    static void createBPMThread();
    static void joinBPMThread();

private:
    static void       *BPMUpdateHandler(void *args);
    inline static bool newBeatHandler();

private:
    inline static clock_t         currentBeat;
    inline static clock_t         lastBeat = clock();
    inline static clock_t         duration;
    inline static pthread_t       bpmThread;
    inline static pthread_mutex_t mutex                  = PTHREAD_MUTEX_INITIALIZER;
    inline static bool            newBeat                = false;
    inline static uint16_t        continuousBeats        = 0;
    inline static float           averageBeatDuration    = 0.0F;  // calculate bpm with: 60/avgBeatDur
    inline static uint16_t        BPMIntTimesTen         = 0;
    inline static float           maxBeatOffsetTolerance = 0.03F;
    inline static uint8_t         conversionTemp;
    inline static bool            threadShouldJoin = false;
    inline static std::string     BPM;
};

#endif  // BPM_HPP_
