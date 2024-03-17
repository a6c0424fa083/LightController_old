#include "ArduinoCommunication.hpp"

void ArduinoCommunication::createCommunicationThreads()
{
    pthread_create(&communicationThread, nullptr, &communicationThreadHandler, nullptr);
}
void ArduinoCommunication::joinCommunicationThreads()
{
    threadsShouldJoin = true;
    while (pthread_join(communicationThread, nullptr) != 0) {}
    // closeSerialConnection();
}

std::vector<uint8_t> ArduinoCommunication::audioToStr(std::vector<uint16_t> _audio)
{
    std::vector<uint8_t> _str(4, 0);

    // big endian
    _str[0] = static_cast<uint8_t>(_audio[0] & 0b0000000000011111) + 65;
    _str[1] = static_cast<uint8_t>((_audio[0] >> 5) & 0b0000000000011111) + 65;
    _str[2] = static_cast<uint8_t>(_audio[1] & 0b0000000000011111) + 65;
    _str[3] = static_cast<uint8_t>((_audio[1] >> 5) & 0b0000000000011111) + 65;

    return _str;
}
std::vector<uint16_t> ArduinoCommunication::strToAudio(std::vector<uint8_t> _str)
{
    std::vector<uint16_t> _audio(2, 0);

    // big endian
    _audio[0] = static_cast<uint16_t>(static_cast<uint16_t>(_str[0] - 65) & 0b0000000000011111) |
                static_cast<uint16_t>(static_cast<uint16_t>(static_cast<uint16_t>(_str[1] - 65) << 5) & 0b0000001111100000);
    _audio[1] = static_cast<uint16_t>(static_cast<uint16_t>(_str[2] - 65) & 0b0000000000011111) |
                static_cast<uint16_t>(static_cast<uint16_t>(static_cast<uint16_t>(_str[3] - 65) << 5) & 0b0000001111100000);

    return _audio;
}

void *ArduinoCommunication::communicationThreadHandler(void *args)
{
    if (args == nullptr)
    {
        uint8_t error;
        openSerialConnection();
        while (!threadsShouldJoin)
        {
            error = receiveAudioData();
            if (error != 0) fprintf(stderr, "Error receiving audio data! (Internal error: %d)\n", error);

            error = transmitDMXData();
            if (error != 0) fprintf(stderr, "Error receiving audio data! (Internal error: %d)\n", error);
        }
        closeSerialConnection();
    }

    return nullptr;
}

std::string ArduinoCommunication::findSerialPort(std::string &path)
{
    std::string output;
    std::string test;
    // Iterate through the files in /dev/cu.usbmodem* to find the Arduino port
    for (const auto &file : std::filesystem::directory_iterator("/dev"))
    {
        test = file.path().string();
        if (test.find(path) == 0) { output = test; }
    }

    if (!output.empty()){}
        //printf("Found Arduino at: %s!\n", output.data());
    else fprintf(stderr, "Could not find Arduino in file system!\n");

    return output;
}
uint8_t ArduinoCommunication::openSerialConnection()
{
    std::string path = findSerialPort(arduinoPathBegin);

    // Open the serial port
    serialConnection = open(path.c_str(), O_RDWR/* | O_NOCTTY | O_NDELAY*/);

    setupUARTParameters();


    if (serialConnection < 0)
    {
        fprintf(stderr, "Error opening serial port. Error No.: %d\n", errno);
        return 255;
    }
    isArduinoConnected = true;
    return 0;
}
void ArduinoCommunication::closeSerialConnection()
{
    if (isArduinoConnected) close(serialConnection);
    isArduinoConnected = false;
     //printf("Closed connection!\n");
}

uint8_t ArduinoCommunication::receiveAudioData()
{
    if (!isArduinoConnected) openSerialConnection();
    if (isArduinoConnected)
    {
        uint8_t ret;

        ret = readBuffer();
        if (ret != 0) return ret;

        ret = readUntilStartByte();
        if (ret != 0) return ret;

        // wait for at least 1 more character is transmitted
        usleep(15);

        ret = readAudioData();
        if (ret != 0) return ret;

        return convertAudioData();
    }
    return 255;
}

uint8_t ArduinoCommunication::transmitDMXData()
{
    if (!isArduinoConnected) openSerialConnection();

    bytesWrote = write(serialConnection, &startByte, 1);

    bytesWrote = write(serialConnection, dmxValuesStr.data(), dmxValuesStr.size());

    bytesWrote = write(serialConnection, &endByte, 1);
    bytesWrote = write(serialConnection, &endByte, 1);
    bytesWrote = write(serialConnection, &endOfLine, 1);

    return 0;
}