#pragma once

#include <cstddef>
#include <cstdint>
#include <vector>

#include "anbot_chassis_protocol/wheel_protocol.hpp"

namespace anbot
{

class WheelStreamParser
{
public:
    explicit WheelStreamParser(
        std::size_t maximum_buffer_size = 4096);

    std::vector<WheelFeedback> pushBytes(
        const uint8_t* data,
        std::size_t size);

    std::vector<WheelFeedback> pushBytes(
        const std::vector<uint8_t>& data);

    void reset();

    std::size_t bufferedByteCount() const;

    std::size_t discardedByteCount() const;

    std::size_t invalidFrameCount() const;

private:
    void enforceBufferLimit();

    WheelProtocol protocol_;
    std::vector<uint8_t> buffer_;

    std::size_t maximum_buffer_size_;
    std::size_t discarded_byte_count_ = 0;
    std::size_t invalid_frame_count_ = 0;
};

}  // namespace anbot