#pragma once

#include <chrono>
#include <cstddef>
#include <cstdint>
#include <memory>
#include <optional>
#include <string>

#include "anbot_chassis_driver/chassis_can_driver.hpp"

#include "diagnostic_msgs/msg/diagnostic_array.hpp"
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

    void publishDiagnostics();

    static geometry_msgs::msg::Quaternion
    quaternionFromYaw(double yaw);

    std::string can_interface_;
    std::string odom_frame_id_;
    std::string base_frame_id_;

    int poll_period_ms_ = 10;
    int maximum_frames_per_poll_ = 64;
    int diagnostic_period_ms_ = 1000;
    int stale_timeout_ms_ = 2000;

    double pose_covariance_x_ = 0.01;
    double pose_covariance_y_ = 0.01;
    double pose_covariance_z_ = 99999.0;
    double pose_covariance_roll_ = 99999.0;
    double pose_covariance_pitch_ = 99999.0;
    double pose_covariance_yaw_ = 0.01;

    double twist_covariance_x_ = 999999.0;
    double twist_covariance_y_ = 999999.0;
    double twist_covariance_z_ = 999999.0;
    double twist_covariance_roll_ = 999999.0;
    double twist_covariance_pitch_ = 999999.0;
    double twist_covariance_yaw_ = 999999.0;

    ChassisCanDriver driver_;

    rclcpp::Publisher<nav_msgs::msg::Odometry>::SharedPtr
        odometry_publisher_;

    rclcpp::Publisher<
        diagnostic_msgs::msg::DiagnosticArray>::SharedPtr
        diagnostics_publisher_;

    rclcpp::Publisher<
        std_msgs::msg::Int16MultiArray>::SharedPtr
        wheel_speeds_publisher_;

    rclcpp::TimerBase::SharedPtr poll_timer_;
    rclcpp::TimerBase::SharedPtr diagnostics_timer_;

    std::optional<rclcpp::Time> last_decoded_frame_time_;
    std::size_t previous_decoded_frame_count_ = 0U;

    std::optional<ChassisPosition> last_published_position_;
    std::optional<ChassisHeading> last_published_heading_;
    std::optional<ChassisWheelSpeeds>
        last_published_wheel_speeds_;
};

}  // namespace anbot
