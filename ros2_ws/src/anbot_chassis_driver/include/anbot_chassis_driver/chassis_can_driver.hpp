#pragma once

#include <cstddef>
#include <optional>

#include "anbot_chassis_protocol/chassis_can_protocol.hpp"
#include "anbot_chassis_transport/socketcan_receiver.hpp"

namespace anbot
{

class ChassisCanDriver
{
public:
    explicit ChassisCanDriver(
        SocketCanConfiguration configuration);

    bool open();

    void close();

    bool isOpen() const;

    /*
     * Reads and processes up to maximum_frame_count frames.
     *
     * SocketCanReceiver is non-blocking, so this function returns
     * immediately when no further CAN frame is available.
     */
    std::size_t poll(
        std::size_t maximum_frame_count = 64U);

    /*
     * Processes one already-received frame.
     *
     * This method allows protocol/state behavior to be tested without
     * requiring a physical or virtual CAN interface.
     *
     * Returns true when the frame is one of the confirmed chassis
     * message types and was decoded successfully.
     */
    bool processFrame(const CanFrame& frame);

    std::optional<ChassisPosition> latestPosition() const;

    std::optional<ChassisHeading> latestHeading() const;

    std::optional<ChassisWheelSpeeds> latestWheelSpeeds() const;

    std::size_t receivedFrameCount() const;

    std::size_t decodedFrameCount() const;

    std::size_t unknownFrameCount() const;

    void resetStatistics();

    int lastSocketError() const;

    const SocketCanConfiguration& configuration() const;

private:
    SocketCanReceiver receiver_;
    ChassisCanProtocol protocol_;

    std::optional<ChassisPosition> latest_position_;
    std::optional<ChassisHeading> latest_heading_;
    std::optional<ChassisWheelSpeeds> latest_wheel_speeds_;

    std::size_t received_frame_count_ = 0U;
    std::size_t decoded_frame_count_ = 0U;
    std::size_t unknown_frame_count_ = 0U;
};

}  // namespace anbot
