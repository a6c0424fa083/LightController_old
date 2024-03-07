#include "ArduinoAudioInput.hpp"

void ArduinoAudioInput::createCommunicationThreads()
{
    pthread_create(&communicationThread, nullptr, &communicationThreadHandler, nullptr);
}
void ArduinoAudioInput::joinCommunicationThreads()
{
    threadsShouldJoin = true;
    while (pthread_join(communicationThread, nullptr) != 0) {}
}

std::vector<uint8_t> ArduinoAudioInput::audioToStr(std::vector<uint16_t> _audio)
{
    std::vector<uint8_t> _str(4, 0);

    // big endian
    _str[0] = static_cast<uint8_t>(_audio[0] & 0b0000000000011111) + 65;
    _str[1] = static_cast<uint8_t>((_audio[0] >> 5) & 0b0000000000011111) + 65;
    _str[2] = static_cast<uint8_t>(_audio[1] & 0b0000000000011111) + 65;
    _str[3] = static_cast<uint8_t>((_audio[1] >> 5) & 0b0000000000011111) + 65;

    return _str;
}
std::vector<uint16_t> ArduinoAudioInput::strToAudio(std::vector<uint8_t> _str)
{
    std::vector<uint16_t> _audio(2, 0);

    // big endian
    _audio[0] = static_cast<uint16_t>(static_cast<uint16_t>(_str[0] - 65) & 0b0000000000011111) |
                static_cast<uint16_t>(static_cast<uint16_t>(static_cast<uint16_t>(_str[1] - 65) << 5) & 0b0000001111100000);
    _audio[1] = static_cast<uint16_t>(static_cast<uint16_t>(_str[2] - 65) & 0b0000000000011111) |
                static_cast<uint16_t>(static_cast<uint16_t>(static_cast<uint16_t>(_str[3] - 65) << 5) & 0b0000001111100000);

    return _audio;
}

void *ArduinoAudioInput::communicationThreadHandler(void *args)
{
    if (args == nullptr)
    {
        while (!threadsShouldJoin)
        {
            uint8_t error = receiveAudioData();

            if (error != 0) fprintf(stderr, "Error receiving audio data! Error No.: %d\n", error);
        }
    }

    return nullptr;
}

std::string ArduinoAudioInput::findSerialPort(std::string &path)
{
    std::string output;
    std::string test;
    // Iterate through the files in /dev/cu.usbmodem* to find the Arduino port
    for (const auto &file : std::filesystem::directory_iterator("/dev"))
    {
        test = file.path().string();
        if (test.find(path) == 0) { output = test; }
    }

    if (!output.empty())
        printf("Found Arduino at: %s!\n", output.data());
    else
        fprintf(stderr, "Could not find Arduino in file system!\n");

    return output;
}
uint8_t ArduinoAudioInput::openSerialConnection()
{
    std::string path = findSerialPort(arduinoPathBegin);

    // Open the serial port
    serialConnection = open(path.c_str(), O_RDWR);
    if (serialConnection < 0)
    {
        fprintf(stderr, "Error opening serial port. Error No.: %d\n", errno);
        return 255;
    }
    return 0;
}
void ArduinoAudioInput::closeSerialConnection()
{
    close(serialConnection);
    printf("Closed connection!\n");
}

uint8_t ArduinoAudioInput::receiveAudioData()
{
    if (openSerialConnection() == 0)
    {
        struct termios tty;
        tcgetattr(serialConnection, &tty);
        cfsetospeed(&tty, baudRate);
        cfsetispeed(&tty, baudRate);
        tcsetattr(serialConnection, TCSANOW, &tty);

        ssize_t bytesRead;
        uint8_t startByte = 0b01111000;
        uint8_t endByte   = 0b00110000;

        uint8_t byte[1] = { startByte };

        // Reading chars until a start character
        do {
            bytesRead = read(serialConnection, byte, 1);

            if (byte[0] == startByte) break;

            if (bytesRead == 0)
            {
                return 128;
                closeSerialConnection();
            }
            else if (bytesRead < 0)
                fprintf(stderr, "Could not open arduino serial file!\n");

            usleep(50);

        } while (true);

        usleep(250);

        if (receivedData.empty()) receivedData.clear();

        // reading in the audio data
        do {
            bytesRead = read(serialConnection, byte, 1);


            if (byte[0] == endByte)
            {
                printf("Detected end byte '0x%x'\n", byte[0]);
                break;
            }
            else { receivedData.push_back(static_cast<char>(byte[0])); }

            usleep(50);

        } while (bytesRead > 0);

        // convert the raw data to sample points
        if (receivedData.length() == 4 * expectedAudioSampleSize)  // checks if length is even too...
        {
            for (size_t i = 0; i < expectedAudioSampleSize; i++)
            {
                std::vector<uint8_t> tempIn;
                tempIn.push_back(static_cast<uint8_t>(receivedData.at(4 * i)));
                tempIn.push_back(static_cast<uint8_t>(receivedData.at(4 * i + 1)));
                tempIn.push_back(static_cast<uint8_t>(receivedData.at(4 * i + 2)));
                tempIn.push_back(static_cast<uint8_t>(receivedData.at(4 * i + 3)));

                std::vector<uint16_t> tempOut;
                tempOut = strToAudio(tempIn);

                pthread_mutex_lock(&audioDataMutex);
                audioDataL.at(i) = tempOut.at(0);
                audioDataR.at(i) = tempOut.at(1);
                pthread_mutex_unlock(&audioDataMutex);
            }
            closeSerialConnection();
            return 0;
        }
        else
        {
            fprintf(stderr,
                    "Invalid sample size! (expected %zu, got %zu instead)\n",
                    2 * expectedAudioSampleSize,
                    receivedData.length());
            closeSerialConnection();
            return 64;
        }
        closeSerialConnection();
    }
    return 255;
}

uint8_t ArduinoAudioInput::transmitDMXData() { return 0; }