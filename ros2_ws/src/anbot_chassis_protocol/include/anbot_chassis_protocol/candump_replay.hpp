#pragma once

#include <cstddef>
#include <cstdint>
#include <istream>
#include <optional>
#include <string>

#include "anbot_chassis_protocol/chassis_can_protocol.hpp"

namespace anbot
{

struct CandumpReplayStatistics
{
    std::size_t total_lines = 0U;
    std::size_t parsed_lines = 0U;
    std::size_t malformed_lines = 0U;

    std::size_t selected_interface_frames = 0U;
    std::size_t ignored_interface_frames = 0U;

    std::size_t position_frames = 0U;
    std::size_t heading_frames = 0U;
    std::size_t wheel_speed_frames = 0U;
    std::size_t unknown_frames = 0U;

    std::optional<double> first_timestamp;
    std::optional<double> last_timestamp;

    std::optional<ChassisPosition> first_position;
    std::optional<ChassisPosition> last_position;

    std::optional<ChassisHeading> first_heading;
    std::optional<ChassisHeading> last_heading;

    std::optional<int16_t> minimum_left_speed;
    std::optional<int16_t> maximum_left_speed;
    std::optional<int16_t> minimum_right_speed;
    std::optional<int16_t> maximum_right_speed;
};

class CandumpReplay
{
public:
    CandumpReplayStatistics process(
        std::istream& input,
        const std::string& interface_name = "can0") const;
};

}  // namespace anbot
