#include <gtest/gtest.h>

#include <cerrno>
#include <chrono>
#include <cstdint>
#include <fcntl.h>
#include <memory>
#include <pty.h>
#include <stdexcept>
#include <string>
#include <thread>
#include <unistd.h>
#include <vector>

#include "anbot_chassis_transport/serial_transport.hpp"

namespace
{

class PseudoTerminal
{
public:
    PseudoTerminal()
    {
        char slave_name[256]{};

        if (::openpty(
                &master_descriptor_,
                &slave_descriptor_,
                slave_name,
                nullptr,
                nullptr) != 0)
        {
            throw std::runtime_error(
                "Failed to create pseudo-terminal");
        }

        slave_path_ = slave_name;

        /*
         * SerialTransport must open the slave itself.
         * Keeping this original slave descriptor open is unnecessary.
         */
        ::close(slave_descriptor_);
        slave_descriptor_ = -1;
    }

    ~PseudoTerminal()
    {
        if (master_descriptor_ >= 0)
        {
            ::close(master_descriptor_);
        }

        if (slave_descriptor_ >= 0)
        {
            ::close(slave_descriptor_);
        }
    }

    PseudoTerminal(const PseudoTerminal&) = delete;
    PseudoTerminal& operator=(const PseudoTerminal&) = delete;

    const std::string& slavePath() const
    {
        return slave_path_;
    }

    int masterDescriptor() const
    {
        return master_descriptor_;
    }

private:
    int master_descriptor_ = -1;
    int slave_descriptor_ = -1;

