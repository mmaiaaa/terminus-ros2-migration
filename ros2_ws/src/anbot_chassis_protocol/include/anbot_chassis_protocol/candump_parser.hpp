#pragma once

#include <optional>
#include <string>

#include "anbot_chassis_protocol/chassis_can_protocol.hpp"

namespace anbot
{

struct CandumpRecord
{
    double timestamp = 0.0;
    std::string interface_name;
    CanFrame frame;
};

class CandumpParser
{
public:
    std::optional<CandumpRecord> parseLine(
        const std::string& line) const;
};

}  // namespace anbot
