#include "anbot_chassis_protocol/candump_parser.hpp"

#include <cstdint>
#include <sstream>
#include <string>

namespace
{

std::optional<uint8_t> decodeHexByte(
    const char high,
    const char low)
{
    const auto decodeNibble = [](const char value)
        -> std::optional<uint8_t>
    {
        if (value >= '0' && value <= '9')
        {
            return static_cast<uint8_t>(value - '0');
        }

        if (value >= 'A' && value <= 'F')
        {
            return static_cast<uint8_t>(value - 'A' + 10);
        }

        if (value >= 'a' && value <= 'f')
        {
            return static_cast<uint8_t>(value - 'a' + 10);
        }

        return std::nullopt;
    };

    const auto high_nibble = decodeNibble(high);
    const auto low_nibble = decodeNibble(low);

    if (!high_nibble.has_value() || !low_nibble.has_value())
    {
        return std::nullopt;
    }

    return static_cast<uint8_t>(
        static_cast<uint8_t>(*high_nibble << 4U) |
        *low_nibble);
}

}  // namespace

namespace anbot
{

std::optional<CandumpRecord> CandumpParser::parseLine(
    const std::string& line) const
{
    std::istringstream stream(line);

    std::string timestamp_token;
    std::string interface_name;
    std::string frame_token;

    if (!(stream >> timestamp_token >> interface_name >> frame_token))
    {
        return std::nullopt;
    }

    if (
        timestamp_token.size() < 3U ||
        timestamp_token.front() != '(' ||
        timestamp_token.back() != ')')
    {
        return std::nullopt;
    }

    const std::string timestamp_text =
        timestamp_token.substr(1U, timestamp_token.size() - 2U);

    double timestamp = 0.0;

    try
    {
        std::size_t parsed_characters = 0U;

        timestamp = std::stod(
            timestamp_text,
            &parsed_characters);

        if (parsed_characters != timestamp_text.size())
        {
            return std::nullopt;
        }
    }
    catch (...)
    {
        return std::nullopt;
    }

    const std::size_t separator_position =
        frame_token.find('#');

    if (
        separator_position == std::string::npos ||
        separator_position == 0U)
    {
        return std::nullopt;
    }

    const std::string id_text =
        frame_token.substr(0U, separator_position);

    const std::string payload_text =
        frame_token.substr(separator_position + 1U);

    if (
        payload_text.size() % 2U != 0U ||
        payload_text.size() > 16U)
    {
        return std::nullopt;
    }

    uint32_t frame_id = 0U;

    try
    {
        std::size_t parsed_characters = 0U;

        frame_id = static_cast<uint32_t>(
            std::stoul(
                id_text,
                &parsed_characters,
                16));

        if (parsed_characters != id_text.size())
        {
            return std::nullopt;
        }
    }
    catch (...)
    {
        return std::nullopt;
    }

    CanFrame frame;
    frame.id = frame_id;
    frame.size = payload_text.size() / 2U;

    for (std::size_t index = 0U;
         index < frame.size;
         ++index)
    {
        const auto byte = decodeHexByte(
            payload_text[index * 2U],
            payload_text[index * 2U + 1U]);

        if (!byte.has_value())
        {
            return std::nullopt;
        }

        frame.data[index] = *byte;
    }

    CandumpRecord record;
    record.timestamp = timestamp;
    record.interface_name = interface_name;
    record.frame = frame;

    return record;
}

}  // namespace anbot
