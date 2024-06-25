#ifndef ARDUINOCOMMUNICATION2_H
#define ARDUINOCOMMUNICATION2_H

#include <cstdlib>
#include <fcntl.h>
#include <filesystem>
#include <iostream>
#include <pthread.h>
#include <string>
#include <termios.h>
#include <unistd.h>
#include <vector>

class ArduinoCommunication
{
public:
    static void createCommunicationThreads();

    static void joinCommunicationThreads();
    static void writeDMX(uint16_t start_addr, uint8_t *dmxChannels, uint16_t dmxChannelCount);
    static void writeDMX(uint16_t start_addr, std::vector<uint8_t> dmxChannels);
    static std::vector<uint8_t> getDMX();
    static uint16_t getDMXChannelCount();
    static void pushDMX();

    static void readAudioData();

private:
    static void *communicationThreadHandler(void *args);

    static uint8_t openSerialConnection();

    static void closeSerialConnection();

    static std::string findSerialPort(std::string &path);

    static uint8_t setupUARTParameters(const int *fd);


private:
    inline static size_t               expectedAudioSampleSize = 64;
    inline static std::vector<uint16_t> audioDataL              = std::vector<uint16_t>(expectedAudioSampleSize, 522);
    inline static std::vector<uint16_t> audioDataR              = std::vector<uint16_t>(expectedAudioSampleSize, 502);
    //inline static pthread_mutex_t      audioDataMutex          = PTHREAD_MUTEX_INITIALIZER;
    inline static std::string          receivedData = std::string("");
    //inline static std::vector<uint8_t> dmxData      = std::vector<uint8_t>(512, 0);
    //inline static pthread_mutex_t       dmxDataMutex             = PTHREAD_MUTEX_INITIALIZER;
    inline static pthread_t communicationThread;
    inline static bool      threadsShouldJoin = false;
    inline static int       serialConnection  = 0;
    inline static speed_t   baudRate          = B115200;
    inline static bool      ArduinoConnected  = false;


    inline static uint8_t dmxStartChar = 'd';
    inline static uint8_t cmdStartChar = 'C';

    inline static uint16_t    _dmxChannelCount = 8;
    inline static uint8_t _dmxChannelsRoot[515] = { 0 }; // start char, init, ch1, ch2, ch3, ...
    inline static uint8_t *_dmxChannels = (_dmxChannelsRoot + 1); // init, ch1, ch2, ch3, ...
    //inline static std::string dmxValues    = std::string(2 * dmxChannelCount, 0);

    #if defined(__APPLE__)
    inline static std::string arduinoPathBegin = "/dev/cu.usbmodem"; // "/dev/tty.usbmodem"
    #elif defined(__linux__)
    inline static std::string arduinoPathBegin = "/dev/tty.ACM";
    #endif
};


#endif //ARDUINOCOMMUNICATION2_H