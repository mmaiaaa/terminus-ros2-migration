#pragma once

#include <cstddef>
#include <cstdint>
#include <vector>

namespace anbot
{

class Transport
{
public:
    virtual ~Transport() = default;

    virtual bool open() = 0;

    virtual void close() = 0;

    virtual bool isOpen() const = 0;

    virtual std::size_t write(
        const uint8_t* data,
        std::size_t size) = 0;

    virtual std::vector<uint8_t> read(
        std::size_t maximum_size) = 0;
};

}  // namespace anbot