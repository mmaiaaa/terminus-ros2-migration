#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <optional>
#include <vector>

namespace anbot
{

struct WheelCommand
{
    int16_t left_speed = 0;
    int16_t right_speed = 0;
    bool emergency_stop = false;
};

struct WheelFeedback
{
    int16_t left_value = 0;
    int16_t right_value = 0;

    uint8_t status = 0;

    bool valid = false;
};

class WheelProtocol
{
public:
    static constexpr std::size_t command_frame_size = 8;
    static constexpr uint8_t command_header = 0x04;
    static constexpr uint8_t repeated_stop_marker = 0xAA;
    static constexpr std::size_t feedback_frame_size = 10;
    static constexpr uint8_t feedback_header = 0x05;

    std::vector<uint8_t> encodeCommand(const WheelCommand& command);

    std::optional<WheelFeedback> decodeFrame(
        const std::vector<uint8_t>& frame);

    void reset();

private:
    uint8_t checksum(
        const std::vector<uint8_t>& frame,
        std::size_t byte_count) const;

    uint32_t consecutive_zero_commands_ = 0;
};

}  // namespace anbot