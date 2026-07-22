#include <gtest/gtest.h>

#include <cstdint>
#include <memory>
#include <stdexcept>
#include <vector>

#include "anbot_chassis_driver/chassis_driver.hpp"
#include "anbot_chassis_transport/mock_transport.hpp"

namespace
{

std::vector<uint8_t> makeFeedbackFrame(
    const int16_t left,
    const int16_t right,
    const uint8_t status = 0)
{
    const auto left_raw = static_cast<uint16_t>(left);
    const auto right_raw = static_cast<uint16_t>(right);

    std::vector<uint8_t> frame{
        0x05,
        static_cast<uint8_t>((left_raw >> 8U) & 0xFFU),
        static_cast<uint8_t>(left_raw & 0xFFU),
        static_cast<uint8_t>((right_raw >> 8U) & 0xFFU),
        static_cast<uint8_t>(right_raw & 0xFFU),
        0x00,
        0x00,
        0x00,
        status,
        0x00
    };

    uint32_t checksum = 0;

    for (std::size_t index = 0; index < 9; ++index)
    {
        checksum += frame[index];
    }

    frame[9] = static_cast<uint8_t>(checksum & 0xFFU);

    return frame;
}

TEST(ChassisDriverTest, RejectsNullTransport)
{
    EXPECT_THROW(
        anbot::ChassisDriver driver(nullptr),
        std::invalid_argument);
}

TEST(ChassisDriverTest, OpensAndClosesTransport)
{
    auto transport =
        std::make_shared<anbot::MockTransport>();

    anbot::ChassisDriver driver(transport);

    EXPECT_FALSE(driver.isOpen());

    EXPECT_TRUE(driver.open());
    EXPECT_TRUE(driver.isOpen());

    driver.close();

    EXPECT_FALSE(driver.isOpen());
}

TEST(ChassisDriverTest, CannotSendWhileClosed)
{
    auto transport =
        std::make_shared<anbot::MockTransport>();

    anbot::ChassisDriver driver(transport);

    const anbot::WheelCommand command{
        10,
        20,
        false
    };

    EXPECT_FALSE(driver.sendWheelCommand(command));
    EXPECT_EQ(driver.failedWriteCount(), 1U);
    EXPECT_EQ(driver.transmittedFrameCount(), 0U);
}

TEST(ChassisDriverTest, EncodesAndWritesCommand)
{
    auto transport =
        std::make_shared<anbot::MockTransport>();

    anbot::ChassisDriver driver(transport);

    ASSERT_TRUE(driver.open());

    const anbot::WheelCommand command{
        0x1234,
        0x0567,
        false
    };

    ASSERT_TRUE(driver.sendWheelCommand(command));

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

    EXPECT_EQ(transport->takeWrittenBytes(), expected);
    EXPECT_EQ(driver.transmittedFrameCount(), 1U);
    EXPECT_EQ(driver.failedWriteCount(), 0U);
}

TEST(ChassisDriverTest, PreservesRepeatedStopBehavior)
{
    auto transport =
        std::make_shared<anbot::MockTransport>();

    anbot::ChassisDriver driver(transport);

    ASSERT_TRUE(driver.open());

    const anbot::WheelCommand stop{
        0,
        0,
        false
    };

    ASSERT_TRUE(driver.sendWheelCommand(stop));
    ASSERT_TRUE(driver.sendWheelCommand(stop));

    const std::vector<uint8_t> expected{
        0x04,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x04,

        0x04,
        0x00,
        0x00,
        0x00,
        0x00,
        0xAA,
        0x00,
        0xAE
    };

    EXPECT_EQ(transport->takeWrittenBytes(), expected);
    EXPECT_EQ(driver.transmittedFrameCount(), 2U);
}

TEST(ChassisDriverTest, ReceivesOneFeedbackFrame)
{
    auto transport =
        std::make_shared<anbot::MockTransport>();

    anbot::ChassisDriver driver(transport);

    ASSERT_TRUE(driver.open());

    transport->injectReceivedBytes(
        makeFeedbackFrame(100, -200, 4));

    const auto messages = driver.pollFeedback();

    ASSERT_EQ(messages.size(), 1U);

    EXPECT_EQ(messages[0].left_value, 100);
    EXPECT_EQ(messages[0].right_value, -200);
    EXPECT_EQ(messages[0].status, 4);

    EXPECT_EQ(driver.receivedFrameCount(), 1U);

    const auto latest = driver.latestFeedback();

    ASSERT_TRUE(latest.has_value());
    EXPECT_EQ(latest->left_value, 100);
    EXPECT_EQ(latest->right_value, -200);
}

TEST(ChassisDriverTest, HandlesFragmentedFeedback)
{
    auto transport =
        std::make_shared<anbot::MockTransport>();

    anbot::ChassisDriver driver(transport);

    ASSERT_TRUE(driver.open());

    const auto frame = makeFeedbackFrame(12, 34);

    const std::vector<uint8_t> first_part(
        frame.begin(),
        frame.begin() + 4);

    const std::vector<uint8_t> second_part(
        frame.begin() + 4,
        frame.end());

    transport->injectReceivedBytes(first_part);

    EXPECT_TRUE(driver.pollFeedback().empty());
    EXPECT_EQ(driver.receivedFrameCount(), 0U);

    transport->injectReceivedBytes(second_part);

    const auto messages = driver.pollFeedback();

    ASSERT_EQ(messages.size(), 1U);
    EXPECT_EQ(messages[0].left_value, 12);
    EXPECT_EQ(messages[0].right_value, 34);
    EXPECT_EQ(driver.receivedFrameCount(), 1U);
}

TEST(ChassisDriverTest, ReceivesSeveralFrames)
{
    auto transport =
        std::make_shared<anbot::MockTransport>();

    anbot::ChassisDriver driver(transport);

    ASSERT_TRUE(driver.open());

    const auto first = makeFeedbackFrame(1, 2);
    const auto second = makeFeedbackFrame(3, 4);

    std::vector<uint8_t> input;

    input.insert(
        input.end(),
        first.begin(),
        first.end());

    input.insert(
        input.end(),
        second.begin(),
        second.end());

    transport->injectReceivedBytes(input);

    const auto messages = driver.pollFeedback(100);

    ASSERT_EQ(messages.size(), 2U);

    EXPECT_EQ(messages[0].left_value, 1);
    EXPECT_EQ(messages[1].left_value, 3);

    EXPECT_EQ(driver.receivedFrameCount(), 2U);

    const auto latest = driver.latestFeedback();

    ASSERT_TRUE(latest.has_value());
    EXPECT_EQ(latest->left_value, 3);
    EXPECT_EQ(latest->right_value, 4);
}

TEST(ChassisDriverTest, PollWhileClosedReturnsNothing)
{
    auto transport =
        std::make_shared<anbot::MockTransport>();

    anbot::ChassisDriver driver(transport);

    transport->injectReceivedBytes(
        makeFeedbackFrame(1, 2));

    EXPECT_TRUE(driver.pollFeedback().empty());
    EXPECT_EQ(driver.receivedFrameCount(), 0U);
}

TEST(ChassisDriverTest, ZeroSizePollReturnsNothing)
{
    auto transport =
        std::make_shared<anbot::MockTransport>();

    anbot::ChassisDriver driver(transport);

    ASSERT_TRUE(driver.open());

    transport->injectReceivedBytes(
        makeFeedbackFrame(1, 2));

    EXPECT_TRUE(driver.pollFeedback(0).empty());
    EXPECT_EQ(driver.receivedFrameCount(), 0U);
}

TEST(ChassisDriverTest, ResetStatisticsClearsCounters)
{
    auto transport =
        std::make_shared<anbot::MockTransport>();

    anbot::ChassisDriver driver(transport);

    const anbot::WheelCommand command{
        1,
        2,
        false
    };

    EXPECT_FALSE(driver.sendWheelCommand(command));

    ASSERT_TRUE(driver.open());
    ASSERT_TRUE(driver.sendWheelCommand(command));

    transport->injectReceivedBytes(
        makeFeedbackFrame(3, 4));

    ASSERT_EQ(driver.pollFeedback().size(), 1U);

    EXPECT_EQ(driver.failedWriteCount(), 1U);
    EXPECT_EQ(driver.transmittedFrameCount(), 1U);
    EXPECT_EQ(driver.receivedFrameCount(), 1U);

    driver.resetStatistics();

    EXPECT_EQ(driver.failedWriteCount(), 0U);
    EXPECT_EQ(driver.transmittedFrameCount(), 0U);
    EXPECT_EQ(driver.receivedFrameCount(), 0U);
}

}  // namespace