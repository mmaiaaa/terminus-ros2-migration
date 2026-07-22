#include <gtest/gtest.h>

#include <cstdint>
#include <stdexcept>
#include <vector>

#include "anbot_chassis_transport/mock_transport.hpp"

namespace
{

TEST(MockTransportTest, StartsClosed)
{
    anbot::MockTransport transport;

    EXPECT_FALSE(transport.isOpen());
}

TEST(MockTransportTest, OpensAndCloses)
{
    anbot::MockTransport transport;

    EXPECT_TRUE(transport.open());
    EXPECT_TRUE(transport.isOpen());

    transport.close();

    EXPECT_FALSE(transport.isOpen());
}

TEST(MockTransportTest, RejectsWritesWhileClosed)
{
    anbot::MockTransport transport;

    const std::vector<uint8_t> bytes{
        0x01,
        0x02,
        0x03
    };

    EXPECT_EQ(
        transport.write(bytes.data(), bytes.size()),
        0U);

    EXPECT_EQ(transport.writtenByteCount(), 0U);
}

TEST(MockTransportTest, RecordsWrittenBytes)
{
    anbot::MockTransport transport;

    ASSERT_TRUE(transport.open());

    const std::vector<uint8_t> bytes{
        0x04,
        0x12,
        0x34,
        0x56
    };

    EXPECT_EQ(
        transport.write(bytes.data(), bytes.size()),
        bytes.size());

    EXPECT_EQ(transport.peekWrittenBytes(), bytes);
    EXPECT_EQ(transport.writtenByteCount(), bytes.size());
}

TEST(MockTransportTest, TakeWrittenBytesClearsOutput)
{
    anbot::MockTransport transport;

    ASSERT_TRUE(transport.open());

    const std::vector<uint8_t> bytes{
        0x10,
        0x20,
        0x30
    };

    transport.write(bytes.data(), bytes.size());

    EXPECT_EQ(transport.takeWrittenBytes(), bytes);
    EXPECT_EQ(transport.writtenByteCount(), 0U);
}

TEST(MockTransportTest, ReadsInjectedBytes)
{
    anbot::MockTransport transport;

    ASSERT_TRUE(transport.open());

    const std::vector<uint8_t> bytes{
        0x05,
        0x01,
        0x02,
        0x03
    };

    transport.injectReceivedBytes(bytes);

    EXPECT_EQ(
        transport.pendingReceivedByteCount(),
        bytes.size());

    EXPECT_EQ(transport.read(10), bytes);
    EXPECT_EQ(transport.pendingReceivedByteCount(), 0U);
}

TEST(MockTransportTest, SupportsFragmentedReads)
{
    anbot::MockTransport transport;

    ASSERT_TRUE(transport.open());

    const std::vector<uint8_t> bytes{
        0x01,
        0x02,
        0x03,
        0x04,
        0x05
    };

    transport.injectReceivedBytes(bytes);

    const std::vector<uint8_t> first_expected{
        0x01,
        0x02
    };

    const std::vector<uint8_t> second_expected{
        0x03,
        0x04,
        0x05
    };

    EXPECT_EQ(transport.read(2), first_expected);
    EXPECT_EQ(transport.read(10), second_expected);
}

TEST(MockTransportTest, ReadReturnsEmptyWhileClosed)
{
    anbot::MockTransport transport;

    transport.injectReceivedBytes(
        std::vector<uint8_t>{0x01, 0x02});

    EXPECT_TRUE(transport.read(10).empty());

    EXPECT_EQ(
        transport.pendingReceivedByteCount(),
        2U);
}

TEST(MockTransportTest, ZeroLengthReadReturnsEmpty)
{
    anbot::MockTransport transport;

    ASSERT_TRUE(transport.open());

    transport.injectReceivedBytes(
        std::vector<uint8_t>{0x01});

    EXPECT_TRUE(transport.read(0).empty());

    EXPECT_EQ(
        transport.pendingReceivedByteCount(),
        1U);
}

TEST(MockTransportTest, ClearRemovesQueuedData)
{
    anbot::MockTransport transport;

    ASSERT_TRUE(transport.open());

    const std::vector<uint8_t> bytes{
        0x01,
        0x02,
        0x03
    };

    transport.injectReceivedBytes(bytes);
    transport.write(bytes.data(), bytes.size());

    transport.clear();

    EXPECT_EQ(transport.pendingReceivedByteCount(), 0U);
    EXPECT_EQ(transport.writtenByteCount(), 0U);
}

TEST(MockTransportTest, RejectsNullWriteWithNonzeroSize)
{
    anbot::MockTransport transport;

    ASSERT_TRUE(transport.open());

    EXPECT_THROW(
        transport.write(nullptr, 1),
        std::invalid_argument);
}

TEST(MockTransportTest, RejectsNullInjectionWithNonzeroSize)
{
    anbot::MockTransport transport;

    EXPECT_THROW(
        transport.injectReceivedBytes(nullptr, 1),
        std::invalid_argument);
}

}  // namespace