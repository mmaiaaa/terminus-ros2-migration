#include <gtest/gtest.h>

#include <cstdint>
#include <vector>

#include "anbot_chassis_protocol/wheel_stream_parser.hpp"

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

TEST(WheelStreamParserTest, ParsesOneCompleteFrame)
{
    anbot::WheelStreamParser parser;

    const auto frame = makeFeedbackFrame(100, -200, 3);
    const auto messages = parser.pushBytes(frame);

    ASSERT_EQ(messages.size(), 1U);

    EXPECT_EQ(messages[0].left_value, 100);
    EXPECT_EQ(messages[0].right_value, -200);
    EXPECT_EQ(messages[0].status, 3);
    EXPECT_TRUE(messages[0].valid);

    EXPECT_EQ(parser.bufferedByteCount(), 0U);
}

TEST(WheelStreamParserTest, BuffersPartialFrame)
{
    anbot::WheelStreamParser parser;

    const auto frame = makeFeedbackFrame(10, 20);

    const std::vector<uint8_t> first_part(
        frame.begin(),
        frame.begin() + 4);

    const std::vector<uint8_t> second_part(
        frame.begin() + 4,
        frame.end());

    const auto first_messages = parser.pushBytes(first_part);

    EXPECT_TRUE(first_messages.empty());
    EXPECT_EQ(parser.bufferedByteCount(), 4U);

    const auto second_messages = parser.pushBytes(second_part);

    ASSERT_EQ(second_messages.size(), 1U);
    EXPECT_EQ(second_messages[0].left_value, 10);
    EXPECT_EQ(second_messages[0].right_value, 20);
    EXPECT_EQ(parser.bufferedByteCount(), 0U);
}

TEST(WheelStreamParserTest, ParsesSeveralFramesInOneChunk)
{
    anbot::WheelStreamParser parser;

    const auto first = makeFeedbackFrame(1, 2);
    const auto second = makeFeedbackFrame(3, 4);
    const auto third = makeFeedbackFrame(-5, -6);

    std::vector<uint8_t> input;

    input.insert(input.end(), first.begin(), first.end());
    input.insert(input.end(), second.begin(), second.end());
    input.insert(input.end(), third.begin(), third.end());

    const auto messages = parser.pushBytes(input);

    ASSERT_EQ(messages.size(), 3U);

    EXPECT_EQ(messages[0].left_value, 1);
    EXPECT_EQ(messages[0].right_value, 2);

    EXPECT_EQ(messages[1].left_value, 3);
    EXPECT_EQ(messages[1].right_value, 4);

    EXPECT_EQ(messages[2].left_value, -5);
    EXPECT_EQ(messages[2].right_value, -6);
}

TEST(WheelStreamParserTest, DiscardsNoiseBeforeValidFrame)
{
    anbot::WheelStreamParser parser;

    const auto frame = makeFeedbackFrame(50, 60);

    std::vector<uint8_t> input{
        0x99,
        0x44,
        0x12,
        0xFE
    };

    input.insert(input.end(), frame.begin(), frame.end());

    const auto messages = parser.pushBytes(input);

    ASSERT_EQ(messages.size(), 1U);
    EXPECT_EQ(messages[0].left_value, 50);
    EXPECT_EQ(messages[0].right_value, 60);

    EXPECT_EQ(parser.discardedByteCount(), 4U);
}

TEST(WheelStreamParserTest, RecoversAfterInvalidFrame)
{
    anbot::WheelStreamParser parser;

    auto invalid_frame = makeFeedbackFrame(10, 11);
    invalid_frame[9] ^= 0xFFU;

    const auto valid_frame = makeFeedbackFrame(20, 21);

    std::vector<uint8_t> input;

    input.insert(
        input.end(),
        invalid_frame.begin(),
        invalid_frame.end());

    input.insert(
        input.end(),
        valid_frame.begin(),
        valid_frame.end());

    const auto messages = parser.pushBytes(input);

    ASSERT_EQ(messages.size(), 1U);
    EXPECT_EQ(messages[0].left_value, 20);
    EXPECT_EQ(messages[0].right_value, 21);

    EXPECT_GE(parser.invalidFrameCount(), 1U);
    EXPECT_GE(parser.discardedByteCount(), 1U);
}

TEST(WheelStreamParserTest, PreservesPossiblePartialHeader)
{
    anbot::WheelStreamParser parser;

    const std::vector<uint8_t> first_input{
        0x80,
        0x81,
        0x05,
        0x00,
        0x64
    };

    const auto first_messages = parser.pushBytes(first_input);

    EXPECT_TRUE(first_messages.empty());
    EXPECT_EQ(parser.bufferedByteCount(), 3U);
    EXPECT_EQ(parser.discardedByteCount(), 2U);

    const auto complete_frame = makeFeedbackFrame(100, 200);

    const std::vector<uint8_t> remaining_bytes(
        complete_frame.begin() + 3,
        complete_frame.end());

    const auto second_messages =
        parser.pushBytes(remaining_bytes);

    ASSERT_EQ(second_messages.size(), 1U);
    EXPECT_EQ(second_messages[0].left_value, 100);
    EXPECT_EQ(second_messages[0].right_value, 200);
}

TEST(WheelStreamParserTest, ResetClearsStateAndStatistics)
{
    anbot::WheelStreamParser parser;

    parser.pushBytes(
        std::vector<uint8_t>{0x99, 0x98, 0x05});

    EXPECT_NE(parser.bufferedByteCount(), 0U);
    EXPECT_NE(parser.discardedByteCount(), 0U);

    parser.reset();

    EXPECT_EQ(parser.bufferedByteCount(), 0U);
    EXPECT_EQ(parser.discardedByteCount(), 0U);
    EXPECT_EQ(parser.invalidFrameCount(), 0U);
}

TEST(WheelStreamParserTest, RejectsInvalidMaximumBufferSize)
{
    EXPECT_THROW(
        anbot::WheelStreamParser parser(5),
        std::invalid_argument);
}

TEST(WheelStreamParserTest, AcceptsEmptyInput)
{
    anbot::WheelStreamParser parser;

    const std::vector<uint8_t> empty;
    const auto messages = parser.pushBytes(empty);

    EXPECT_TRUE(messages.empty());
    EXPECT_EQ(parser.bufferedByteCount(), 0U);
}

}  // namespace