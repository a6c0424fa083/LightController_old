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
        openSerialConnection();
        while (!threadsShouldJoin)
        {
            uint8_t error = receiveAudioData();

            if (error != 0) fprintf(stderr, "Error receiving audio data!\n");
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
    serialConnection = open(path.c_str(), O_RDWR);
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
        // printf("In receive function! (after connection)\n");
        struct termios tty;
        tcgetattr(serialConnection, &tty);
        cfsetospeed(&tty, baudRate);
        cfsetispeed(&tty, baudRate);
        tcsetattr(serialConnection, TCSANOW, &tty);
        // printf("After file Initialization!\n");

        bytesRead = read(serialConnection, flushBuffer, 2500);
         //printf("Read %zu bytes from buffer!\n", bytesRead);
        if (bytesRead < 0)
        {
            fprintf(stderr, "Could not open arduino serial file! Error No.: %d\n", errno);
            closeSerialConnection();
            return 32;
        }
        if (bytesRead == 0)
        {
            fprintf(stderr, "Got to end of file without start byte!\n");
            //  closeSerialConnection();
            return 128;
        }

        // Reading chars until a start character (with upper timeout limit)
        for (uint16_t i = 0; i < 1000; i++)
        {
            // printf("Before read funcit\n");
            bytesRead = read(serialConnection, byte, 1);
            // printf("After read funcit\n");

            if (byte[0] == startByte)
            {
                 //printf("Detected start byte '0x%x'\n", byte[0]);
                break;
            }

            if (bytesRead == 0)
            {
                fprintf(stderr, "Got to end of file without start byte!\n");
                //  closeSerialConnection();
                return 128;
            }

            if (bytesRead < 0)
            {
                fprintf(stderr, "Could not open arduino serial file! Error No.: %d\n", errno);
                closeSerialConnection();
                return 32;
            }

            usleep(15);
            // printf("Cycle %u in start loop!\n", i);
            if (i == 999)
            {
                 fprintf(stderr, "Arduino connection timeout reached!\n");
                return 16;
            }
        }

        usleep(15);

        if (!receivedData.empty()) receivedData.clear();

        // reading in the audio data (with upper timeout limit)
        for (uint16_t i = 0; i < 1000; i++)
        {
            bytesRead = read(serialConnection, byte, 1);


            if (byte[0] == endByte)
            {
                 //printf("Detected end byte '0x%x'\n", byte[0]);
                break;
            }
            else { receivedData.push_back(static_cast<char>(byte[0])); }

            usleep(15);

            if (bytesRead < 0)
            {
                fprintf(stderr, "Could not open arduino serial file! Error No.: %d\n", errno);
                closeSerialConnection();
                return 32;
            }
            if (bytesRead == 0)
            {
                 fprintf(stderr, "Got to end of file without start byte!\n");
                //  closeSerialConnection();
                return 128;
            }

            // printf("Cycle %u in end loop!\n", i);

            if (i == 999)
            {
                 fprintf(stderr, "Arduino connection timeout reached!\n");
                return 16;
            }
        }

        // printf("Received data length: %zu!\n", receivedData.length());

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

             //printf("Received Data: %s\n", receivedData.data());

             //printf("Received Data: ");
             //for (size_t i = 0; i < receivedData.length(); i++) { printf("%c", receivedData.at(i)); }
             //printf("\n");

            // closeSerialConnection();
            return 0;
        }
        else
        {
            fprintf(stderr,
                    "Invalid sample size! (expected %zu, got %zu instead)\n",
                    4 * expectedAudioSampleSize,
                    receivedData.length());
            // printf("(Data: %s)\n", receivedData.data());
            //  closeSerialConnection();
            return 64;
        }
        // closeSerialConnection();
    }
    return 255;
}

uint8_t ArduinoCommunication::transmitDMXData()
{
    if (!isArduinoConnected) openSerialConnection();

    // printf("In transmitt function! (after connection)\n");
    struct termios tty;
    tcgetattr(serialConnection, &tty);
    cfsetospeed(&tty, baudRate);
    cfsetispeed(&tty, baudRate);
    tcsetattr(serialConnection, TCSANOW, &tty);

    bytesWrote = write(serialConnection, &startByte, 1);

    bytesWrote = write(serialConnection, dmxValuesStr.data(), dmxValuesStr.size());

    bytesWrote = write(serialConnection, &endByte, 1);
    bytesWrote = write(serialConnection, &endByte, 1);
    bytesWrote = write(serialConnection, &endOfLine, 1);

    return 0;
}