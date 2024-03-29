#ifndef SERIAL_HPP
#define SERIAL_HPP

#include <cstdlib>
#include <fcntl.h>
#include <filesystem>
#include <iostream>
#include <pthread.h>
#include <string>
#include <sys/filio.h>
#include <termios.h>
#include <unistd.h>
#include <vector>
#include <sys/_types/_timespec.h>
#include <mach/mach_init.h>
#include <mach/clock_types.h>
#include <mach/mach_host.h>
#include <mach/clock.h>
#include <mach/mach_port.h>
#include <sstream>

/*!
 * Structure for setting the timeout of the serial port, times are
 * in milliseconds.
 *
 * In order to disable the interbyte timeout, set it to Timeout::max().
 */
struct Timeout
{
#ifdef max
    #undef max
#endif
    static uint32_t max() { return std::numeric_limits<uint32_t>::max(); }
    /*!
     * Convenience function to generate Timeout structs using a
     * single absolute timeout.
     *
     * \param timeout A long that defines the time in milliseconds until a
     * timeout occurs after a call to read or write is made.
     *
     * \return Timeout struct that represents this simple timeout provided.
     */
    static Timeout simpleTimeout(uint32_t timeout) { return Timeout(max(), timeout, 0, timeout, 0); }

    /*! Number of milliseconds between bytes received to timeout on. */
    uint32_t inter_byte_timeout;
    /*! A constant number of milliseconds to wait after calling read. */
    uint32_t read_timeout_constant;
    /*! A multiplier against the number of requested bytes to wait after
     *  calling read.
     */
    uint32_t read_timeout_multiplier;
    /*! A constant number of milliseconds to wait after calling write. */
    uint32_t write_timeout_constant;
    /*! A multiplier against the number of requested bytes to wait after
     *  calling write.
     */
    uint32_t write_timeout_multiplier;

    explicit Timeout(uint32_t inter_byte_timeout_ = 0, uint32_t read_timeout_constant_ = 0, uint32_t read_timeout_multiplier_ = 0,
                     uint32_t write_timeout_constant_ = 0, uint32_t write_timeout_multiplier_ = 0)
        : inter_byte_timeout(inter_byte_timeout_),
          read_timeout_constant(read_timeout_constant_),
          read_timeout_multiplier(read_timeout_multiplier_),
          write_timeout_constant(write_timeout_constant_),
          write_timeout_multiplier(write_timeout_multiplier_)
    {
    }
};

class MillisecondTimer
{
public:
    MillisecondTimer() = delete;
    MillisecondTimer(const uint32_t millis);
    int64_t remaining();

private:
    static timespec timespec_now();
    timespec        expiry;
};

class Serial
{
public:
    Serial() = delete;
    inline Serial(std::string &portPath, speed_t baudRate) : _portPath(portPath), _baudRate(baudRate) {}

    uint8_t openConnection(int openAttributes);
    uint8_t closeConnection() const;

    ssize_t write(std::string &dataIn);
    ssize_t read(std::string &buffer, size_t size);

private:
    uint8_t setTTYOptions();

    bool waitReadable(uint32_t timeout);

    size_t available();

    timespec timespec_from_ms(const uint32_t millis);

    void waitByteTimes(size_t count);

    ssize_t _read(uint8_t *buffer, ssize_t size = 1);
    ssize_t _write (const uint8_t *data, size_t length);

private:
    int         _serial           = -1;
    std::string _portPath         = "/dev/cu.usbmodemXXXXX";
    speed_t     _baudRate         = B115200;
    bool        _arduinoConnected = false;
    Timeout     _timeout;
    uint32_t    byte_time_ns_;
};

#endif  // SERIAL_HPP
