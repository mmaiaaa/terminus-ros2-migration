#include <gtest/gtest.h>

#include <array>
#include <cstdint>

#include "anbot_chassis_protocol/chassis_can_protocol.hpp"

namespace
{

TEST(ChassisCanProtocolTest, DecodesPositionFrame)
{
    anbot::ChassisCanProtocol protocol;

    const anbot::CanFrame frame{
        0x183U,
        std::array<uint8_t, 8>{
            0x33, 0x01, 0x69, 0x3E,
            0x59, 0xB1, 0x25, 0x3F
        },
        8U
    };

    const auto position = protocol.decodePosition(frame);

    ASSERT_TRUE(position.has_value());
    EXPECT_FLOAT_EQ(position->x, 0.227543637156F);
    EXPECT_FLOAT_EQ(position->y, 0.647237360477F);
}

TEST(ChassisCanProtocolTest, DecodesHeadingFrame)
{
    anbot::ChassisCanProtocol protocol;

    const anbot::CanFrame frame{
        0x283U,
        std::array<uint8_t, 8>{
            0xB4, 0xE1, 0xE9, 0x3F,
            0x00, 0x00, 0x00, 0x03
        },
        8U
    };

    const auto heading = protocol.decodeHeading(frame);

    ASSERT_TRUE(heading.has_value());
    EXPECT_FLOAT_EQ(heading->theta, 1.82720041275F);

    const std::array<uint8_t, 4> expected_trailing{
        0x00, 0x00, 0x00, 0x03
    };

    EXPECT_EQ(heading->trailing_bytes, expected_trailing);
}

TEST(ChassisCanProtocolTest, DecodesPositiveWheelSpeeds)
{
    anbot::ChassisCanProtocol protocol;

    const anbot::CanFrame frame{
        0x184U,
        std::array<uint8_t, 8>{
            0x26, 0x00,
            0x26, 0x00,
            0x00, 0x00, 0x00, 0x00
        },
        8U
    };

    const auto speeds = protocol.decodeWheelSpeeds(frame);

    ASSERT_TRUE(speeds.has_value());
    EXPECT_EQ(speeds->left_speed, 38);
    EXPECT_EQ(speeds->right_speed, 38);
}

TEST(ChassisCanProtocolTest, DecodesSignedRotationWheelSpeeds)
{
    anbot::ChassisCanProtocol protocol;

    const anbot::CanFrame frame{
        0x184U,
        std::array<uint8_t, 8>{
            0x52, 0xFF,
            0xB0, 0x00,
            0x00, 0x00, 0x00, 0x00
        },
        8U
    };

    const auto speeds = protocol.decodeWheelSpeeds(frame);

    ASSERT_TRUE(speeds.has_value());
    EXPECT_EQ(speeds->left_speed, -174);
    EXPECT_EQ(speeds->right_speed, 176);
}

TEST(ChassisCanProtocolTest, PreservesWheelStatusBytes)
{
    anbot::ChassisCanProtocol protocol;

    const anbot::CanFrame frame{
        0x184U,
        std::array<uint8_t, 8>{
            0x09, 0x00,
            0x06, 0x00,
            0x11, 0x22, 0x33, 0x44
        },
        8U
    };

    const auto speeds = protocol.decodeWheelSpeeds(frame);

    ASSERT_TRUE(speeds.has_value());

    const std::array<uint8_t, 4> expected_status{
        0x11, 0x22, 0x33, 0x44
    };

    EXPECT_EQ(speeds->status_bytes, expected_status);
}

TEST(ChassisCanProtocolTest, RejectsWrongFrameId)
{
    anbot::ChassisCanProtocol protocol;

    const anbot::CanFrame frame{
        0x194U,
        std::array<uint8_t, 8>{},
        8U
    };

    EXPECT_FALSE(protocol.decodeWheelSpeeds(frame).has_value());
}

TEST(ChassisCanProtocolTest, RejectsWrongPayloadSize)
{
    anbot::ChassisCanProtocol protocol;

    const anbot::CanFrame frame{
        0x183U,
        std::array<uint8_t, 8>{},
        7U
    };

    EXPECT_FALSE(protocol.decodePosition(frame).has_value());
}

}  // namespace
