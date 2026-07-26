#pragma once

#include <mutex>
#include <optional>
#include <string>

#include "anbot_chassis_protocol/chassis_can_protocol.hpp"

namespace anbot
{

struct SocketCanConfiguration
{
    std::string interface_name = "can0";
};

class SocketCanReceiver final
{
public:
    explicit SocketCanReceiver(
        SocketCanConfiguration configuration);

    ~SocketCanReceiver();

    SocketCanReceiver(const SocketCanReceiver&) = delete;
    SocketCanReceiver& operator=(const SocketCanReceiver&) = delete;

    bool open();

    void close();

    bool isOpen() const;

    std::optional<CanFrame> receive();

    const SocketCanConfiguration& configuration() const;

    int lastError() const;

private:
    SocketCanConfiguration configuration_;

    mutable std::mutex mutex_;

    int socket_descriptor_ = -1;
    int last_error_ = 0;
};

}  // namespace anbot
