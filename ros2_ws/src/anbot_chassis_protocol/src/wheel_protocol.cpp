#include "anbot_chassis_protocol/wheel_protocol.hpp"

#include <stdexcept>

namespace
{

int16_t decodeSignedBigEndian(
    const uint8_t high_byte,
    const uint8_t low_byte)
{
    const uint16_t raw =
        static_cast<uint16_t>(
            static_cast<uint16_t>(high_byte) << 8U) |
        static_cast<uint16_t>(low_byte);

    return static_cast<int16_t>(raw);
}

}  // namespace

namespace anbot
{

std::vector<uint8_t> WheelProtocol::encodeCommand(
    const WheelCommand& command)
{
    if (command.left_speed == 0 && command.right_speed == 0)
    {
        ++consecutive_zero_commands_;
    }
    else
    {
        consecutive_zero_commands_ = 0;
    }

    std::vector<uint8_t> frame(command_frame_size, 0);

    frame[0] = command_header;

    const auto left = static_cast<uint16_t>(command.left_speed);
    const auto right = static_cast<uint16_t>(command.right_speed);

    frame[1] = static_cast<uint8_t>((left >> 8U) & 0xFFU);
    frame[2] = static_cast<uint8_t>(left & 0xFFU);

    frame[3] = static_cast<uint8_t>((right >> 8U) & 0xFFU);
    frame[4] = static_cast<uint8_t>(right & 0xFFU);

    // This reproduces the recovered binary exactly:
    // the marker is emitted only on the second consecutive zero command.
    if (consecutive_zero_commands_ == 2U)
    {
        frame[5] = repeated_stop_marker;
    }

    frame[6] = command.emergency_stop ? 0x01U : 0x00U;

    frame[7] = checksum(frame, 7);

    return frame;
}

std::optional<WheelFeedback> WheelProtocol::decodeFrame(
    const std::vector<uint8_t>& frame)
{
    if (frame.size() != feedback_frame_size)
    {
        return std::nullopt;
    }

    if (frame[0] != feedback_header)
    {
        return std::nullopt;
    }

    const uint8_t expected_checksum = checksum(frame, 9);

    if (frame[9] != expected_checksum)
    {
        return std::nullopt;
    }

    WheelFeedback feedback;

    feedback.left_value =
        decodeSignedBigEndian(frame[1], frame[2]);

    feedback.right_value =
        decodeSignedBigEndian(frame[3], frame[4]);

    /*
     * Bytes 5 through 7 remain reserved until their meanings are
     * verified from further disassembly or captured hardware traffic.
     */

    feedback.status = frame[8];
    feedback.valid = true;

    return feedback;
}

void WheelProtocol::reset()
{
    consecutive_zero_commands_ = 0;
}

uint8_t WheelProtocol::checksum(
    const std::vector<uint8_t>& frame,
    const std::size_t byte_count) const
{
    if (byte_count > frame.size())
    {
        throw std::out_of_range(
            "Checksum byte count exceeds frame size");
    }

    uint32_t sum = 0;

    for (std::size_t index = 0; index < byte_count; ++index)
    {
        sum += frame[index];
    }

    return static_cast<uint8_t>(sum & 0xFFU);
}

}  // namespace anbot