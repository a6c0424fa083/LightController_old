#include "Serial.hpp"
#include <sys/ioctl.h>

MillisecondTimer::MillisecondTimer(const uint32_t millis) : expiry(timespec_now())
{
    auto tv_nsec = static_cast<int64_t>(static_cast<double>(expiry.tv_nsec) + (millis * 1e6));
    if (static_cast<double>(tv_nsec) >= 1e9)
    {
        int64_t sec_diff = tv_nsec / static_cast<int>(1e9);
        expiry.tv_nsec   = tv_nsec % static_cast<int>(1e9);
        expiry.tv_sec += sec_diff;
    }
    else { expiry.tv_nsec = tv_nsec; }
}

int64_t MillisecondTimer::remaining()
{
    timespec now(timespec_now());
    auto  millis = static_cast<int64_t>((static_cast<double>(expiry.tv_sec - now.tv_sec)) * 1e3);
    millis += static_cast<int64_t>((static_cast<double>(expiry.tv_nsec - now.tv_nsec)) / 1e6);
    return millis;
}

timespec MillisecondTimer::timespec_now()
{
    timespec time;
#ifdef __MACH__  // OS X does not have clock_gettime, use clock_get_time
    mach_port_t   cclock;
    mach_timespec mts{};
    host_get_clock_service(mach_host_self(), SYSTEM_CLOCK, &cclock);
    clock_get_time(cclock, &mts);
    mach_port_deallocate(mach_task_self(), cclock);
    time.tv_sec  = mts.tv_sec;
    time.tv_nsec = mts.tv_nsec;
#else
    clock_gettime(CLOCK_MONOTONIC, &time);
#endif
    return time;
}



uint8_t Serial::openConnection(int openAttributes = O_RDWR | O_NOCTTY | O_NONBLOCK)
{
    _serial = open(_portPath.c_str(), openAttributes);

    if (_serial < 0)
    {
        fprintf(stderr, "Error opening file! errno: %u\n", errno);
        return 255;
    }

    _arduinoConnected = true;
    return 0;
}


uint8_t Serial::closeConnection() const
{
    if (_arduinoConnected)
    {
        int temp = close(_serial);
        if (temp != 0)
        {
            fprintf(stderr, "Error closing file! errno: %u\n", errno);
            return 255;
        }
        return 0;
    }
    return 254;
}

ssize_t Serial::write(std::string &dataIn) { return _write (reinterpret_cast<const uint8_t*>(dataIn.c_str()),
                                                                 dataIn.length()); }


ssize_t Serial::read(std::string &buffer, size_t size)
{
    // mutex lock if read onn multiple threads

    uint8_t *buffer_    = new uint8_t[size];
    ssize_t  bytes_read = 0;

    bytes_read = _read(buffer_, static_cast<ssize_t>(size));

    if (bytes_read < 0)
    {
        fprintf(stderr, "Error while reading...!\n");
        return 255;
    }

    if (bytes_read == 0)
    {
        fprintf(stderr, "Got to end of file!\n");
        return 254;
    }

    buffer.append(reinterpret_cast<const char *>(buffer_), static_cast<std::basic_string<char>::size_type>(bytes_read));
    delete[] buffer_;
    return bytes_read;
}


uint8_t Serial::setTTYOptions()
{
    if (!_arduinoConnected)
    {
        fprintf(stderr, "Bad serial value! (Serial::reconfigurePort)\n");
        return 255;
    }

    struct termios options;  // The options for the file descriptor

    if (tcgetattr(_serial, &options) == -1)
    {
        fprintf(stderr, "Bad Error! (Serial::reconfigurePor)\n");
        return 254;
    }

    // set up raw mode / no echo / binary
    options.c_cflag |= static_cast<tcflag_t>((CLOCAL | CREAD));
    options.c_lflag &= static_cast<tcflag_t>(~(ICANON | ECHO | ECHOE | ECHOK | ECHONL | ISIG | IEXTEN));  //|ECHOPRT

    options.c_oflag &= static_cast<tcflag_t>(~(OPOST));
    options.c_iflag &= static_cast<tcflag_t>(~(INLCR | IGNCR | ICRNL | IGNBRK));
#ifdef IUCLC
    options.c_iflag &= static_cast<tcflag_t>(~IUCLC);
#endif
#ifdef PARMRK
    options.c_iflag &= static_cast<tcflag_t>(~PARMRK);
#endif

    // set baud rate
    cfsetispeed(&options, _baudRate);
    cfsetospeed(&options, _baudRate);


    // setup char len
    options.c_cflag &= static_cast<tcflag_t>(~CSIZE);
    options.c_cflag |= static_cast<tcflag_t>(CS8);

    // setup stopbits
    options.c_cflag &= static_cast<tcflag_t>(~CSTOPB);  // 1 stop bit

    // setup parity
    options.c_iflag &= static_cast<tcflag_t>(~(INPCK | ISTRIP));
    options.c_cflag &= static_cast<tcflag_t>(~(PARENB | PARODD));  // no parity


    // setup flow control
#ifdef IXANY
    options.c_iflag &= static_cast<tcflag_t>(~(IXON | IXOFF | IXANY));
#else
    options.c_iflag &= static_cast<tcflag_t>(~(IXON | IXOFF));
#endif
#ifdef CRTSCTS
    options.c_cflag &= static_cast<tcflag_t>(~(CRTSCTS));
#elif defined CNEW_RTSCTS

    options.c_cflag &= static_cast<tcflag_t>(~(CNEW_RTSCTS));
#else
    fprintf(stderr, "OS Support seems wrong.\n");
    return 253;
#endif

    // http://www.unixwiz.net/techtips/termios-vmin-vtime.html
    // this basically sets the read call up to be a polling read,
    // but we are using select to ensure there is data available
    // to read before each call, so we should never needlessly poll
    options.c_cc[VMIN]  = 0;
    options.c_cc[VTIME] = 0;

    // activate settings
    tcsetattr(_serial, TCSANOW, &options);
    return 0;
}


