#ifndef SERIAL_HPP
#define SERIAL_HPP

#include <cstdlib>
#include <fcntl.h>
#include <filesystem>
#include <iostream>
#include <pthread.h>
#include <string>
#include <termios.h>
#include <unistd.h>
#include <vector>

class Serial
{
public:
    Serial() = delete;
    inline Serial(std::string &portPath, speed_t baudRate) : _portPath(portPath), _baudRate(baudRate) {}

    uint8_t openConnection(int openAttributes);
    uint8_t closeConnection();

private:


private:
    int _serial = -1;
    std::string _portPath = "/dev/cu.usbmodemXXXXX";
    speed_t     _baudRate = B115200;
    bool arduinoConnected = false;
};

#endif  // SERIAL_HPP
