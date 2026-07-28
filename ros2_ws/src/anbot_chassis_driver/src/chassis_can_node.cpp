#include "anbot_chassis_driver/chassis_can_node.hpp"

#include <algorithm>
#include <cmath>
#include <functional>
#include <utility>

namespace anbot
{

namespace
{

SocketCanConfiguration makeConfiguration(
    const std::string& interface_name)
{
    SocketCanConfiguration configuration;
    configuration.interface_name = interface_name;
    return configuration;
}

bool positionsEqual(
    const ChassisPosition& left,
    const ChassisPosition& right)
{
    return left.x == right.x &&
           left.y == right.y;
}

bool headingsEqual(
    const ChassisHeading& left,
    const ChassisHeading& right)
{
    return left.theta == right.theta &&
           left.trailing_bytes == right.trailing_bytes;
}

bool wheelSpeedsEqual(
    const ChassisWheelSpeeds& left,
    const ChassisWheelSpeeds& right)
{
    return
        left.left_speed == right.left_speed &&
        left.right_speed == right.right_speed &&
        left.status_bytes == right.status_bytes;
}

}  // namespace

ChassisCanNode::ChassisCanNode(
    const rclcpp::NodeOptions& options)
    : Node("anbot_chassis_can_node", options),
      can_interface_(
          declare_parameter<std::string>(
              "can_interface",
              "can0")),
      odom_frame_id_(
          declare_parameter<std::string>(
              "odom_frame_id",
              "odom")),
      base_frame_id_(
          declare_parameter<std::string>(
              "base_frame_id",
              "base_link")),
      poll_period_ms_(
          declare_parameter<int>(
              "poll_period_ms",
              10)),
      maximum_frames_per_poll_(
          declare_parameter<int>(
              "maximum_frames_per_poll",
              64)),
      driver_(makeConfiguration(can_interface_))
{
    poll_period_ms_ = std::max(poll_period_ms_, 1);
    maximum_frames_per_poll_ =
        std::max(maximum_frames_per_poll_, 1);

    odometry_publisher_ =
        create_publisher<nav_msgs::msg::Odometry>(
            "chassis/odom",
            rclcpp::SensorDataQoS());

    wheel_speeds_publisher_ =
        create_publisher<std_msgs::msg::Int16MultiArray>(
            "chassis/wheel_speeds",
            rclcpp::SensorDataQoS());

    if (!driver_.open())
    {
        RCLCPP_ERROR(
            get_logger(),
            "Failed to open SocketCAN interface '%s'; errno=%d",
            can_interface_.c_str(),
            driver_.lastSocketError());
    }
    else
    {
        RCLCPP_INFO(
            get_logger(),
            "Listening read-only on SocketCAN interface '%s'",
            can_interface_.c_str());
    }

    poll_timer_ = create_wall_timer(
        std::chrono::milliseconds(poll_period_ms_),
        std::bind(
            &ChassisCanNode::pollCan,
            this));
}

void ChassisCanNode::pollCan()
{
    if (!driver_.isOpen())
    {
        return;
    }

    driver_.poll(
        static_cast<std::size_t>(
            maximum_frames_per_poll_));

    publishOdometryIfReady();
    publishWheelSpeedsIfUpdated();
}

void ChassisCanNode::publishOdometryIfReady()
{
    const auto position = driver_.latestPosition();
    const auto heading = driver_.latestHeading();

    if (!position.has_value() || !heading.has_value())
    {
        return;
    }

    const bool position_changed =
        !last_published_position_.has_value() ||
        !positionsEqual(
            *last_published_position_,
            *position);

    const bool heading_changed =
        !last_published_heading_.has_value() ||
        !headingsEqual(
            *last_published_heading_,
            *heading);

    if (!position_changed && !heading_changed)
    {
        return;
    }

    nav_msgs::msg::Odometry message;

    message.header.stamp = now();
    message.header.frame_id = odom_frame_id_;
    message.child_frame_id = base_frame_id_;

    message.pose.pose.position.x =
        static_cast<double>(position->x);

    message.pose.pose.position.y =
        static_cast<double>(position->y);

    message.pose.pose.position.z = 0.0;

    message.pose.pose.orientation =
        quaternionFromYaw(
            static_cast<double>(heading->theta));

    odometry_publisher_->publish(message);

    last_published_position_ = position;
    last_published_heading_ = heading;
}

void ChassisCanNode::publishWheelSpeedsIfUpdated()
{
    const auto wheel_speeds =
        driver_.latestWheelSpeeds();

    if (!wheel_speeds.has_value())
    {
        return;
    }

    if (
        last_published_wheel_speeds_.has_value() &&
        wheelSpeedsEqual(
            *last_published_wheel_speeds_,
            *wheel_speeds))
    {
        return;
    }

    std_msgs::msg::Int16MultiArray message;

    message.data = {
        wheel_speeds->left_speed,
        wheel_speeds->right_speed,
        static_cast<int16_t>(
            wheel_speeds->status_bytes[0]),
        static_cast<int16_t>(
            wheel_speeds->status_bytes[1]),
        static_cast<int16_t>(
            wheel_speeds->status_bytes[2]),
        static_cast<int16_t>(
            wheel_speeds->status_bytes[3])
    };

    wheel_speeds_publisher_->publish(message);

    last_published_wheel_speeds_ =
        wheel_speeds;
}

geometry_msgs::msg::Quaternion
ChassisCanNode::quaternionFromYaw(
    const double yaw)
{
    geometry_msgs::msg::Quaternion quaternion;

    const double half_yaw = yaw * 0.5;

    quaternion.x = 0.0;
    quaternion.y = 0.0;
    quaternion.z = std::sin(half_yaw);
    quaternion.w = std::cos(half_yaw);

    return quaternion;
}

}  // namespace anbot
