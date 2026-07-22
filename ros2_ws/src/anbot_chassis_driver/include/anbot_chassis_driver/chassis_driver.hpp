#pragma once

#include <cstddef>
#include <memory>
#include <optional>
#include <vector>

#include "anbot_chassis_protocol/wheel_protocol.hpp"
#include "anbot_chassis_protocol/wheel_stream_parser.hpp"
#include "anbot_chassis_transport/transport.hpp"

namespace anbot
{

class ChassisDriver
{
public:
    explicit ChassisDriver(
        std::shared_ptr<Transport> transport);

    bool open();

    void close();

    bool isOpen() const;

    bool sendWheelCommand(
        const WheelCommand& command);

    std::vector<WheelFeedback> pollFeedback(
        std::size_t maximum_read_size = 256);

    std::optional<WheelFeedback> latestFeedback() const;

    std::size_t transmittedFrameCount() const;

    std::size_t receivedFrameCount() const;

    std::size_t failedWriteCount() const;

    void resetStatistics();

private:
    std::shared_ptr<Transport> transport_;

    WheelProtocol protocol_;
    WheelStreamParser parser_;

    std::optional<WheelFeedback> latest_feedback_;

    std::size_t transmitted_frame_count_ = 0;
    std::size_t received_frame_count_ = 0;
    std::size_t failed_write_count_ = 0;
};

}  // namespace anbot