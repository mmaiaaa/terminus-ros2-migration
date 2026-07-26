#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>

#include "anbot_chassis_protocol/candump_replay.hpp"

namespace
{

void printUsage(const char* program_name)
{
    std::cerr
        << "Usage: "
        << program_name
        << " <candump-log-file> [interface]\n"
        << "\n"
        << "Example:\n"
        << "  "
        << program_name
        << " baseline.log can0\n";
}

template<typename ValueType>
void printOptionalValue(
    const std::string& label,
    const std::optional<ValueType>& value)
{
    std::cout << label << ": ";

    if (value.has_value())
    {
        std::cout << *value;
    }
    else
    {
        std::cout << "not available";
    }

    std::cout << '\n';
}

}  // namespace

int main(int argc, char** argv)
{
    if (argc < 2 || argc > 3)
    {
        printUsage(argv[0]);
        return 1;
    }

    const std::string log_path = argv[1];

    const std::string interface_name =
        argc == 3
        ? argv[2]
        : "can0";

    std::ifstream input(log_path);

    if (!input.is_open())
    {
        std::cerr
            << "Failed to open candump log: "
            << log_path
            << '\n';

        return 2;
    }

    anbot::CandumpReplay replay;

    const auto statistics =
        replay.process(input, interface_name);

    std::cout << std::fixed << std::setprecision(6);

    std::cout
        << "Candump replay summary\n"
        << "======================\n"
        << "Log file: " << log_path << '\n'
        << "Selected interface: " << interface_name << '\n'
        << '\n'
        << "Line statistics\n"
        << "---------------\n"
        << "Total lines: " << statistics.total_lines << '\n'
        << "Parsed lines: " << statistics.parsed_lines << '\n'
        << "Malformed lines: " << statistics.malformed_lines << '\n'
        << '\n'
        << "Interface statistics\n"
        << "--------------------\n"
        << "Selected-interface frames: "
        << statistics.selected_interface_frames
        << '\n'
        << "Ignored-interface frames: "
        << statistics.ignored_interface_frames
        << '\n'
        << '\n'
        << "Decoded frame statistics\n"
        << "------------------------\n"
        << "Position frames (0x183): "
        << statistics.position_frames
        << '\n'
        << "Heading frames (0x283): "
        << statistics.heading_frames
        << '\n'
        << "Wheel-speed frames (0x184): "
        << statistics.wheel_speed_frames
        << '\n'
        << "Unknown or invalid frames: "
        << statistics.unknown_frames
        << '\n'
        << '\n';

    printOptionalValue(
        "First timestamp",
        statistics.first_timestamp);

    printOptionalValue(
        "Last timestamp",
        statistics.last_timestamp);

    if (
        statistics.first_timestamp.has_value() &&
        statistics.last_timestamp.has_value())
    {
        std::cout
            << "Duration: "
            << (*statistics.last_timestamp -
                *statistics.first_timestamp)
            << " seconds\n";
    }
    else
    {
        std::cout << "Duration: not available\n";
    }

    std::cout << '\n';

    if (statistics.first_position.has_value())
    {
        std::cout
            << "First position: x="
            << statistics.first_position->x
            << ", y="
            << statistics.first_position->y
            << '\n';
    }
    else
    {
        std::cout << "First position: not available\n";
    }

    if (statistics.last_position.has_value())
    {
        std::cout
            << "Last position: x="
            << statistics.last_position->x
            << ", y="
            << statistics.last_position->y
            << '\n';
    }
    else
    {
        std::cout << "Last position: not available\n";
    }

    if (statistics.first_heading.has_value())
    {
        std::cout
            << "First heading: theta="
            << statistics.first_heading->theta
            << '\n';
    }
    else
    {
        std::cout << "First heading: not available\n";
    }

    if (statistics.last_heading.has_value())
    {
        std::cout
            << "Last heading: theta="
            << statistics.last_heading->theta
            << '\n';
    }
    else
    {
        std::cout << "Last heading: not available\n";
    }

    std::cout << '\n';

    printOptionalValue(
        "Minimum left wheel speed",
        statistics.minimum_left_speed);

    printOptionalValue(
        "Maximum left wheel speed",
        statistics.maximum_left_speed);

    printOptionalValue(
        "Minimum right wheel speed",
        statistics.minimum_right_speed);

    printOptionalValue(
        "Maximum right wheel speed",
        statistics.maximum_right_speed);

    return 0;
}
