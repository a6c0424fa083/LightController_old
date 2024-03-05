#ifndef ARDUINOAUDIOINPUT_HPP
#define ARDUINOAUDIOINPUT_HPP

#include <cstdlib>
#include <fcntl.h>
#include <filesystem>
#include <iostream>
#include <pthread.h>
#include <string>
#include <termios.h>
#include <unistd.h>
#include <vector>

class ArduinoAudioInput
{
public:
    inline static size_t getAudioSampleSize() { return expectedAudioSampleSize; }
    inline static void   setAudioSampleSize(size_t newExpectedAudioSampleSize)
    {
        expectedAudioSampleSize = newExpectedAudioSampleSize;
    }

    inline static std::vector<std::vector<uint16_t>> getAudioData()
    {
        std::vector<std::vector<uint16_t>> temp;

        pthread_mutex_lock(&audioDataMutex);
        temp.push_back(audioDataL);
        temp.push_back(audioDataR);
        pthread_mutex_unlock(&audioDataMutex);

        return temp;
    }

    static void createCommunicationThreads();
    static void joinCommunicationThreads();

    inline static bool getArduinoConnected() { return arduinoConnected; }

private:
    static std::string findSerialPort(std::string &path);
    static uint8_t     connectSerialConnection();
    static void        closeSerialConnection();
    static void       *communicationThreadHandler(void *args);
    static uint8_t     receiveAudioData();
    static uint8_t     transmitDMXData();

private:
    inline static size_t                expectedAudioSampleSize = 64;
    inline static std::vector<uint16_t> audioDataL              = std::vector<uint16_t>(expectedAudioSampleSize, 522);
    inline static std::vector<uint16_t> audioDataR              = std::vector<uint16_t>(expectedAudioSampleSize, 522);
    inline static pthread_mutex_t       audioDataMutex          = PTHREAD_MUTEX_INITIALIZER;
    inline static std::string           receivedData            = std::string("");
    inline static std::vector<uint8_t>  dmxData                 = std::vector<uint8_t>(512, 0);
    inline static pthread_mutex_t       dmxDataMutex            = PTHREAD_MUTEX_INITIALIZER;
    inline static pthread_t                    communicationThread;
    inline static bool                  threadsShouldJoin = false;
    inline static int                   serialConnection  = 0;
    inline static bool                  arduinoConnected  = false;
    inline static speed_t               baudRate          = B115200;

#if defined(__APPLE__)
    inline static std::string arduinoPathBegin = "/dev/tty.usbmodem";  // "/dev/cu.usbmodem"
#elif defined(__linux__)
    inline static std::string arduinoPathBegin = "/dev/tty.ACM";
#endif
};

#endif  // ARDUINOAUDIOINPUT_HPP
