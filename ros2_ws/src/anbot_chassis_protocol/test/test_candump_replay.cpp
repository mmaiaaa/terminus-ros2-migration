#include <gtest/gtest.h>

#include <sstream>
#include <string>

#include "anbot_chassis_protocol/candump_replay.hpp"

namespace
{

TEST(CandumpReplayTest, ProcessesConfirmedChassisFrames)
{
    std::istringstream input(
        "(1.000000) can0 183#3301693E59B1253F\n"
        "(1.010000) can0 283#B4E1E93F00000003\n"
        "(1.020000) can0 184#2600260000000000\n"
        "(1.030000) can0 184#52FFB00000000000\n");

    anbot::CandumpReplay replay;

    const auto statistics = replay.process(input);

    EXPECT_EQ(statistics.total_lines, 4U);
    EXPECT_EQ(statistics.parsed_lines, 4U);
    EXPECT_EQ(statistics.malformed_lines, 0U);

    EXPECT_EQ(statistics.selected_interface_frames, 4U);
    EXPECT_EQ(statistics.ignored_interface_frames, 0U);

    EXPECT_EQ(statistics.position_frames, 1U);
    EXPECT_EQ(statistics.heading_frames, 1U);
    EXPECT_EQ(statistics.wheel_speed_frames, 2U);
    EXPECT_EQ(statistics.unknown_frames, 0U);

    ASSERT_TRUE(statistics.first_timestamp.has_value());
    ASSERT_TRUE(statistics.last_timestamp.has_value());

    EXPECT_DOUBLE_EQ(*statistics.first_timestamp, 1.0);
    EXPECT_DOUBLE_EQ(*statistics.last_timestamp, 1.03);

    ASSERT_TRUE(statistics.first_position.has_value());
    ASSERT_TRUE(statistics.last_position.has_value());

    EXPECT_FLOAT_EQ(
        statistics.first_position->x,
        0.227543637156F);

    EXPECT_FLOAT_EQ(
        statistics.first_position->y,
        0.647237360477F);

    ASSERT_TRUE(statistics.first_heading.has_value());
    ASSERT_TRUE(statistics.last_heading.has_value());

    EXPECT_FLOAT_EQ(
        statistics.first_heading->theta,
        1.82720041275F);

    ASSERT_TRUE(statistics.minimum_left_speed.has_value());
    ASSERT_TRUE(statistics.maximum_left_speed.has_value());
    ASSERT_TRUE(statistics.minimum_right_speed.has_value());
    ASSERT_TRUE(statistics.maximum_right_speed.has_value());

    EXPECT_EQ(*statistics.minimum_left_speed, -174);
    EXPECT_EQ(*statistics.maximum_left_speed, 38);
    EXPECT_EQ(*statistics.minimum_right_speed, 38);
    EXPECT_EQ(*statistics.maximum_right_speed, 176);
}

TEST(CandumpReplayTest, CountsMalformedAndUnknownFrames)
{
    std::istringstream input(
        "invalid line\n"
        "(2.000000) can0 194#0000000000000000\n"
        "(2.010000) can0 183#00000000\n");

    anbot::CandumpReplay replay;

    const auto statistics = replay.process(input);

    EXPECT_EQ(statistics.total_lines, 3U);
    EXPECT_EQ(statistics.parsed_lines, 2U);
    EXPECT_EQ(statistics.malformed_lines, 1U);

    EXPECT_EQ(statistics.selected_interface_frames, 2U);
    EXPECT_EQ(statistics.unknown_frames, 2U);

    EXPECT_EQ(statistics.position_frames, 0U);
    EXPECT_EQ(statistics.heading_frames, 0U);
    EXPECT_EQ(statistics.wheel_speed_frames, 0U);
}

TEST(CandumpReplayTest, IgnoresFramesFromOtherInterfaces)
{
    std::istringstream input(
        "(3.000000) can1 1E9#4D00\n"
        "(3.010000) can1 1E8#0000\n"
        "(3.020000) can0 184#0900060000000000\n");

    anbot::CandumpReplay replay;

    const auto statistics = replay.process(input, "can0");

    EXPECT_EQ(statistics.total_lines, 3U);
    EXPECT_EQ(statistics.parsed_lines, 3U);

    EXPECT_EQ(statistics.selected_interface_frames, 1U);
    EXPECT_EQ(statistics.ignored_interface_frames, 2U);

    EXPECT_EQ(statistics.wheel_speed_frames, 1U);
    EXPECT_EQ(statistics.unknown_frames, 0U);

    ASSERT_TRUE(statistics.minimum_left_speed.has_value());
    ASSERT_TRUE(statistics.maximum_left_speed.has_value());
    ASSERT_TRUE(statistics.minimum_right_speed.has_value());
    ASSERT_TRUE(statistics.maximum_right_speed.has_value());

    EXPECT_EQ(*statistics.minimum_left_speed, 9);
    EXPECT_EQ(*statistics.maximum_left_speed, 9);
    EXPECT_EQ(*statistics.minimum_right_speed, 6);
    EXPECT_EQ(*statistics.maximum_right_speed, 6);
}

TEST(CandumpReplayTest, HandlesEmptyInput)
{
    std::istringstream input("");

    anbot::CandumpReplay replay;

    const auto statistics = replay.process(input);

    EXPECT_EQ(statistics.total_lines, 0U);
    EXPECT_EQ(statistics.parsed_lines, 0U);
    EXPECT_EQ(statistics.malformed_lines, 0U);

    EXPECT_FALSE(statistics.first_timestamp.has_value());
    EXPECT_FALSE(statistics.last_timestamp.has_value());

    EXPECT_FALSE(statistics.first_position.has_value());
    EXPECT_FALSE(statistics.last_position.has_value());

    EXPECT_FALSE(statistics.first_heading.has_value());
    EXPECT_FALSE(statistics.last_heading.has_value());

    EXPECT_FALSE(statistics.minimum_left_speed.has_value());
    EXPECT_FALSE(statistics.maximum_left_speed.has_value());
    EXPECT_FALSE(statistics.minimum_right_speed.has_value());
    EXPECT_FALSE(statistics.maximum_right_speed.has_value());
}

}  // namespace
