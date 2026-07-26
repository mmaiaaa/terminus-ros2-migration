#include "anbot_chassis_protocol/chassis_can_protocol.hpp"

#include <cstring>

namespace
{

uint16_t decodeUnsignedLittleEndian16(
    const uint8_t low_byte,
    const uint8_t high_byte)
{
    return static_cast<uint16_t>(
        static_cast<uint16_t>(low_byte) |
        (static_cast<uint16_t>(high_byte) << 8U));
}

int16_t decodeSignedLittleEndian16(
    const uint8_t low_byte,
    const uint8_t high_byte)
{
    return static_cast<int16_t>(
        decodeUnsignedLittleEndian16(low_byte, high_byte));
}

uint32_t decodeUnsignedLittleEndian32(
    const uint8_t byte_0,
    const uint8_t byte_1,
    const uint8_t byte_2,
    const uint8_t byte_3)
{
    return
        static_cast<uint32_t>(byte_0) |
        (static_cast<uint32_t>(byte_1) << 8U) |
        (static_cast<uint32_t>(byte_2) << 16U) |
        (static_cast<uint32_t>(byte_3) << 24U);
}

float decodeFloatLittleEndian(
    const uint8_t byte_0,
    const uint8_t byte_1,
    const uint8_t byte_2,
    const uint8_t byte_3)
{
    const uint32_t raw = decodeUnsignedLittleEndian32(
        byte_0,
        byte_1,
        byte_2,
        byte_3);

    float value = 0.0F;

    static_assert(
        sizeof(value) == sizeof(raw),
        "Expected float to contain 32 bits");

    std::memcpy(&value, &raw, sizeof(value));

    return value;
}

bool isExpectedFrame(
    const anbot::CanFrame& frame,
    const uint32_t expected_id)
{
    return
        frame.id == expected_id &&
        frame.size == anbot::ChassisCanProtocol::chassis_frame_size;
}

}  // namespace

namespace anbot
{

std::optional<ChassisPosition> ChassisCanProtocol::decodePosition(
    const CanFrame& frame) const
{
    if (!isExpectedFrame(frame, position_frame_id))
    {
        return std::nullopt;
    }

    ChassisPosition position;

    position.x = decodeFloatLittleEndian(
        frame.data[0],
        frame.data[1],
        frame.data[2],
        frame.data[3]);

    position.y = decodeFloatLittleEndian(
        frame.data[4],
        frame.data[5],
        frame.data[6],
        frame.data[7]);

    return position;
}

std::optional<ChassisHeading> ChassisCanProtocol::decodeHeading(
    const CanFrame& frame) const
{
    if (!isExpectedFrame(frame, heading_frame_id))
    {
        return std::nullopt;
    }

    ChassisHeading heading;

    heading.theta = decodeFloatLittleEndian(
        frame.data[0],
        frame.data[1],
        frame.data[2],
        frame.data[3]);

    heading.trailing_bytes = {
        frame.data[4],
        frame.data[5],
        frame.data[6],
        frame.data[7]
    };

    return heading;
}

std::optional<ChassisWheelSpeeds>
ChassisCanProtocol::decodeWheelSpeeds(
    const CanFrame& frame) const
{
    if (!isExpectedFrame(frame, wheel_speed_frame_id))
    {
        return std::nullopt;
    }

    ChassisWheelSpeeds wheel_speeds;

    wheel_speeds.left_speed = decodeSignedLittleEndian16(
        frame.data[0],
        frame.data[1]);

    wheel_speeds.right_speed = decodeSignedLittleEndian16(
        frame.data[2],
        frame.data[3]);

    wheel_speeds.status_bytes = {
        frame.data[4],
        frame.data[5],
        frame.data[6],
        frame.data[7]
    };

    return wheel_speeds;
}

}  // namespace anbot