bool Serial::waitReadable(uint32_t timeout)
{
    // Setup a select call to block for serial data or a timeout
    fd_set readfds;
    FD_ZERO(&readfds);
    FD_SET(_serial, &readfds);
    timespec timeout_ts(timespec_from_ms(timeout));
    int      r = pselect(_serial + 1, &readfds, NULL, NULL, &timeout_ts, NULL);

    if (r < 0)
    {
        // Select was interrupted
        if (errno == EINTR) { return false; }
        fprintf(stderr, "Error with pselect()! errno: %u\n", errno);
        return false;
    }
    // Timeout occurred
    if (r == 0) { return false; }
    // This shouldn't happen, if r > 0 our fd has to be in the list!
    if (!FD_ISSET(_serial, &readfds))
    {
        fprintf(stderr, "select reports ready to read, but our fd isn't! (THIS SHOULD NOT HAPPEN)");
    }
    // Data available to read.
    return true;
}


size_t Serial::available()
{
    if (!_arduinoConnected) { return 0; }
    int count = 0;
    if (-1 == ioctl(_serial, FIONREAD, &count)) { fprintf(stderr, "Error with available() function! errno: %u\n", errno); }
    else { return static_cast<size_t>(count); }
    return static_cast<size_t>(-1);
}


timespec timespec_from_ms(const uint32_t millis)
{
    timespec time;
    time.tv_sec  = static_cast<__darwin_time_t>(millis / 1e3);
    time.tv_nsec = static_cast<long>((millis - (static_cast<double>(time.tv_sec) * 1e3)) * 1e6);
    return time;
}


void Serial::waitByteTimes(size_t count)
{
    timespec wait_time = { 0, static_cast<long>(byte_time_ns_ * count) };
    pselect(0, NULL, NULL, NULL, &wait_time, NULL);
}


ssize_t Serial::_read(uint8_t *buffer, ssize_t size)
{
    // If the port is not open, throw
    if (!_arduinoConnected) { return -1; }
    ssize_t bytes_read = 0;

    // Calculate total timeout in milliseconds t_c + (t_m * N)
    uint32_t total_timeout_ms = _timeout.read_timeout_constant;
    total_timeout_ms += _timeout.read_timeout_multiplier * static_cast<long>(size);
    MillisecondTimer total_timeout(total_timeout_ms);

    // Pre-fill buffer with available bytes
    ssize_t bytes_read_now = ::read(_serial, buffer, static_cast<size_t>(size));
    if (bytes_read_now == 0)
    {
        fprintf(stderr, "Got to end of file!\n");
        return -2;
    }
    if (bytes_read_now > 0) { bytes_read = bytes_read_now; }


    while (bytes_read < size)
    {
        int64_t timeout_remaining_ms = total_timeout.remaining();
        if (timeout_remaining_ms <= 0)
        {
            // Timed out
            break;
        }
        // Timeout for the next select is whichever is less of the remaining
        // total read timeout and the inter-byte timeout.
        uint32_t timeout = std::min(static_cast<uint32_t>(timeout_remaining_ms), _timeout.inter_byte_timeout);
        // Wait for the device to be readable, and then attempt to read.
        if (waitReadable(timeout))
        {
            // If it's a fixed-length multi-byte read, insert a wait here so that
            // we can attempt to grab the whole thing in a single IO call. Skip
            // this wait if a non-max inter_byte_timeout is specified.
            if (size > 1 && _timeout.inter_byte_timeout == Timeout::max())
            {
                ssize_t bytes_available = static_cast<ssize_t>(available());
                if (bytes_available + bytes_read < size)
                {
                    waitByteTimes(static_cast<size_t>(size - (bytes_available + bytes_read)));
                }
            }
            // This should be non-blocking returning only what is available now
            //  Then returning so that select can block again.
            bytes_read_now = ::read(_serial, buffer + bytes_read, static_cast<size_t>(size - bytes_read));
            // read should always return some data as select reported it was
            // ready to read when we get to this point.
            if (bytes_read_now < 1)
            {
                // Disconnected devices, at least on Linux, show the
                // behavior that they are always ready to read immediately
                // but reading returns nothing.
                fprintf(stderr,
                        "device reports readiness to read but returned no data (device disconnected?)! errno: %u\n",
                        errno);
            }
            // Update bytes_read
            bytes_read += static_cast<size_t>(bytes_read_now);
            // If bytes_read == size then we have read everything we need
            if (bytes_read == size) { break; }
            // If bytes_read < size then we have more to read
            if (bytes_read < size) { continue; }
            // If bytes_read > size then we have over read, which shouldn't happen
            if (bytes_read > size)
            {
                fprintf(stderr, "read over read, too many bytes where read, this shouldn't happen (might be a logical error)!\n");
            }
        }
    }
    return bytes_read;
}


