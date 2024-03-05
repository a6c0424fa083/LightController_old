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

void *ArduinoAudioInput::communicationThreadHandler(void *args)
{
    if (args == nullptr)
    {
        if (connectSerialConnection() > 0)
        {
            while (!threadsShouldJoin)
            {
                if (!arduinoConnected) connectSerialConnection();

                if (arduinoConnected)
                {
                    if (receiveAudioData() != 0) fprintf(stderr, "Error receiving audio data!\n");
                }
            }
        }
        else { fprintf(stderr, "Connection to Arduino failed!\n"); }
    }
    return nullptr;
}

std::string ArduinoAudioInput::findSerialPort(std::string &path)
{
    std::string output;
    std::string test;
    // Iterate through the files in /dev/tty.usbmodem* to find the Arduino port
    for (const auto &file : std::filesystem::directory_iterator("/dev"))
    {
        test = file.path().string();
        if (test.find(path) == 0) { output = test; }
    }

    return output;
}

uint8_t ArduinoAudioInput::connectSerialConnection()
{
    std::string path = findSerialPort(arduinoPathBegin);

    // Open the serial port
    serialConnection = open(path.c_str(), O_RDWR);
    if (serialConnection == -1)
    {
        fprintf(stderr, "Error opening serial port.\n");
        return 255;
    }

    arduinoConnected = true;
    return 0;
}

void ArduinoAudioInput::closeSerialConnection() { close(serialConnection); }

uint8_t ArduinoAudioInput::receiveAudioData()
{
    if (arduinoConnected)
    {
        struct termios tty;
        tcgetattr(serialConnection, &tty);
        cfsetospeed(&tty, baudRate);
        cfsetispeed(&tty, baudRate);
        tcsetattr(serialConnection, TCSANOW, &tty);

        uint8_t byte[1] = { 0xFF };

        ssize_t bytesRead;
        uint8_t startByte = 0b11110000;
        uint8_t endByte   = 0b11000000;

        // Reading chars until a start character
        do {
            bytesRead = read(serialConnection, byte, 1);

            if (byte[0] == startByte)
            {
                printf("Found start byte: '0x%x'\n", byte[0]);
                break;
            }
            if (bytesRead == 0)
            {
                printf("No start bit found till end of serial string!\n");
                return 128;
            }
            else if (bytesRead < 0)
            {
                fprintf(stderr, "Could not open arduino serial file!\n");
                arduinoConnected = false;
            }
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
                pthread_mutex_lock(&audioDataMutex);
                audioDataL.at(i) = static_cast<uint16_t>(receivedData.at(4 * i) - 65) |
                                   static_cast<uint16_t>((receivedData.at(4 * i + 1) - 65) << 5);  // big endian
                audioDataR.at(i) = static_cast<uint16_t>(receivedData.at(4 * i + 2) - 65) |
                                   static_cast<uint16_t>((receivedData.at(4 * i + 3) - 65) << 5);  // big endian
                pthread_mutex_unlock(&audioDataMutex);
            }
        }
        else
        {
            fprintf(stderr,
                    "Invalid sample size! (expected %zu, got %zu instead)\n",
                    2 * expectedAudioSampleSize,
                    receivedData.length());
        }
    }

    return 255;
}

uint8_t ArduinoAudioInput::transmitDMXData() { return 0; }