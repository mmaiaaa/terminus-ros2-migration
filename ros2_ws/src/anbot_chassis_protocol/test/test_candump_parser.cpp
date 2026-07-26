#include <gtest/gtest.h>

#include <cstdint>
#include <string>

#include "anbot_chassis_protocol/candump_parser.hpp"

namespace
{

TEST(CandumpParserTest, ParsesEightByteCanFrame)
{
    anbot::CandumpParser parser;

    const auto record = parser.parseLine(
        "(1455221182.888050) can0 183#42B63D3EBA544F3F");

    ASSERT_TRUE(record.has_value());

    EXPECT_DOUBLE_EQ(record->timestamp, 1455221182.888050);
    EXPECT_EQ(record->interface_name, "can0");
    EXPECT_EQ(record->frame.id, 0x183U);
    EXPECT_EQ(record->frame.size, 8U);

    EXPECT_EQ(record->frame.data[0], 0x42U);
    EXPECT_EQ(record->frame.data[1], 0xB6U);
    EXPECT_EQ(record->frame.data[2], 0x3DU);
    EXPECT_EQ(record->frame.data[3], 0x3EU);
    EXPECT_EQ(record->frame.data[4], 0xBAU);
    EXPECT_EQ(record->frame.data[5], 0x54U);
    EXPECT_EQ(record->frame.data[6], 0x4FU);
    EXPECT_EQ(record->frame.data[7], 0x3FU);
}

TEST(CandumpParserTest, ParsesShortPayload)
{
    anbot::CandumpParser parser;

    const auto record = parser.parseLine(
        "(1455221182.890443) can1 1E9#4D00");

    ASSERT_TRUE(record.has_value());

    EXPECT_EQ(record->interface_name, "can1");
    EXPECT_EQ(record->frame.id, 0x1E9U);
    EXPECT_EQ(record->frame.size, 2U);
    EXPECT_EQ(record->frame.data[0], 0x4DU);
    EXPECT_EQ(record->frame.data[1], 0x00U);
}

TEST(CandumpParserTest, AcceptsLowercaseHex)
{
    anbot::CandumpParser parser;

    const auto record = parser.parseLine(
        "(1.250000) can0 184#52ffb00000000000");

    ASSERT_TRUE(record.has_value());

    EXPECT_EQ(record->frame.id, 0x184U);
    EXPECT_EQ(record->frame.size, 8U);
    EXPECT_EQ(record->frame.data[0], 0x52U);
    EXPECT_EQ(record->frame.data[1], 0xFFU);
    EXPECT_EQ(record->frame.data[2], 0xB0U);
}

TEST(CandumpParserTest, RejectsMissingTimestampParentheses)
{
    anbot::CandumpParser parser;

    EXPECT_FALSE(
        parser.parseLine(
            "1455221182.888050 can0 183#42B63D3EBA544F3F")
            .has_value());
}

TEST(CandumpParserTest, RejectsMissingSeparator)
{
    anbot::CandumpParser parser;

    EXPECT_FALSE(
        parser.parseLine(
            "(1455221182.888050) can0 18342B63D3EBA544F3F")
            .has_value());
}

TEST(CandumpParserTest, RejectsOddLengthPayload)
{
    anbot::CandumpParser parser;

    EXPECT_FALSE(
        parser.parseLine(
            "(1455221182.888050) can0 183#123")
            .has_value());
}

TEST(CandumpParserTest, RejectsPayloadLargerThanEightBytes)
{
    anbot::CandumpParser parser;

    EXPECT_FALSE(
        parser.parseLine(
            "(1455221182.888050) can0 183#001122334455667788")
            .has_value());
}

TEST(CandumpParserTest, RejectsInvalidHexPayload)
{
    anbot::CandumpParser parser;

    EXPECT_FALSE(
        parser.parseLine(
            "(1455221182.888050) can0 183#42B63D3EBA544F3Z")
            .has_value());
}

TEST(CandumpParserTest, RejectsInvalidCanId)
{
    anbot::CandumpParser parser;

    EXPECT_FALSE(
        parser.parseLine(
            "(1455221182.888050) can0 XYZ#00000000")
            .has_value());
}

TEST(CandumpParserTest, RejectsIncompleteLine)
{
    anbot::CandumpParser parser;

    EXPECT_FALSE(parser.parseLine("").has_value());
    EXPECT_FALSE(parser.parseLine("(1.0) can0").has_value());
}

}  // namespace
