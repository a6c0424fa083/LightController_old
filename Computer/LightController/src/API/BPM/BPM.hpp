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
        pthread_mutex_lock(&mutex);
        bpm = newBPM;
        pthread_mutex_unlock(&mutex);
    }
    inline static std::string getBPM()
    {
        std::string temp;
        pthread_mutex_lock(&mutex);
        temp = bpm;
        pthread_mutex_unlock(&mutex);
        return temp;
    }

private:
    static std::string            bpm;
    inline static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
};

#endif  // BPM_HPP