ssize_t
Serial::_write (const uint8_t *data, size_t length)
{
    if (!_arduinoConnected) {
        fprintf(stderr, "Arduino not connected!\n");
    }
    fd_set writefds;
    size_t bytes_written = 0;

    // Calculate total timeout in milliseconds t_c + (t_m * N)
    uint32_t total_timeout_ms = _timeout.write_timeout_constant;
    total_timeout_ms += _timeout.write_timeout_multiplier * static_cast<long> (length);
    MillisecondTimer total_timeout(total_timeout_ms);

    bool first_iteration = true;
    while (bytes_written < length) {
        int64_t timeout_remaining_ms = total_timeout.remaining();
        // Only consider the timeout if it's not the first iteration of the loop
        // otherwise a timeout of 0 won't be allowed through
        if (!first_iteration && (timeout_remaining_ms <= 0)) {
            // Timed out
            break;
        }
        first_iteration = false;

        timespec timeout(timespec_from_ms(static_cast<uint32_t>(timeout_remaining_ms)));

        FD_ZERO (&writefds);
        FD_SET (_serial, &writefds);

        // Do the select
        int r = pselect (_serial + 1, NULL, &writefds, NULL, &timeout, NULL);

        // Figure out what happened by looking at select's response 'r'
        /** Error **/
        if (r < 0) {
            // Select was interrupted, try again
            if (errno == EINTR) {
                continue;
            }
            // Otherwise there was some error
            fprintf(stderr, "Error with pselect()! errno: %u\n", errno);
            return -1;
        }
        /** Timeout **/
        if (r == 0) {
            break;
        }
        /** Port ready to write **/
        if (r > 0) {
            // Make sure our file descriptor is in the ready to write list
            if (FD_ISSET (_serial, &writefds)) {
                // This will write some
                ssize_t bytes_written_now =
                    ::write (_serial, data + bytes_written, length - bytes_written);

                // even though pselect returned readiness the call might still be
                // interrupted. In that case simply retry.
                if (bytes_written_now == -1 && errno == EINTR) {
                    continue;
                }

                // write should always return some data as select reported it was
                // ready to write when we get to this point.
                if (bytes_written_now < 1) {
                    // Disconnected devices, at least on Linux, show the
                    // behavior that they are always ready to write immediately
                    // but writing returns nothing.
                    std::stringstream strs;
                    strs << "device reports readiness to write but "
                            "returned no data (device disconnected?)";
                    strs << " errno=" << errno;
                    strs << " bytes_written_now= " << bytes_written_now;
                    strs << " bytes_written=" << bytes_written;
                    strs << " length=" << length;
                    fprintf(stderr, "%s", strs.str().c_str());
                    return -2;
                }
                // Update bytes_written
                bytes_written += static_cast<size_t> (bytes_written_now);
                // If bytes_written == size then we have written everything we need to
                if (bytes_written == length) {
                    break;
                }
                // If bytes_written < size then we have more to write
                if (bytes_written < length) {
                    continue;
                }
                // If bytes_written > size then we have over written, which shouldn't happen
                if (bytes_written > length) {
                    fprintf (stderr, "write over wrote, too many bytes where "
                                          "written, this shouldn't happen, might be "
                                          "a logical error!");
                    return -3;
                }
            }
            // This shouldn't happen, if r > 0 our fd has to be in the list!
            fprintf (stderr, "select reports ready to write, but our fd isn't"
                               " in the list, this shouldn't happen!");
            return -4;
        }
    }
    return static_cast<ssize_t>(bytes_written);
}
