#include "anbot_chassis_protocol/wheel_stream_parser.hpp"

#include <algorithm>
#include <stdexcept>

namespace anbot
{

WheelStreamParser::WheelStreamParser(
    const std::size_t maximum_buffer_size)
    : maximum_buffer_size_(maximum_buffer_size)
{
    if (maximum_buffer_size_ < WheelProtocol::feedback_frame_size)
    {
        throw std::invalid_argument(
            "Maximum stream buffer size is smaller than one feedback frame");
    }

    buffer_.reserve(maximum_buffer_size_);
}

std::vector<WheelFeedback> WheelStreamParser::pushBytes(
    const uint8_t* data,
    const std::size_t size)
{
    if (data == nullptr && size != 0)
    {
        throw std::invalid_argument(
            "Null input pointer with non-zero byte count");
    }

    if (size != 0)
    {
        buffer_.insert(buffer_.end(), data, data + size);
    }

    enforceBufferLimit();

    std::vector<WheelFeedback> feedback_messages;

    while (true)
    {
        const auto header_position = std::find(
            buffer_.begin(),
            buffer_.end(),
            WheelProtocol::feedback_header);

        if (header_position == buffer_.end())
        {
            discarded_byte_count_ += buffer_.size();
            buffer_.clear();
            break;
        }

        const auto bytes_before_header = static_cast<std::size_t>(
            std::distance(buffer_.begin(), header_position));

        if (bytes_before_header != 0)
        {
            discarded_byte_count_ += bytes_before_header;

            buffer_.erase(
                buffer_.begin(),
                header_position);
        }

        if (buffer_.size() < WheelProtocol::feedback_frame_size)
        {
            break;
        }

        const std::vector<uint8_t> candidate(
            buffer_.begin(),
            buffer_.begin() +
                static_cast<std::ptrdiff_t>(
                    WheelProtocol::feedback_frame_size));

        const auto feedback = protocol_.decodeFrame(candidate);

        if (feedback.has_value())
        {
            feedback_messages.push_back(*feedback);

            buffer_.erase(
                buffer_.begin(),
                buffer_.begin() +
                    static_cast<std::ptrdiff_t>(
                        WheelProtocol::feedback_frame_size));

            continue;
        }

        /*
         * The current header did not begin a valid frame.
         *
         * Discard only that one byte. The following bytes may contain
         * another valid 0x05 header.
         */
        ++invalid_frame_count_;
        ++discarded_byte_count_;

        buffer_.erase(buffer_.begin());
    }

    return feedback_messages;
}

std::vector<WheelFeedback> WheelStreamParser::pushBytes(
    const std::vector<uint8_t>& data)
{
    return pushBytes(data.data(), data.size());
}

void WheelStreamParser::reset()
{
    buffer_.clear();
    protocol_.reset();

    discarded_byte_count_ = 0;
    invalid_frame_count_ = 0;
}

std::size_t WheelStreamParser::bufferedByteCount() const
{
    return buffer_.size();
}

std::size_t WheelStreamParser::discardedByteCount() const
{
    return discarded_byte_count_;
}

std::size_t WheelStreamParser::invalidFrameCount() const
{
    return invalid_frame_count_;
}

void WheelStreamParser::enforceBufferLimit()
{
    if (buffer_.size() <= maximum_buffer_size_)
    {
        return;
    }

    const std::size_t excess =
        buffer_.size() - maximum_buffer_size_;

    discarded_byte_count_ += excess;

    buffer_.erase(
        buffer_.begin(),
        buffer_.begin() +
            static_cast<std::ptrdiff_t>(excess));
}

}  // namespace anbot