    std::string slave_path_;
};

std::vector<uint8_t> waitForTransportRead(
    anbot::SerialTransport& transport,
    const std::size_t expected_size)
{
    std::vector<uint8_t> result;

    for (int attempt = 0; attempt < 100; ++attempt)
    {
        const auto chunk = transport.read(256);

        result.insert(
            result.end(),
            chunk.begin(),
            chunk.end());

        if (result.size() >= expected_size)
        {
            break;
        }

        std::this_thread::sleep_for(
            std::chrono::milliseconds(1));
    }

    return result;
}

std::vector<uint8_t> waitForMasterRead(
    const int master_descriptor,
    const std::size_t expected_size)
{
    std::vector<uint8_t> result;
    result.reserve(expected_size);

    for (int attempt = 0; attempt < 100; ++attempt)
    {
        uint8_t buffer[256]{};

        const ssize_t count = ::read(
            master_descriptor,
            buffer,
            sizeof(buffer));

        if (count > 0)
        {
            result.insert(
                result.end(),
                buffer,
                buffer + count);

            if (result.size() >= expected_size)
            {
                break;
            }
        }
        else if (
            count < 0 &&
            errno != EAGAIN &&
            errno != EWOULDBLOCK &&
            errno != EINTR)
        {
            break;
        }

        std::this_thread::sleep_for(
            std::chrono::milliseconds(1));
    }

    return result;
}

TEST(SerialTransportTest, RejectsEmptyDevicePath)
{
    anbot::SerialConfiguration configuration;
    configuration.device = "";

    EXPECT_THROW(
        anbot::SerialTransport transport(configuration),
        std::invalid_argument);
}

TEST(SerialTransportTest, StartsClosed)
{
    PseudoTerminal terminal;

    anbot::SerialConfiguration configuration;
    configuration.device = terminal.slavePath();

    anbot::SerialTransport transport(configuration);

    EXPECT_FALSE(transport.isOpen());
}

TEST(SerialTransportTest, OpensAndClosesPseudoTerminal)
{
    PseudoTerminal terminal;

    anbot::SerialConfiguration configuration;
    configuration.device = terminal.slavePath();
    configuration.baud_rate = 38400;

    anbot::SerialTransport transport(configuration);

    ASSERT_TRUE(transport.open());
    EXPECT_TRUE(transport.isOpen());

    transport.close();

    EXPECT_FALSE(transport.isOpen());
}

TEST(SerialTransportTest, OpeningTwiceIsSafe)
{
    PseudoTerminal terminal;

    anbot::SerialConfiguration configuration;
    configuration.device = terminal.slavePath();

    anbot::SerialTransport transport(configuration);

    ASSERT_TRUE(transport.open());
    EXPECT_TRUE(transport.open());
    EXPECT_TRUE(transport.isOpen());
}

TEST(SerialTransportTest, FailsForMissingDevice)
{
    anbot::SerialConfiguration configuration;
    configuration.device =
        "/dev/anbot-device-that-does-not-exist";

    anbot::SerialTransport transport(configuration);

    EXPECT_FALSE(transport.open());
    EXPECT_FALSE(transport.isOpen());
    EXPECT_NE(transport.lastError(), 0);
}

TEST(SerialTransportTest, RejectsUnsupportedBaudRate)
{
    PseudoTerminal terminal;

    anbot::SerialConfiguration configuration;
    configuration.device = terminal.slavePath();
    configuration.baud_rate = 12345;

    anbot::SerialTransport transport(configuration);

    EXPECT_FALSE(transport.open());
    EXPECT_FALSE(transport.isOpen());
    EXPECT_EQ(transport.lastError(), EINVAL);
}

TEST(SerialTransportTest, ReadsBytesFromPseudoTerminalMaster)
{
    PseudoTerminal terminal;

    anbot::SerialConfiguration configuration;
    configuration.device = terminal.slavePath();

    anbot::SerialTransport transport(configuration);

    ASSERT_TRUE(transport.open());

    const std::vector<uint8_t> expected{
        0x05,
        0x01,
        0x02,
        0x03,
        0x04
    };

    ASSERT_EQ(
        ::write(
            terminal.masterDescriptor(),
            expected.data(),
            expected.size()),
        static_cast<ssize_t>(expected.size()));

    const auto received =
        waitForTransportRead(transport, expected.size());

    EXPECT_EQ(received, expected);
}

TEST(SerialTransportTest, WritesBytesToPseudoTerminalMaster)
{
    PseudoTerminal terminal;

    const int flags = ::fcntl(
        terminal.masterDescriptor(),
        F_GETFL,
        0);

    ASSERT_GE(flags, 0);

    ASSERT_EQ(
        ::fcntl(
            terminal.masterDescriptor(),
            F_SETFL,
            flags | O_NONBLOCK),
        0);

    anbot::SerialConfiguration configuration;
    configuration.device = terminal.slavePath();

    anbot::SerialTransport transport(configuration);

    ASSERT_TRUE(transport.open());

    const std::vector<uint8_t> expected{
        0x04,
        0x12,
        0x34,
        0x05,
        0x67,
        0x00,
        0x00,
        0xB6
    };

    ASSERT_EQ(
        transport.write(
            expected.data(),
            expected.size()),
        expected.size());

    const auto received = waitForMasterRead(
        terminal.masterDescriptor(),
        expected.size());

    EXPECT_EQ(received, expected);
}

TEST(SerialTransportTest, ClosedTransportDoesNotReadOrWrite)
{
    PseudoTerminal terminal;

    anbot::SerialConfiguration configuration;
    configuration.device = terminal.slavePath();

    anbot::SerialTransport transport(configuration);

    const std::vector<uint8_t> bytes{
        0x01,
        0x02
    };

    EXPECT_TRUE(transport.read(10).empty());

    EXPECT_EQ(
        transport.write(bytes.data(), bytes.size()),
        0U);
}

TEST(SerialTransportTest, EmptyOperationsAreSafe)
{
    PseudoTerminal terminal;

    anbot::SerialConfiguration configuration;
    configuration.device = terminal.slavePath();

    anbot::SerialTransport transport(configuration);

    ASSERT_TRUE(transport.open());

    EXPECT_TRUE(transport.read(0).empty());
    EXPECT_EQ(transport.write(nullptr, 0), 0U);
}

TEST(SerialTransportTest, RejectsNullWriteWithNonzeroSize)
{
    PseudoTerminal terminal;

    anbot::SerialConfiguration configuration;
    configuration.device = terminal.slavePath();

    anbot::SerialTransport transport(configuration);

    ASSERT_TRUE(transport.open());

    EXPECT_THROW(
        transport.write(nullptr, 1),
        std::invalid_argument);
}

TEST(SerialTransportTest, ExposesConfiguration)
{
    PseudoTerminal terminal;

    anbot::SerialConfiguration configuration;
    configuration.device = terminal.slavePath();
    configuration.baud_rate = 38400;
    configuration.hardware_flow_control = false;
    configuration.software_flow_control = false;

    anbot::SerialTransport transport(configuration);

    EXPECT_EQ(
        transport.configuration().device,
        terminal.slavePath());

    EXPECT_EQ(
        transport.configuration().baud_rate,
        38400);

    EXPECT_FALSE(
        transport.configuration().hardware_flow_control);

    EXPECT_FALSE(
        transport.configuration().software_flow_control);
}

}  // namespace