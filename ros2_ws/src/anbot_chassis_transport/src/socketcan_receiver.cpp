#include "anbot_chassis_transport/socketcan_receiver.hpp"

#include <cerrno>
#include <cstring>
#include <stdexcept>
#include <utility>

#include <fcntl.h>
#include <linux/can.h>
#include <linux/can/raw.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <unistd.h>

namespace anbot
{

SocketCanReceiver::SocketCanReceiver(
    SocketCanConfiguration configuration)
    : configuration_(std::move(configuration))
{
    if (configuration_.interface_name.empty())
    {
        throw std::invalid_argument(
            "SocketCAN interface name must not be empty");
    }

    if (configuration_.interface_name.size() >= IFNAMSIZ)
    {
        throw std::invalid_argument(
            "SocketCAN interface name is too long");
    }
}

SocketCanReceiver::~SocketCanReceiver()
{
    close();
}

bool SocketCanReceiver::open()
{
    std::lock_guard<std::mutex> lock(mutex_);

    if (socket_descriptor_ >= 0)
    {
        return true;
    }

    last_error_ = 0;

    const int descriptor = ::socket(
        PF_CAN,
        SOCK_RAW | SOCK_NONBLOCK,
        CAN_RAW);

    if (descriptor < 0)
    {
        last_error_ = errno;
        return false;
    }

    ifreq interface_request{};

    std::strncpy(
        interface_request.ifr_name,
        configuration_.interface_name.c_str(),
        IFNAMSIZ - 1);

    interface_request.ifr_name[IFNAMSIZ - 1] = '\0';

    if (
        ::ioctl(
            descriptor,
            SIOCGIFINDEX,
            &interface_request) < 0)
    {
        const int ioctl_error = errno;

        ::close(descriptor);

        last_error_ = ioctl_error;
        return false;
    }

    sockaddr_can address{};
    address.can_family = AF_CAN;
    address.can_ifindex = interface_request.ifr_ifindex;

    if (
        ::bind(
            descriptor,
            reinterpret_cast<const sockaddr*>(&address),
            sizeof(address)) < 0)
    {
        const int bind_error = errno;

        ::close(descriptor);

        last_error_ = bind_error;
        return false;
    }

    socket_descriptor_ = descriptor;
    return true;
}

void SocketCanReceiver::close()
{
    std::lock_guard<std::mutex> lock(mutex_);

    if (socket_descriptor_ < 0)
    {
        return;
    }

    ::close(socket_descriptor_);
    socket_descriptor_ = -1;
}

bool SocketCanReceiver::isOpen() const
{
    std::lock_guard<std::mutex> lock(mutex_);

    return socket_descriptor_ >= 0;
}

std::optional<CanFrame> SocketCanReceiver::receive()
{
    std::lock_guard<std::mutex> lock(mutex_);

    if (socket_descriptor_ < 0)
    {
        return std::nullopt;
    }

    can_frame native_frame{};

    const ssize_t result = ::read(
        socket_descriptor_,
        &native_frame,
        sizeof(native_frame));

    if (result < 0)
    {
        if (
            errno != EAGAIN &&
            errno != EWOULDBLOCK &&
            errno != EINTR)
        {
            last_error_ = errno;
        }

        return std::nullopt;
    }

    if (result != static_cast<ssize_t>(sizeof(native_frame)))
    {
        last_error_ = EIO;
        return std::nullopt;
    }

    /*
     * Initial implementation accepts only standard or extended
     * classical CAN data frames. Error and RTR frames are ignored.
     */
    if (
        (native_frame.can_id & CAN_ERR_FLAG) != 0U ||
        (native_frame.can_id & CAN_RTR_FLAG) != 0U)
    {
        return std::nullopt;
    }

    CanFrame frame;

    if ((native_frame.can_id & CAN_EFF_FLAG) != 0U)
    {
        frame.id = native_frame.can_id & CAN_EFF_MASK;
    }
    else
    {
        frame.id = native_frame.can_id & CAN_SFF_MASK;
    }

    frame.size = native_frame.can_dlc;

    if (frame.size > frame.data.size())
    {
        last_error_ = EPROTO;
        return std::nullopt;
    }

    for (std::size_t index = 0U;
         index < frame.size;
         ++index)
    {
        frame.data[index] = native_frame.data[index];
    }

    return frame;
}

const SocketCanConfiguration&
SocketCanReceiver::configuration() const
{
    return configuration_;
}

int SocketCanReceiver::lastError() const
{
    std::lock_guard<std::mutex> lock(mutex_);

    return last_error_;
}

}  // namespace anbot
