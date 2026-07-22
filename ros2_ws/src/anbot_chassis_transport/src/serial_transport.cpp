#include "anbot_chassis_transport/serial_transport.hpp"

#include <cerrno>
#include <cstddef>
#include <cstdint>
#include <fcntl.h>
#include <stdexcept>
#include <termios.h>
#include <unistd.h>
#include <utility>

namespace anbot
{

SerialTransport::SerialTransport(
    SerialConfiguration configuration)
    : configuration_(std::move(configuration))
{
    if (configuration_.device.empty())
    {
        throw std::invalid_argument(
            "Serial device path must not be empty");
    }
}

SerialTransport::~SerialTransport()
{
    close();
}

bool SerialTransport::open()
{
    std::lock_guard<std::mutex> lock(mutex_);

    if (file_descriptor_ >= 0)
    {
        return true;
    }

    last_error_ = 0;

    const int descriptor = ::open(
        configuration_.device.c_str(),
        O_RDWR | O_NOCTTY | O_NONBLOCK);

    if (descriptor < 0)
    {
        last_error_ = errno;
        return false;
    }

    if (!configurePort(descriptor))
    {
        const int configuration_error = last_error_;

        ::close(descriptor);

        last_error_ = configuration_error;
        return false;
    }

    file_descriptor_ = descriptor;
    return true;
}

void SerialTransport::close()
{
    std::lock_guard<std::mutex> lock(mutex_);

    if (file_descriptor_ < 0)
    {
        return;
    }

    ::close(file_descriptor_);
    file_descriptor_ = -1;
}

bool SerialTransport::isOpen() const
{
    std::lock_guard<std::mutex> lock(mutex_);

    return file_descriptor_ >= 0;
}

std::size_t SerialTransport::write(
    const uint8_t* data,
    const std::size_t size)
{
    if (data == nullptr && size != 0)
    {
        throw std::invalid_argument(
            "Null serial write pointer with non-zero size");
    }

    if (size == 0)
    {
        return 0;
    }

    std::lock_guard<std::mutex> lock(mutex_);

    if (file_descriptor_ < 0)
    {
        return 0;
    }

    std::size_t total_written = 0;

    while (total_written < size)
    {
        const ssize_t result = ::write(
            file_descriptor_,
            data + total_written,
            size - total_written);

        if (result > 0)
        {
            total_written += static_cast<std::size_t>(result);
            continue;
        }

        if (result < 0 && errno == EINTR)
        {
            continue;
        }

        if (
            result < 0 &&
            (errno == EAGAIN || errno == EWOULDBLOCK))
        {
            break;
        }

        if (result < 0)
        {
            last_error_ = errno;
        }

        break;
    }

    return total_written;
}

std::vector<uint8_t> SerialTransport::read(
    const std::size_t maximum_size)
{
    if (maximum_size == 0)
    {
        return {};
    }

    std::lock_guard<std::mutex> lock(mutex_);

    if (file_descriptor_ < 0)
    {
        return {};
    }

    std::vector<uint8_t> buffer(maximum_size);

    const ssize_t result = ::read(
        file_descriptor_,
        buffer.data(),
        buffer.size());

    if (result > 0)
    {
        buffer.resize(static_cast<std::size_t>(result));
        return buffer;
    }

    if (result < 0)
    {
        if (
            errno != EAGAIN &&
            errno != EWOULDBLOCK &&
            errno != EINTR)
        {
            last_error_ = errno;
        }
    }

    return {};
}

const SerialConfiguration&
SerialTransport::configuration() const
{
    return configuration_;
}

int SerialTransport::lastError() const
{
    std::lock_guard<std::mutex> lock(mutex_);

    return last_error_;
}

bool SerialTransport::configurePort(
    const int file_descriptor)
{
    termios options{};

    if (::tcgetattr(file_descriptor, &options) != 0)
    {
        setLastError(errno);
        return false;
    }

    unsigned int baud_constant = 0;

    if (!convertBaudRate(
            configuration_.baud_rate,
            baud_constant))
    {
        setLastError(EINVAL);
        return false;
    }

    ::cfmakeraw(&options);

    if (
        ::cfsetispeed(
            &options,
            static_cast<speed_t>(baud_constant)) != 0 ||
        ::cfsetospeed(
            &options,
            static_cast<speed_t>(baud_constant)) != 0)
    {
        setLastError(errno);
        return false;
    }

    /*
     * Initial framing configuration:
     *
     * 8 data bits
     * no parity
     * one stop bit
     * local connection
     * receiver enabled
     */
    options.c_cflag &= ~CSIZE;
    options.c_cflag |= CS8;

    options.c_cflag &= ~PARENB;
    options.c_cflag &= ~CSTOPB;

    options.c_cflag |= CLOCAL;
    options.c_cflag |= CREAD;

#ifdef CRTSCTS
    if (configuration_.hardware_flow_control)
    {
        options.c_cflag |= CRTSCTS;
    }
    else
    {
        options.c_cflag &= ~CRTSCTS;
    }
#endif

    if (configuration_.software_flow_control)
    {
        options.c_iflag |= IXON;
        options.c_iflag |= IXOFF;
    }
    else
    {
        options.c_iflag &= ~IXON;
        options.c_iflag &= ~IXOFF;
        options.c_iflag &= ~IXANY;
    }

    /*
     * Reads are non-blocking. The driver decides how frequently
     * to poll the transport.
     */
    options.c_cc[VMIN] = 0;
    options.c_cc[VTIME] = 0;

    if (::tcsetattr(
            file_descriptor,
            TCSANOW,
            &options) != 0)
    {
        setLastError(errno);
        return false;
    }

    if (::tcflush(file_descriptor, TCIOFLUSH) != 0)
    {
        setLastError(errno);
        return false;
    }

    return true;
}

bool SerialTransport::convertBaudRate(
    const int baud_rate,
    unsigned int& output_speed)
{
    switch (baud_rate)
    {
        case 9600:
            output_speed = B9600;
            return true;

        case 19200:
            output_speed = B19200;
            return true;

        case 38400:
            output_speed = B38400;
            return true;

        case 57600:
            output_speed = B57600;
            return true;

        case 115200:
            output_speed = B115200;
            return true;

        default:
            return false;
    }
}

void SerialTransport::setLastError(
    const int error_number)
{
    last_error_ = error_number;
}

}  // namespace anbot