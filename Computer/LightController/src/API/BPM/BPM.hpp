#ifndef BPM_HPP
#define BPM_HPP

#include <imgui.h>
#include <pthread.h>
#include <string>

class BPM
{
public:
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

private:
    static std::string            bpm;
    static bool                   buttonShouldBlink;
    inline static pthread_mutex_t bpmMutex    = PTHREAD_MUTEX_INITIALIZER;
    inline static pthread_mutex_t buttonMutex = PTHREAD_MUTEX_INITIALIZER;
};

#endif  // BPM_HPP
