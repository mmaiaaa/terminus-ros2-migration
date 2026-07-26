#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <optional>

namespace anbot
{

struct CanFrame
{
    uint32_t id = 0;
    std::array<uint8_t, 8> data{};
    std::size_t size = 0;
};

struct ChassisPosition
{
    float x = 0.0F;
    float y = 0.0F;
};

struct ChassisHeading
{
    float theta = 0.0F;

    // The final four bytes of CAN ID 0x283 are preserved until
    // their exact meaning is confirmed.
    std::array<uint8_t, 4> trailing_bytes{};
};

struct ChassisWheelSpeeds
{
    int16_t left_speed = 0;
    int16_t right_speed = 0;

    // The final four bytes of CAN ID 0x184 are preserved until
    // their exact status-bit meanings are confirmed.
    std::array<uint8_t, 4> status_bytes{};
};

class ChassisCanProtocol
{
public:
    static constexpr uint32_t position_frame_id = 0x183U;
    static constexpr uint32_t wheel_speed_frame_id = 0x184U;
    static constexpr uint32_t heading_frame_id = 0x283U;

    static constexpr std::size_t chassis_frame_size = 8U;

    std::optional<ChassisPosition> decodePosition(
        const CanFrame& frame) const;

    std::optional<ChassisHeading> decodeHeading(
        const CanFrame& frame) const;

    std::optional<ChassisWheelSpeeds> decodeWheelSpeeds(
        const CanFrame& frame) const;
};

}  // namespace anbot
