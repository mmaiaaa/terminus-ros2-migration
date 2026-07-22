#include <gtest/gtest.h>

#include <cstdint>
#include <vector>

#include "anbot_chassis_protocol/wheel_protocol.hpp"

namespace
{

TEST(WheelProtocolTest, EncodesPositiveSpeeds)
{
    anbot::WheelProtocol protocol;

    const anbot::WheelCommand command{
        0x1234,
        0x0567,
        false
    };

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

    EXPECT_EQ(protocol.encodeCommand(command), expected);
}

TEST(WheelProtocolTest, EncodesNegativeSpeedsAsTwosComplement)
{
    anbot::WheelProtocol protocol;

    const anbot::WheelCommand command{
        -1,
        -2,
        false
    };

    const std::vector<uint8_t> expected{
        0x04,
        0xFF,
        0xFF,
        0xFF,
        0xFE,
        0x00,
        0x00,
        0xFF
    };

    EXPECT_EQ(protocol.encodeCommand(command), expected);
}

TEST(WheelProtocolTest, EncodesEmergencyFlag)
{
    anbot::WheelProtocol protocol;

    const anbot::WheelCommand command{
        1,
        2,
        true
    };

    const std::vector<uint8_t> expected{
        0x04,
        0x00,
        0x01,
        0x00,
        0x02,
        0x00,
        0x01,
        0x08
    };

    EXPECT_EQ(protocol.encodeCommand(command), expected);
}

TEST(WheelProtocolTest, AddsMarkerOnSecondConsecutiveStop)
{
    anbot::WheelProtocol protocol;

    const anbot::WheelCommand stop_command{
        0,
        0,
        false
    };

    const std::vector<uint8_t> first_expected{
        0x04,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x04
    };

    const std::vector<uint8_t> second_expected{
        0x04,
        0x00,
        0x00,
        0x00,
        0x00,
        0xAA,
        0x00,
        0xAE
    };

    EXPECT_EQ(protocol.encodeCommand(stop_command), first_expected);
    EXPECT_EQ(protocol.encodeCommand(stop_command), second_expected);
}

TEST(WheelProtocolTest, MarkerIsAbsentOnThirdStop)
{
    anbot::WheelProtocol protocol;

    const anbot::WheelCommand stop_command{
        0,
        0,
        false
    };

    protocol.encodeCommand(stop_command);
    protocol.encodeCommand(stop_command);

    const std::vector<uint8_t> third_expected{
        0x04,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x04
    };

    EXPECT_EQ(protocol.encodeCommand(stop_command), third_expected);
}

TEST(WheelProtocolTest, MovementResetsStopCounter)
{
    anbot::WheelProtocol protocol;

    const anbot::WheelCommand stop_command{
        0,
        0,
        false
    };

    const anbot::WheelCommand movement_command{
        10,
        10,
        false
    };

    protocol.encodeCommand(stop_command);
    protocol.encodeCommand(movement_command);

    const std::vector<uint8_t> expected{
        0x04,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x04
    };

    EXPECT_EQ(protocol.encodeCommand(stop_command), expected);
}


TEST(WheelProtocolTest, DecodesValidFeedbackFrame)
{
    anbot::WheelProtocol protocol;

    const std::vector<uint8_t> frame{
        0x05,
        0x12,
        0x34,
        0x05,
        0x67,
        0x00,
        0x00,
        0x00,
        0x01,
        0xB8
    };

    const auto feedback = protocol.decodeFrame(frame);

    ASSERT_TRUE(feedback.has_value());
    EXPECT_TRUE(feedback->valid);
    EXPECT_EQ(feedback->left_value, 0x1234);
    EXPECT_EQ(feedback->right_value, 0x0567);
    EXPECT_EQ(feedback->status, 0x01);
}

TEST(WheelProtocolTest, DecodesNegativeFeedbackValues)
{
    anbot::WheelProtocol protocol;

    const std::vector<uint8_t> frame{
        0x05,
        0xFF,
        0xFF,
        0xFF,
        0xFE,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00
    };

    const auto feedback = protocol.decodeFrame(frame);

    ASSERT_TRUE(feedback.has_value());
    EXPECT_EQ(feedback->left_value, -1);
    EXPECT_EQ(feedback->right_value, -2);
}

TEST(WheelProtocolTest, RejectsIncorrectFeedbackSize)
{
    anbot::WheelProtocol protocol;

    const std::vector<uint8_t> frame{
        0x05,
        0x00
    };

    EXPECT_FALSE(protocol.decodeFrame(frame).has_value());
}

TEST(WheelProtocolTest, RejectsIncorrectFeedbackHeader)
{
    anbot::WheelProtocol protocol;

    const std::vector<uint8_t> frame{
        0x04,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x04
    };

    EXPECT_FALSE(protocol.decodeFrame(frame).has_value());
}

TEST(WheelProtocolTest, RejectsIncorrectFeedbackChecksum)
{
    anbot::WheelProtocol protocol;

    const std::vector<uint8_t> frame{
        0x05,
        0x00,
        0x01,
        0x00,
        0x02,
        0x00,
        0x00,
        0x00,
        0x00,
        0xFF
    };

    EXPECT_FALSE(protocol.decodeFrame(frame).has_value());
}

}  // namespace
