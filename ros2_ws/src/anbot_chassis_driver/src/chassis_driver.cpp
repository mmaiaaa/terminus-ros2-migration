#include "anbot_chassis_driver/chassis_driver.hpp"

#include <stdexcept>
#include <utility>

namespace anbot
{

ChassisDriver::ChassisDriver(
    std::shared_ptr<Transport> transport)
    : transport_(std::move(transport))
{
    if (!transport_)
    {
        throw std::invalid_argument(
            "ChassisDriver requires a valid transport");
    }
}

bool ChassisDriver::open()
{
    parser_.reset();
    protocol_.reset();
    latest_feedback_.reset();

    return transport_->open();
}

void ChassisDriver::close()
{
    transport_->close();
}

bool ChassisDriver::isOpen() const
{
    return transport_->isOpen();
}

bool ChassisDriver::sendWheelCommand(
    const WheelCommand& command)
{
    if (!transport_->isOpen())
    {
        ++failed_write_count_;
        return false;
    }

    const auto frame = protocol_.encodeCommand(command);

    const std::size_t bytes_written =
        transport_->write(frame.data(), frame.size());

    if (bytes_written != frame.size())
    {
        ++failed_write_count_;
        return false;
    }

    ++transmitted_frame_count_;
    return true;
}

std::vector<WheelFeedback> ChassisDriver::pollFeedback(
    const std::size_t maximum_read_size)
{
    if (!transport_->isOpen() || maximum_read_size == 0)
    {
        return {};
    }

    const auto bytes = transport_->read(maximum_read_size);

    if (bytes.empty())
    {
        return {};
    }

    auto messages = parser_.pushBytes(bytes);

    received_frame_count_ += messages.size();

    if (!messages.empty())
    {
        latest_feedback_ = messages.back();
    }

    return messages;
}

std::optional<WheelFeedback>
ChassisDriver::latestFeedback() const
{
    return latest_feedback_;
}

std::size_t ChassisDriver::transmittedFrameCount() const
{
    return transmitted_frame_count_;
}

std::size_t ChassisDriver::receivedFrameCount() const
{
    return received_frame_count_;
}

std::size_t ChassisDriver::failedWriteCount() const
{
    return failed_write_count_;
}

void ChassisDriver::resetStatistics()
{
    transmitted_frame_count_ = 0;
    received_frame_count_ = 0;
    failed_write_count_ = 0;
}

}  // namespace anbot