#include <gtest/gtest.h>

#include <array>
#include <cstdint>

#include "anbot_chassis_driver/chassis_can_driver.hpp"

namespace
{

anbot::ChassisCanDriver makeDriver()
{
    anbot::SocketCanConfiguration configuration;
    configuration.interface_name = "can0";

    return anbot::ChassisCanDriver(configuration);
}

TEST(ChassisCanDriverTest, StartsWithoutDecodedState)
{
    auto driver = makeDriver();

    EXPECT_FALSE(driver.isOpen());
    EXPECT_FALSE(driver.latestPosition().has_value());
    EXPECT_FALSE(driver.latestHeading().has_value());
    EXPECT_FALSE(driver.latestWheelSpeeds().has_value());

    EXPECT_EQ(driver.receivedFrameCount(), 0U);
    EXPECT_EQ(driver.decodedFrameCount(), 0U);
    EXPECT_EQ(driver.unknownFrameCount(), 0U);
}

TEST(ChassisCanDriverTest, ProcessesPositionFrame)
{
    auto driver = makeDriver();

    const anbot::CanFrame frame{
        0x183U,
        std::array<uint8_t, 8>{
            0x33, 0x01, 0x69, 0x3E,
            0x59, 0xB1, 0x25, 0x3F
        },
        8U
    };

    EXPECT_TRUE(driver.processFrame(frame));

    const auto position = driver.latestPosition();

    ASSERT_TRUE(position.has_value());
    EXPECT_FLOAT_EQ(position->x, 0.227543637156F);
    EXPECT_FLOAT_EQ(position->y, 0.647237360477F);

    EXPECT_EQ(driver.receivedFrameCount(), 1U);
    EXPECT_EQ(driver.decodedFrameCount(), 1U);
    EXPECT_EQ(driver.unknownFrameCount(), 0U);
}

TEST(ChassisCanDriverTest, ProcessesHeadingFrame)
{
    auto driver = makeDriver();

    const anbot::CanFrame frame{
        0x283U,
        std::array<uint8_t, 8>{
            0xB4, 0xE1, 0xE9, 0x3F,
            0x00, 0x00, 0x00, 0x03
        },
        8U
    };

    EXPECT_TRUE(driver.processFrame(frame));

    const auto heading = driver.latestHeading();

    ASSERT_TRUE(heading.has_value());
    EXPECT_FLOAT_EQ(heading->theta, 1.82720041275F);

    const std::array<uint8_t, 4> expected{
        0x00, 0x00, 0x00, 0x03
    };

    EXPECT_EQ(heading->trailing_bytes, expected);
}

TEST(ChassisCanDriverTest, ProcessesWheelSpeedFrame)
{
    auto driver = makeDriver();

    const anbot::CanFrame frame{
        0x184U,
        std::array<uint8_t, 8>{
            0x52, 0xFF,
            0xB0, 0x00,
            0x11, 0x22, 0x33, 0x44
        },
        8U
    };

    EXPECT_TRUE(driver.processFrame(frame));

    const auto wheel_speeds =
        driver.latestWheelSpeeds();

    ASSERT_TRUE(wheel_speeds.has_value());
    EXPECT_EQ(wheel_speeds->left_speed, -174);
    EXPECT_EQ(wheel_speeds->right_speed, 176);

    const std::array<uint8_t, 4> expected_status{
        0x11, 0x22, 0x33, 0x44
    };

    EXPECT_EQ(
        wheel_speeds->status_bytes,
        expected_status);
}

TEST(ChassisCanDriverTest, PreservesIndependentLatestStates)
{
    auto driver = makeDriver();

    const anbot::CanFrame position_frame{
        0x183U,
        std::array<uint8_t, 8>{
            0x33, 0x01, 0x69, 0x3E,
            0x59, 0xB1, 0x25, 0x3F
        },
        8U
    };

    const anbot::CanFrame heading_frame{
        0x283U,
        std::array<uint8_t, 8>{
            0xB4, 0xE1, 0xE9, 0x3F,
            0x00, 0x00, 0x00, 0x03
        },
        8U
    };

    const anbot::CanFrame wheel_frame{
        0x184U,
        std::array<uint8_t, 8>{
            0x26, 0x00,
            0x26, 0x00,
            0x00, 0x00, 0x00, 0x00
        },
        8U
    };

    EXPECT_TRUE(driver.processFrame(position_frame));
    EXPECT_TRUE(driver.processFrame(heading_frame));
    EXPECT_TRUE(driver.processFrame(wheel_frame));

    EXPECT_TRUE(driver.latestPosition().has_value());
    EXPECT_TRUE(driver.latestHeading().has_value());
    EXPECT_TRUE(driver.latestWheelSpeeds().has_value());

    EXPECT_EQ(driver.receivedFrameCount(), 3U);
    EXPECT_EQ(driver.decodedFrameCount(), 3U);
    EXPECT_EQ(driver.unknownFrameCount(), 0U);
}

TEST(ChassisCanDriverTest, CountsUnknownIdentifier)
{
    auto driver = makeDriver();

    const anbot::CanFrame frame{
        0x194U,
        std::array<uint8_t, 8>{},
        8U
    };

    EXPECT_FALSE(driver.processFrame(frame));

    EXPECT_EQ(driver.receivedFrameCount(), 1U);
    EXPECT_EQ(driver.decodedFrameCount(), 0U);
    EXPECT_EQ(driver.unknownFrameCount(), 1U);
}

TEST(ChassisCanDriverTest, CountsIncorrectPayloadSizeAsUnknown)
{
    auto driver = makeDriver();

    const anbot::CanFrame frame{
        0x183U,
        std::array<uint8_t, 8>{},
        7U
    };

    EXPECT_FALSE(driver.processFrame(frame));

    EXPECT_FALSE(driver.latestPosition().has_value());
    EXPECT_EQ(driver.receivedFrameCount(), 1U);
    EXPECT_EQ(driver.decodedFrameCount(), 0U);
    EXPECT_EQ(driver.unknownFrameCount(), 1U);
}

TEST(ChassisCanDriverTest, ZeroFramePollDoesNothing)
{
    auto driver = makeDriver();

    EXPECT_EQ(driver.poll(0U), 0U);
    EXPECT_EQ(driver.receivedFrameCount(), 0U);
}

TEST(ChassisCanDriverTest, PollWhileClosedDoesNothing)
{
    auto driver = makeDriver();

    EXPECT_EQ(driver.poll(), 0U);
    EXPECT_EQ(driver.receivedFrameCount(), 0U);
}

TEST(ChassisCanDriverTest, ResetsStatisticsWithoutClearingState)
{
    auto driver = makeDriver();

    const anbot::CanFrame frame{
        0x184U,
        std::array<uint8_t, 8>{
            0x09, 0x00,
            0x06, 0x00,
            0x00, 0x00, 0x00, 0x00
        },
        8U
    };

    ASSERT_TRUE(driver.processFrame(frame));
    ASSERT_TRUE(driver.latestWheelSpeeds().has_value());

    driver.resetStatistics();

    EXPECT_EQ(driver.receivedFrameCount(), 0U);
    EXPECT_EQ(driver.decodedFrameCount(), 0U);
    EXPECT_EQ(driver.unknownFrameCount(), 0U);

    EXPECT_TRUE(driver.latestWheelSpeeds().has_value());
}

TEST(ChassisCanDriverTest, ExposesConfiguration)
{
    auto driver = makeDriver();

    EXPECT_EQ(
        driver.configuration().interface_name,
        "can0");
}

}  // namespace
