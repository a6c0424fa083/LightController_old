#ifndef BPM_HPP
#define BPM_HPP

//#include <GLFW/glfw3.h>
#include <cmath>
#include <cstdlib>
#include <ctime>
//#include <imgui.h>
#include <iomanip>
#include <pthread.h>
#include <sstream>
#include <string>
#include <unistd.h>

#include "backend/other/defines.hpp"

class BPM
{
public:
    inline static void setCurrentBeatToNow()
    {
        currentBeat = std::chrono::high_resolution_clock::now();
        newBeat     = true;
    }
    static void createBPMThreads(/*GLFWwindow *_window*/);
    static void joinBPMThreads();

    inline static void setBPM(std::string &newBPM)
    {
        pthread_mutex_lock(&bpmMutex);
        bpm = newBPM;
        pthread_mutex_unlock(&bpmMutex);
    }
    inline static std::string getBPM()
    {
        std::string temp;
        pthread_mutex_lock(&bpmMutex);
        temp = bpm;
        pthread_mutex_unlock(&bpmMutex);
        return temp;
    }

    inline static void setButtonShouldBlink(bool state)
    {
        pthread_mutex_lock(&buttonMutex);
        buttonShouldBlink = state;
        pthread_mutex_unlock(&buttonMutex);
    }
    inline static bool getButtonShouldBlink()
    {
        bool temp;
        pthread_mutex_lock(&buttonMutex);
        temp = buttonShouldBlink;
        pthread_mutex_unlock(&buttonMutex);
        return temp;
    }

    inline static void setKeyPressed(bool state) { keyPressed = state; }
    inline static bool getKeyPressed()
    {
        bool temp;
        temp = keyPressed;
        return temp;
    }

private:
    // static void              *BPMKeyUpdateHandler(void *args);
    static void              *BPMClickUpdateHandler(void *args);
    static void              *BPMButtonStateUpdater(void *args);
    static bool               newBeatHandler();
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
    inline static pthread_t       bpmClickThread;
    inline static pthread_t       bpmKeyThread;
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

    inline static std::string            bpm = "XXX.XX";
    inline static bool                   buttonShouldBlink = false;
    inline static bool                   keyPressed = false;
    inline static pthread_mutex_t bpmMutex    = PTHREAD_MUTEX_INITIALIZER;
    inline static pthread_mutex_t buttonMutex = PTHREAD_MUTEX_INITIALIZER;
};

#endif  // BPM_HPP
