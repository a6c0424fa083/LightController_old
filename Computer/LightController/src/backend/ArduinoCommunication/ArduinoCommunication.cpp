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
std::vector<uint16_t> ArduinoCommunication::strToAudio2(std::vector<uint8_t> _str)
{
    std::vector<uint16_t> _audio;

    uint64_t temp = 0;

    temp |= static_cast<uint64_t>(_str.at(0) & 0b01111111);
    temp |= (static_cast<uint64_t>(_str.at(1) & 0b01111111) << 7);
    temp |= (static_cast<uint64_t>(_str.at(2) & 0b01111111) << 14);

    _audio.push_back(temp & 0b00000000000000000000001111111111);          // L
    _audio.push_back((temp >> 10) & 0b00000000000000000000001111111111);  // R

    return _audio;
}

void *ArduinoCommunication::communicationThreadHandler(void *args)
{
    if (args == nullptr)
    {
        std::string path;
        while (!threadsShouldJoin)
        {
            if (!isArduinoConnected)
            {
                path = findSerialPort(arduinoPathBegin);

                if (!path.empty())
                {
                    auto *serialConnectionTemp =
                        new serial::Serial(path, static_cast<uint32_t>(baudRate), serial::Timeout::simpleTimeout(2000));

                    serialConnection2 = serialConnectionTemp;

                    if (serialConnection2->isOpen()) { isArduinoConnected = true; }
                }
            }
            receiveAudioData();
            transmitDMXData();
        }
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

    if (output.empty()) fprintf(stderr, "Could not find Arduino in file system!\n");

    return output;
}


uint8_t ArduinoCommunication::receiveAudioData() { return 0; }

uint8_t ArduinoCommunication::transmitDMXData()
{
    serialConnection2->flushOutput();

    std::string ON  = std::string("1");
    std::string OFF = std::string("0");

    if (dmxData.at(1) >= 128) serialConnection2->write(ON);
    if (dmxData.at(1) <= 127) serialConnection2->write(OFF);

    return 0;
}