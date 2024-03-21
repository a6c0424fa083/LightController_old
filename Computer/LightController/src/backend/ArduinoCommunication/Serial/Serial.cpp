#include "Serial.hpp"

uint8_t Serial::openConnection(int openAttributes = O_RDWR | O_NOCTTY | O_NONBLOCK)
{
        _serial = open(_portPath.c_str(), openAttributes);

        if (_serial < 0)
        {
            fprintf(stderr, "Error opening file! errno: %u\n", errno);
            return 1;
        }

        return 0;
}


uint8_t Serial::closeConnection()
{

}