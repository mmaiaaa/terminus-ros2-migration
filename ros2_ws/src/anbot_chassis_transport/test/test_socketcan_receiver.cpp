#include <gtest/gtest.h>

#include <cerrno>
#include <cstring>
#include <optional>
#include <stdexcept>
#include <string>

#include <linux/can.h>
#include <linux/can/raw.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <unistd.h>

#include "anbot_chassis_transport/socketcan_receiver.hpp"

namespace
{

class CanSocket
{
public:
    explicit CanSocket(const std::string& interface_name)
    {
        descriptor_ = ::socket(
            PF_CAN,
            SOCK_RAW,
            CAN_RAW);

        if (descriptor_ < 0)
        {
            return;
        }

        ifreq interface_request{};

        std::strncpy(
            interface_request.ifr_name,
            interface_name.c_str(),
            IFNAMSIZ - 1);

        interface_request.ifr_name[IFNAMSIZ - 1] = '\0';

        if (
            ::ioctl(
                descriptor_,
                SIOCGIFINDEX,
                &interface_request) < 0)
        {
            ::close(descriptor_);
            descriptor_ = -1;
            return;
        }

        sockaddr_can address{};
        address.can_family = AF_CAN;
        address.can_ifindex = interface_request.ifr_ifindex;

        if (
            ::bind(
                descriptor_,
                reinterpret_cast<const sockaddr*>(&address),
                sizeof(address)) < 0)
        {
            ::close(descriptor_);
            descriptor_ = -1;
        }
    }

    ~CanSocket()
    {
        if (descriptor_ >= 0)
        {
            ::close(descriptor_);
        }
    }

    CanSocket(const CanSocket&) = delete;
    CanSocket& operator=(const CanSocket&) = delete;

    bool isOpen() const
    {
        return descriptor_ >= 0;
    }

    bool send(const can_frame& frame)
    {
        if (descriptor_ < 0)
        {
            return false;
        }

        const ssize_t result = ::write(
            descriptor_,
            &frame,
            sizeof(frame));

        return result == static_cast<ssize_t>(sizeof(frame));
    }

private:
    int descriptor_ = -1;
};

std::optional<anbot::CanFrame> waitForFrame(
    anbot::SocketCanReceiver& receiver)
{
    for (int attempt = 0; attempt < 200; ++attempt)
    {
        const auto frame = receiver.receive();

        if (frame.has_value())
        {
            return frame;
        }

        ::usleep(1000);
    }

    return std::nullopt;
}

TEST(SocketCanReceiverTest, RejectsEmptyInterfaceName)
{
    anbot::SocketCanConfiguration configuration;
    configuration.interface_name = "";

    EXPECT_THROW(
        anbot::SocketCanReceiver receiver(configuration),
        std::invalid_argument);
}

TEST(SocketCanReceiverTest, RejectsOverlongInterfaceName)
{
    anbot::SocketCanConfiguration configuration;
    configuration.interface_name =
        std::string(IFNAMSIZ, 'a');

    EXPECT_THROW(
        anbot::SocketCanReceiver receiver(configuration),
        std::invalid_argument);
}

TEST(SocketCanReceiverTest, StartsClosed)
{
    anbot::SocketCanConfiguration configuration;
    configuration.interface_name = "can0";

    anbot::SocketCanReceiver receiver(configuration);

    EXPECT_FALSE(receiver.isOpen());
}

TEST(SocketCanReceiverTest, ClosedReceiverReturnsNoFrame)
{
    anbot::SocketCanConfiguration configuration;
    configuration.interface_name = "can0";

    anbot::SocketCanReceiver receiver(configuration);

    EXPECT_FALSE(receiver.receive().has_value());
}

TEST(SocketCanReceiverTest, FailsForMissingInterface)
{
    anbot::SocketCanConfiguration configuration;
    configuration.interface_name =
        "anbotcanmissing";

    anbot::SocketCanReceiver receiver(configuration);

    EXPECT_FALSE(receiver.open());
    EXPECT_FALSE(receiver.isOpen());
    EXPECT_NE(receiver.lastError(), 0);
}

TEST(SocketCanReceiverTest, OpensAndClosesVirtualCanInterface)
{
    anbot::SocketCanConfiguration configuration;
    configuration.interface_name = "vcan0";

    anbot::SocketCanReceiver receiver(configuration);

    if (!receiver.open())
    {
        GTEST_SKIP()
            << "vcan0 is not available; last error: "
            << receiver.lastError();
    }

    EXPECT_TRUE(receiver.isOpen());

    EXPECT_TRUE(receiver.open());
    EXPECT_TRUE(receiver.isOpen());

    receiver.close();

    EXPECT_FALSE(receiver.isOpen());
}

TEST(SocketCanReceiverTest, ReceivesClassicalCanFrame)
{
    anbot::SocketCanConfiguration configuration;
    configuration.interface_name = "vcan0";

    anbot::SocketCanReceiver receiver(configuration);

    if (!receiver.open())
    {
        GTEST_SKIP()
            << "vcan0 is not available; last error: "
            << receiver.lastError();
    }

    CanSocket sender("vcan0");

    if (!sender.isOpen())
    {
        GTEST_SKIP()
            << "Unable to create vcan0 sender socket";
    }

    can_frame transmitted{};
    transmitted.can_id = 0x184U;
    transmitted.can_dlc = 8U;

    transmitted.data[0] = 0x52U;
    transmitted.data[1] = 0xFFU;
    transmitted.data[2] = 0xB0U;
    transmitted.data[3] = 0x00U;
    transmitted.data[4] = 0x11U;
    transmitted.data[5] = 0x22U;
    transmitted.data[6] = 0x33U;
    transmitted.data[7] = 0x44U;

    ASSERT_TRUE(sender.send(transmitted));

    const auto received = waitForFrame(receiver);

    ASSERT_TRUE(received.has_value());

    EXPECT_EQ(received->id, 0x184U);
    EXPECT_EQ(received->size, 8U);

    EXPECT_EQ(received->data[0], 0x52U);
    EXPECT_EQ(received->data[1], 0xFFU);
    EXPECT_EQ(received->data[2], 0xB0U);
    EXPECT_EQ(received->data[3], 0x00U);
    EXPECT_EQ(received->data[4], 0x11U);
    EXPECT_EQ(received->data[5], 0x22U);
    EXPECT_EQ(received->data[6], 0x33U);
    EXPECT_EQ(received->data[7], 0x44U);
}

TEST(SocketCanReceiverTest, ReceivesExtendedCanIdentifier)
{
    anbot::SocketCanConfiguration configuration;
    configuration.interface_name = "vcan0";

    anbot::SocketCanReceiver receiver(configuration);

    if (!receiver.open())
    {
        GTEST_SKIP()
            << "vcan0 is not available; last error: "
            << receiver.lastError();
    }

    CanSocket sender("vcan0");

    if (!sender.isOpen())
    {
        GTEST_SKIP()
            << "Unable to create vcan0 sender socket";
    }

    can_frame transmitted{};
    transmitted.can_id =
        CAN_EFF_FLAG | 0x01ABCDE0U;

    transmitted.can_dlc = 2U;
    transmitted.data[0] = 0x12U;
    transmitted.data[1] = 0x34U;

    ASSERT_TRUE(sender.send(transmitted));

    const auto received = waitForFrame(receiver);

    ASSERT_TRUE(received.has_value());

    EXPECT_EQ(received->id, 0x01ABCDE0U);
    EXPECT_EQ(received->size, 2U);
    EXPECT_EQ(received->data[0], 0x12U);
    EXPECT_EQ(received->data[1], 0x34U);
}

}  // namespace
