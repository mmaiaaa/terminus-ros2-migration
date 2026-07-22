#include "anbot_chassis_transport/mock_transport.hpp"

#include <algorithm>
#include <stdexcept>

namespace anbot
{

bool MockTransport::open()
{
    std::lock_guard<std::mutex> lock(mutex_);

    open_ = true;
    return true;
}

void MockTransport::close()
{
    std::lock_guard<std::mutex> lock(mutex_);

    open_ = false;
}

bool MockTransport::isOpen() const
{
    std::lock_guard<std::mutex> lock(mutex_);

    return open_;
}

std::size_t MockTransport::write(
    const uint8_t* data,
    const std::size_t size)
{
    if (data == nullptr && size != 0)
    {
        throw std::invalid_argument(
            "Null write pointer with non-zero size");
    }

    std::lock_guard<std::mutex> lock(mutex_);

    if (!open_)
    {
        return 0;
    }

    written_bytes_.insert(
        written_bytes_.end(),
        data,
        data + size);

    return size;
}

std::vector<uint8_t> MockTransport::read(
    const std::size_t maximum_size)
{
    std::lock_guard<std::mutex> lock(mutex_);

    if (!open_ || maximum_size == 0)
    {
        return {};
    }

    const std::size_t count =
        std::min(maximum_size, received_bytes_.size());

    std::vector<uint8_t> output;
    output.reserve(count);

    for (std::size_t index = 0; index < count; ++index)
    {
        output.push_back(received_bytes_.front());
        received_bytes_.pop_front();
    }

    return output;
}

void MockTransport::injectReceivedBytes(
    const uint8_t* data,
    const std::size_t size)
{
    if (data == nullptr && size != 0)
    {
        throw std::invalid_argument(
            "Null receive pointer with non-zero size");
    }

    std::lock_guard<std::mutex> lock(mutex_);

    received_bytes_.insert(
        received_bytes_.end(),
        data,
        data + size);
}

void MockTransport::injectReceivedBytes(
    const std::vector<uint8_t>& data)
{
    injectReceivedBytes(data.data(), data.size());
}

std::vector<uint8_t> MockTransport::takeWrittenBytes()
{
    std::lock_guard<std::mutex> lock(mutex_);

    std::vector<uint8_t> output;
    output.swap(written_bytes_);

    return output;
}

std::vector<uint8_t> MockTransport::peekWrittenBytes() const
{
    std::lock_guard<std::mutex> lock(mutex_);

    return written_bytes_;
}

std::size_t MockTransport::pendingReceivedByteCount() const
{
    std::lock_guard<std::mutex> lock(mutex_);

    return received_bytes_.size();
}

std::size_t MockTransport::writtenByteCount() const
{
    std::lock_guard<std::mutex> lock(mutex_);

    return written_bytes_.size();
}

void MockTransport::clear()
{
    std::lock_guard<std::mutex> lock(mutex_);

    received_bytes_.clear();
    written_bytes_.clear();
}

}  // namespace anbot