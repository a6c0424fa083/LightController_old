#include "ArduinoCommunication2.hpp"


void ArduinoCommunication::createCommunicationThreads()
{
    pthread_create(&communicationThread, nullptr, &communicationThreadHandler, nullptr);
}

void ArduinoCommunication::joinCommunicationThreads()
{
    threadsShouldJoin = true;
    pthread_join(communicationThread, nullptr);
}

void ArduinoCommunication::writeDMX(uint16_t start_addr, uint8_t *dmxChannels, uint16_t dmxChannelCount)
{
    if (start_addr + dmxChannelCount > 513) return;
    for (uint16_t i = start_addr; i < start_addr + dmxChannelCount; i++)
    {
        _dmxChannels[i] = *(dmxChannels + i - start_addr);
    }
}

void ArduinoCommunication::writeDMX(uint16_t start_addr, std::vector<uint8_t> dmxChannels)
{
    if (start_addr + dmxChannels.size() > 513) return;
    for (uint16_t i = start_addr; i < start_addr + dmxChannels.size(); i++)
    {
        _dmxChannels[i] = dmxChannels.at(i - start_addr);
    }
}

std::vector<uint8_t> ArduinoCommunication::getDMX()
{
    std::vector<uint8_t> output;
    for(uint16_t i = 0; i < _dmxChannelCount; i++)
    {
        output.push_back(_dmxChannels[i]);
    }

    return output;
}
uint16_t ArduinoCommunication::getDMXChannelCount()
{
    return _dmxChannelCount;
}

void ArduinoCommunication::pushDMX()
{

    if (_dmxChannelCount > 512) return;
    _dmxChannelsRoot[0] = dmxStartChar;
    _dmxChannelsRoot[_dmxChannelCount + 2] = '\n';
    ssize_t written_chars = write(serialConnection, _dmxChannelsRoot, _dmxChannelCount + 3);

    FILE *log = fopen("comm.txt", "a");
    fprintf(log, "DMX Data:");
    fflush(log);
    fwrite(_dmxChannelsRoot, _dmxChannelCount + 3, 1, log);
    fprintf(log, "EofP");
    fflush(log);

    if (written_chars < 0)
    {
        fprintf(stderr, "Error while writing to arduino! (%ld)\n", written_chars);
    }
    else
    {
        printf("\r                                       \r");
        printf("Successfully sent dmx values to Arduino");
        fflush(stdout);
    }
/*
    usleep(100000);

    constexpr size_t buffer_len = 500;
    char buffer[buffer_len];
    memset(buffer, 0, buffer_len);

    ssize_t read_chars = read(serialConnection, buffer, buffer_len);
    if (read_chars < 0)
    {
        fprintf(stderr, "Error while reading from arduino! (%ld)\n", read_chars);
    }
    else
    {
        fprintf(log, "Arduino Response:");
        fflush(log);
        fwrite(buffer, 1, static_cast<size_t>(read_chars), log);
        fprintf(log, "EofP");
        fflush(log);
    }
*/
    fclose(log);
}

void ArduinoCommunication::readAudioData()
{

}

// PRIVATE
void *ArduinoCommunication::communicationThreadHandler(void *args)
{
    if (args == nullptr)
    {
        // uint8_t error;
        openSerialConnection();
        while (!threadsShouldJoin)
        {
            // audio read / dmx write loop
            pushDMX();
            usleep(50000);
        }
        closeSerialConnection();
    }

    return nullptr;
}

uint8_t ArduinoCommunication::openSerialConnection()
{
    if (ArduinoConnected)
        return 0;
    std::string path = findSerialPort(arduinoPathBegin);

    // Open the serial port
    serialConnection = open(path.c_str(), O_RDWR | O_NOCTTY | O_NDELAY );
    setupUARTParameters(&serialConnection);

    if (serialConnection < 0)
    {
        fprintf(stderr, "Error opening serial port. Error No.: %d\n", errno);
        return 255;
    }
    ArduinoConnected = true;
    return 0;
}

void ArduinoCommunication::closeSerialConnection()
{
    if (ArduinoConnected) close(serialConnection);
    ArduinoConnected = false;
}

std::string ArduinoCommunication::findSerialPort(std::string &path)
{
    std::string output = {};
    std::string test;

    // Iterate through the files in /dev/cu.usbmodem* to find the Arduino port
    for (const auto &file : std::filesystem::directory_iterator("/dev"))
    {
        test = file.path().string();
        if (test.find(path) == 0) { output = test; }
    }

    if (output.empty())
        fprintf(stderr, "Could not find Arduino in file system!\n");

    return output;
}


uint8_t ArduinoCommunication::setupUARTParameters(const int *fd)
{
    termios tty{};

    // Read in existing settings, and handle any error
    if (tcgetattr(*fd, &tty) != 0)
    {
        std::cerr << "Error from tcgetattr: " << strerror(errno) << std::endl;
        return 1;
    }

    // Set Baud Rate
    cfsetospeed(&tty, baudRate);
    cfsetispeed(&tty, baudRate);

    // Setting other Port Stuff
    tty.c_cflag &= static_cast<tcflag_t>(~PARENB); // Make 8n1
    tty.c_cflag &= static_cast<tcflag_t>(~CSTOPB);
    tty.c_cflag &= static_cast<tcflag_t>(~CSIZE);
    tty.c_cflag |= CS8;

    tty.c_cflag &= static_cast<tcflag_t>(~CRTSCTS); // No flow control
    tty.c_cflag |= CREAD | CLOCAL;                  // Turn on READ & ignore ctrl lines (CLOCAL = 1)

    tty.c_lflag &= static_cast<tcflag_t>(~ICANON);
    tty.c_lflag &= static_cast<tcflag_t>(~ECHO);  // Disable echo
    tty.c_lflag &= static_cast<tcflag_t>(~ECHOE); // Disable erasure
    tty.c_lflag &= static_cast<tcflag_t>(~ECHONL);
    tty.c_lflag &= static_cast<tcflag_t>(~ISIG); // Disable interpretation of INTR, QUIT and SUSP

    tty.c_iflag &= static_cast<tcflag_t>(~(IXON | IXOFF | IXANY)); // Turn off s/w flow ctrl
    tty.c_iflag &= static_cast<tcflag_t>(~(IGNBRK | BRKINT | ICRNL | INLCR | PARMRK | INPCK | ISTRIP | IXON));

    tty.c_oflag &= static_cast<tcflag_t>(~OPOST); // Prevent special interpretation of output bytes (e.g. newline chars)
    tty.c_oflag &= static_cast<tcflag_t>(~ONLCR); // Prevent conversion of newline to carriage return/line feed

    // Setting Timeouts
    tty.c_cc[VTIME] = 10; // Wait for up to 1s (10 deciseconds), returning as soon as any data is received.
    tty.c_cc[VMIN]  = 0;

    tcflush(*fd, TCIOFLUSH);

    // Save tty settings, also checking for error
    if (tcsetattr(*fd, TCSANOW, &tty) != 0)
    {
        std::cerr << "Error from tcsetattr: " << strerror(errno) << std::endl;
        return 1;
    }
    return 0;
}
