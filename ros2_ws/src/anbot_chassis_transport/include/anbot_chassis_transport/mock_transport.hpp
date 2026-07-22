#pragma once

#include <cstddef>
#include <cstdint>
#include <deque>
#include <mutex>
#include <vector>

#include "anbot_chassis_transport/transport.hpp"

namespace anbot
{

class MockTransport final : public Transport
{
public:
    MockTransport() = default;

    bool open() override;

    void close() override;

    bool isOpen() const override;

    std::size_t write(
        const uint8_t* data,
        std::size_t size) override;

    std::vector<uint8_t> read(
        std::size_t maximum_size) override;

    void injectReceivedBytes(
        const uint8_t* data,
        std::size_t size);

    void injectReceivedBytes(
        const std::vector<uint8_t>& data);

    std::vector<uint8_t> takeWrittenBytes();

    std::vector<uint8_t> peekWrittenBytes() const;

    std::size_t pendingReceivedByteCount() const;

    std::size_t writtenByteCount() const;

    void clear();

private:
    mutable std::mutex mutex_;

    bool open_ = false;

    std::deque<uint8_t> received_bytes_;
    std::vector<uint8_t> written_bytes_;
};

}  // namespace anbot