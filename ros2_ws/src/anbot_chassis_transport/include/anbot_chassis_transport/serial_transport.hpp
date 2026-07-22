#pragma once

#include <cstddef>
#include <cstdint>
#include <mutex>
#include <string>
#include <vector>

#include "anbot_chassis_transport/transport.hpp"

namespace anbot
{

struct SerialConfiguration
{
    std::string device;
    int baud_rate = 38400;

    bool hardware_flow_control = false;
    bool software_flow_control = false;
};

class SerialTransport final : public Transport
{
public:
    explicit SerialTransport(SerialConfiguration configuration);

    ~SerialTransport() override;

    SerialTransport(const SerialTransport&) = delete;
    SerialTransport& operator=(const SerialTransport&) = delete;

    bool open() override;

    void close() override;

    bool isOpen() const override;

    std::size_t write(
        const uint8_t* data,
        std::size_t size) override;

    std::vector<uint8_t> read(
        std::size_t maximum_size) override;

    const SerialConfiguration& configuration() const;

    int lastError() const;

private:
    bool configurePort(int file_descriptor);

    static bool convertBaudRate(
        int baud_rate,
        unsigned int& output_speed);

    void setLastError(int error_number);

    SerialConfiguration configuration_;

    mutable std::mutex mutex_;

    int file_descriptor_ = -1;
    int last_error_ = 0;
};

}  // namespace anbot