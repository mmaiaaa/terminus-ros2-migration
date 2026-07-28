#include "anbot_chassis_driver/chassis_can_driver.hpp"

#include <utility>

namespace anbot
{

ChassisCanDriver::ChassisCanDriver(
    SocketCanConfiguration configuration)
    : receiver_(std::move(configuration))
{
}

bool ChassisCanDriver::open()
{
    latest_position_.reset();
    latest_heading_.reset();
    latest_wheel_speeds_.reset();

    return receiver_.open();
}

void ChassisCanDriver::close()
{
    receiver_.close();
}

bool ChassisCanDriver::isOpen() const
{
    return receiver_.isOpen();
}

std::size_t ChassisCanDriver::poll(
    const std::size_t maximum_frame_count)
{
    if (!receiver_.isOpen() || maximum_frame_count == 0U)
    {
        return 0U;
    }

    std::size_t processed_count = 0U;

    while (processed_count < maximum_frame_count)
    {
        const auto frame = receiver_.receive();

        if (!frame.has_value())
        {
            break;
        }

        processFrame(*frame);
        ++processed_count;
    }

    return processed_count;
}

bool ChassisCanDriver::processFrame(
    const CanFrame& frame)
{
    ++received_frame_count_;

    if (const auto position = protocol_.decodePosition(frame))
    {
        latest_position_ = *position;
        ++decoded_frame_count_;
        return true;
    }

    if (const auto heading = protocol_.decodeHeading(frame))
    {
        latest_heading_ = *heading;
        ++decoded_frame_count_;
        return true;
    }

    if (
        const auto wheel_speeds =
            protocol_.decodeWheelSpeeds(frame))
    {
        latest_wheel_speeds_ = *wheel_speeds;
        ++decoded_frame_count_;
        return true;
    }

    ++unknown_frame_count_;
    return false;
}

std::optional<ChassisPosition>
ChassisCanDriver::latestPosition() const
{
    return latest_position_;
}

std::optional<ChassisHeading>
ChassisCanDriver::latestHeading() const
{
    return latest_heading_;
}

std::optional<ChassisWheelSpeeds>
ChassisCanDriver::latestWheelSpeeds() const
{
    return latest_wheel_speeds_;
}

std::size_t ChassisCanDriver::receivedFrameCount() const
{
    return received_frame_count_;
}

std::size_t ChassisCanDriver::decodedFrameCount() const
{
    return decoded_frame_count_;
}

std::size_t ChassisCanDriver::unknownFrameCount() const
{
    return unknown_frame_count_;
}

void ChassisCanDriver::resetStatistics()
{
    received_frame_count_ = 0U;
    decoded_frame_count_ = 0U;
    unknown_frame_count_ = 0U;
}

int ChassisCanDriver::lastSocketError() const
{
    return receiver_.lastError();
}

const SocketCanConfiguration&
ChassisCanDriver::configuration() const
{
    return receiver_.configuration();
}

}  // namespace anbot
