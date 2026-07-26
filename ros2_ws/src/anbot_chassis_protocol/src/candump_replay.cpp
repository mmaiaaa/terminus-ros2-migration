#include "anbot_chassis_protocol/candump_replay.hpp"

#include <algorithm>
#include <string>

#include "anbot_chassis_protocol/candump_parser.hpp"

namespace
{

template<typename ValueType>
void updateMinimum(
    std::optional<ValueType>& current,
    const ValueType value)
{
    if (!current.has_value() || value < *current)
    {
        current = value;
    }
}

template<typename ValueType>
void updateMaximum(
    std::optional<ValueType>& current,
    const ValueType value)
{
    if (!current.has_value() || value > *current)
    {
        current = value;
    }
}

}  // namespace

namespace anbot
{

CandumpReplayStatistics CandumpReplay::process(
    std::istream& input,
    const std::string& interface_name) const
{
    CandumpReplayStatistics statistics;

    CandumpParser parser;
    ChassisCanProtocol protocol;

    std::string line;

    while (std::getline(input, line))
    {
        ++statistics.total_lines;

        const auto record = parser.parseLine(line);

        if (!record.has_value())
        {
            ++statistics.malformed_lines;
            continue;
        }

        ++statistics.parsed_lines;

        if (!statistics.first_timestamp.has_value())
        {
            statistics.first_timestamp = record->timestamp;
        }

        statistics.last_timestamp = record->timestamp;

        if (record->interface_name != interface_name)
        {
            ++statistics.ignored_interface_frames;
            continue;
        }

        ++statistics.selected_interface_frames;

        const auto position =
            protocol.decodePosition(record->frame);

        if (position.has_value())
        {
            ++statistics.position_frames;

            if (!statistics.first_position.has_value())
            {
                statistics.first_position = *position;
            }

            statistics.last_position = *position;
            continue;
        }

        const auto heading =
            protocol.decodeHeading(record->frame);

        if (heading.has_value())
        {
            ++statistics.heading_frames;

            if (!statistics.first_heading.has_value())
            {
                statistics.first_heading = *heading;
            }

            statistics.last_heading = *heading;
            continue;
        }

        const auto wheel_speeds =
            protocol.decodeWheelSpeeds(record->frame);

        if (wheel_speeds.has_value())
        {
            ++statistics.wheel_speed_frames;

            updateMinimum(
                statistics.minimum_left_speed,
                wheel_speeds->left_speed);

            updateMaximum(
                statistics.maximum_left_speed,
                wheel_speeds->left_speed);

            updateMinimum(
                statistics.minimum_right_speed,
                wheel_speeds->right_speed);

            updateMaximum(
                statistics.maximum_right_speed,
                wheel_speeds->right_speed);

            continue;
        }

        ++statistics.unknown_frames;
    }

    return statistics;
}

}  // namespace anbot
