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

#include "serial/serial.h"

class ArduinoCommunication
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
        temp.push_back(audioDataL2); // replace with original later
        temp.push_back(audioDataR2); // replace with original later
        pthread_mutex_unlock(&audioDataMutex);

        return temp;
    }
    inline static void setDmxValues(std::vector<uint8_t> &newValues)
    {
        if (newValues.size() == dmxChannelCount)
        {
            dmxData = newValues;
            for (uint16_t i = 0; i < dmxChannelCount; i++)
            {
                dmxValuesStr.at(2 * i)     = static_cast<char>((newValues.at(i) & 0b00001111) + 65);
                dmxValuesStr.at(2 * i + 1) = static_cast<char>(((newValues.at(i) >> 4) & 0b00001111) + 65);
            }
        }
        else { fprintf(stderr, "Cannot assign a dmx vector of incorrect length to original!\n"); }
    }

    inline static uint16_t getDMXChannelCount() { return dmxChannelCount; }

    static void createCommunicationThreads();
    static void joinCommunicationThreads();

private:
    static std::vector<uint16_t> strToAudio2(std::vector<uint8_t> _str);
    static std::string           findSerialPort(std::string &path);
    inline static uint8_t readUntilStartByte()
    {
        // Reading chars until a start character (with upper timeout limit)
        for (uint16_t i = 0; i < 1000; i++)
        {
            bytesRead = read(serialConnection, byte, 1);

            if (byte[0] == startByte) break;

            if (bytesRead == 0)
            {
                fprintf(stderr, "Got to end of file without start byte!\n");
                return 128;
            }

            if (bytesRead < 0)
            {
                fprintf(stderr, "Could not open arduino serial file! Error No.: %d\n", errno);
                //closeSerialConnection();
                return 32;
            }

            usleep(15);

            if (i == 999)
            {
                fprintf(stderr, "Arduino connection timeout reached!\n");
                return 16;
            }
        }
        return 0;
    }
    inline static uint8_t readAudioData2()
    {
        if (!receivedData2.empty()) receivedData2.clear();

        // reading in the audio data (with upper timeout limit)
        for (uint16_t i = 0; i < 1000; i++)
        {
            bytesRead = read(serialConnection, byte, 1);


            if (byte[0] == endByte)
                break;
            else
                receivedData2.push_back(static_cast<char>(byte[0]));

            usleep(15);

            if (bytesRead < 0)
            {
                fprintf(stderr, "Could not open arduino serial file! Error No.: %d\n", errno);
                //closeSerialConnection();
                return 32;
            }
            if (bytesRead == 0)
            {
                fprintf(stderr, "Got to end of file without start byte!\n");

                return 128;
            }

            if (i == 999)
            {
                fprintf(stderr, "Arduino connection timeout reached!\n");
                return 16;
            }
        }
        return 0;
    }
    inline static uint8_t convertAudioData2()
    {
        // convert the raw data to sample points
        if (receivedData2.length() == expectedAudioSampleSize2 * 3)
        {
            for (size_t i = 0; i < expectedAudioSampleSize2; i++)
            {
                std::vector<uint8_t> tempIn2;
                tempIn2.push_back(static_cast<uint8_t>(receivedData2.at(3 * i)));
                tempIn2.push_back(static_cast<uint8_t>(receivedData2.at(3 * i + 1)));
                tempIn2.push_back(static_cast<uint8_t>(receivedData2.at(3 * i + 2)));

                std::vector<uint16_t> tempOut2;
                tempOut2 = strToAudio2(tempIn2);

                pthread_mutex_lock(&audioDataMutex);
                audioDataL2.at(i) = tempOut2.at(0);
                audioDataR2.at(i) = tempOut2.at(1);
                pthread_mutex_unlock(&audioDataMutex);
            }
            //printf("Received Data: ");
            //for (size_t i = 0; i < receivedData.length(); i++) { printf("%c", receivedData.at(i)); }
            //printf("\n");
            return 0;
        }
        else
        {
            fprintf(stderr,
                    "Invalid sample size! (expected %zu, got %zu instead)\n",
                    3 * expectedAudioSampleSize2,
                    receivedData2.length());
            return 64;
        }

    }
    static uint8_t receiveAudioData();
    static uint8_t transmitDMXData();
    static void   *communicationThreadHandler(void *args);

private:
    inline static size_t                expectedAudioSampleSize = 64;
    inline static size_t                expectedAudioSampleSize2 = 64; // MUST BE EVEN
    inline static std::vector<uint16_t> audioDataL              = std::vector<uint16_t>(expectedAudioSampleSize, 522);
    inline static std::vector<uint16_t> audioDataR              = std::vector<uint16_t>(expectedAudioSampleSize, 502);
    inline static pthread_mutex_t       audioDataMutex          = PTHREAD_MUTEX_INITIALIZER;
    inline static std::vector<uint16_t> audioDataL2              = std::vector<uint16_t>(expectedAudioSampleSize, 522);
    inline static std::vector<uint16_t> audioDataR2              = std::vector<uint16_t>(expectedAudioSampleSize, 502);
    inline static std::string           receivedData            = std::string("");
    inline static std::string           receivedData2            = std::string("");
    inline static std::vector<uint8_t>  dmxData                 = std::vector<uint8_t>(512, 0);
    inline static pthread_mutex_t       dmxDataMutex            = PTHREAD_MUTEX_INITIALIZER;
    inline static pthread_t             communicationThread;
    inline static bool                  threadsShouldJoin  = false;
    inline static int                   serialConnection   = 0;
    inline static speed_t               baudRate           = B115200;
    inline static bool                  isArduinoConnected = false;
    inline static serial::Serial *serialConnection2 = nullptr;

    inline static ssize_t       bytesRead  = 0;
    inline static ssize_t       bytesWrote = 0;
    inline static const uint8_t startByte  = 0b11111111;  // 0b01111000;
    inline static const uint8_t endByte    = 0b11110000;  // 0b00110000;
    inline static const char    endOfLine  = '\n';
    // inline static bool              dmxShouldMute = false;
    // inline static const std::string muteMessage   = "MT";
    // inline static const std::string unmuteMessage = "UM";

    inline static uint16_t     dmxChannelCount = 512;
    inline static std::string dmxValuesStr    = std::string(2 * dmxChannelCount, 'A');

    inline static uint8_t byte[1]           = { startByte };
    inline static const uint16_t flushBufferSize = 2500;
    inline static uint8_t flushBuffer[flushBufferSize] = { 0 };

#if defined(__APPLE__)
    inline static std::string arduinoPathBegin = "/dev/cu.usbmodem";  // "/dev/tty.usbmodem"
#elif defined(__linux__)
    inline static std::string arduinoPathBegin = "/dev/tty.ACM";
#endif
};

#endif  // ARDUINOAUDIOINPUT_HPP
