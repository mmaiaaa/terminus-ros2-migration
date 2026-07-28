#pragma once

#include <chrono>
#include <cstddef>
#include <cstdint>
#include <memory>
#include <optional>
#include <string>

#include "anbot_chassis_driver/chassis_can_driver.hpp"

#include "geometry_msgs/msg/quaternion.hpp"
#include "nav_msgs/msg/odometry.hpp"
#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/int16_multi_array.hpp"

namespace anbot
{

class ChassisCanNode final : public rclcpp::Node
{
public:
    explicit ChassisCanNode(
        const rclcpp::NodeOptions& options =
            rclcpp::NodeOptions());

private:
    void pollCan();

    void publishOdometryIfReady();

    void publishWheelSpeedsIfUpdated();

    static geometry_msgs::msg::Quaternion
    quaternionFromYaw(double yaw);

    std::string can_interface_;
    std::string odom_frame_id_;
    std::string base_frame_id_;

    int poll_period_ms_ = 10;
    int maximum_frames_per_poll_ = 64;

    ChassisCanDriver driver_;

    rclcpp::Publisher<nav_msgs::msg::Odometry>::SharedPtr
        odometry_publisher_;

    rclcpp::Publisher<
        std_msgs::msg::Int16MultiArray>::SharedPtr
        wheel_speeds_publisher_;

    rclcpp::TimerBase::SharedPtr poll_timer_;

    std::optional<ChassisPosition> last_published_position_;
    std::optional<ChassisHeading> last_published_heading_;
    std::optional<ChassisWheelSpeeds>
        last_published_wheel_speeds_;
};

}  // namespace anbot
