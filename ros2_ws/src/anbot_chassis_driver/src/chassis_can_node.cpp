#include "anbot_chassis_driver/chassis_can_node.hpp"

#include "diagnostic_msgs/msg/diagnostic_status.hpp"
#include "diagnostic_msgs/msg/key_value.hpp"

#include <algorithm>
#include <cmath>
#include <functional>
#include <string>
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
              "base_footprint")),
      poll_period_ms_(
          declare_parameter<int>(
              "poll_period_ms",
              10)),
      maximum_frames_per_poll_(
          declare_parameter<int>(
              "maximum_frames_per_poll",
              64)),
      diagnostic_period_ms_(
          declare_parameter<int>(
              "diagnostic_period_ms",
              1000)),
      stale_timeout_ms_(
          declare_parameter<int>(
              "stale_timeout_ms",
              2000)),
      pose_covariance_x_(
          declare_parameter<double>(
              "pose_covariance.x",
              0.01)),
      pose_covariance_y_(
          declare_parameter<double>(
              "pose_covariance.y",
              0.01)),
      pose_covariance_z_(
          declare_parameter<double>(
              "pose_covariance.z",
              99999.0)),
      pose_covariance_roll_(
          declare_parameter<double>(
              "pose_covariance.roll",
              99999.0)),
      pose_covariance_pitch_(
          declare_parameter<double>(
              "pose_covariance.pitch",
              99999.0)),
      pose_covariance_yaw_(
          declare_parameter<double>(
              "pose_covariance.yaw",
              0.01)),
      twist_covariance_x_(
          declare_parameter<double>(
              "twist_covariance.x",
              999999.0)),
      twist_covariance_y_(
          declare_parameter<double>(
              "twist_covariance.y",
              999999.0)),
      twist_covariance_z_(
          declare_parameter<double>(
              "twist_covariance.z",
              999999.0)),
      twist_covariance_roll_(
          declare_parameter<double>(
              "twist_covariance.roll",
              999999.0)),
      twist_covariance_pitch_(
          declare_parameter<double>(
              "twist_covariance.pitch",
              999999.0)),
      twist_covariance_yaw_(
          declare_parameter<double>(
              "twist_covariance.yaw",
              999999.0)),
      driver_(makeConfiguration(can_interface_))
{
    poll_period_ms_ = std::max(poll_period_ms_, 1);
    maximum_frames_per_poll_ =
        std::max(maximum_frames_per_poll_, 1);

    diagnostic_period_ms_ =
        std::max(diagnostic_period_ms_, 100);

    stale_timeout_ms_ =
        std::max(stale_timeout_ms_, 1);

    odometry_publisher_ =
        create_publisher<nav_msgs::msg::Odometry>(
            "chassis/odom",
            rclcpp::SensorDataQoS());

    wheel_speeds_publisher_ =
        create_publisher<std_msgs::msg::Int16MultiArray>(
            "chassis/wheel_speeds",
            rclcpp::SensorDataQoS());

    diagnostics_publisher_ =
        create_publisher<
            diagnostic_msgs::msg::DiagnosticArray>(
            "diagnostics",
            rclcpp::QoS(10));

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

    diagnostics_timer_ = create_wall_timer(
        std::chrono::milliseconds(diagnostic_period_ms_),
        std::bind(
            &ChassisCanNode::publishDiagnostics,
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

    const std::size_t decoded_frame_count =
        driver_.decodedFrameCount();

    if (decoded_frame_count > previous_decoded_frame_count_)
    {
        last_decoded_frame_time_ = now();
    }

    previous_decoded_frame_count_ = decoded_frame_count;

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

    message.pose.covariance[0] =
        pose_covariance_x_;
    message.pose.covariance[7] =
        pose_covariance_y_;
    message.pose.covariance[14] =
        pose_covariance_z_;
    message.pose.covariance[21] =
        pose_covariance_roll_;
    message.pose.covariance[28] =
        pose_covariance_pitch_;
    message.pose.covariance[35] =
        pose_covariance_yaw_;

    message.twist.covariance[0] =
        twist_covariance_x_;
    message.twist.covariance[7] =
        twist_covariance_y_;
    message.twist.covariance[14] =
        twist_covariance_z_;
    message.twist.covariance[21] =
        twist_covariance_roll_;
    message.twist.covariance[28] =
        twist_covariance_pitch_;
    message.twist.covariance[35] =
        twist_covariance_yaw_;

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

void ChassisCanNode::publishDiagnostics()
{
    diagnostic_msgs::msg::DiagnosticArray message;
    message.header.stamp = now();

    diagnostic_msgs::msg::DiagnosticStatus status;

    status.name = "Terminus chassis CAN receiver";
    status.hardware_id = can_interface_;

    double seconds_since_last_decoded_frame = -1.0;

    if (!driver_.isOpen())
    {
        status.level =
            diagnostic_msgs::msg::DiagnosticStatus::ERROR;

        status.message =
            "SocketCAN interface is not open";
    }
    else if (!last_decoded_frame_time_.has_value())
    {
        status.level =
            diagnostic_msgs::msg::DiagnosticStatus::WARN;

        status.message =
            "Interface open; no valid chassis frame received";
    }
    else
    {
        seconds_since_last_decoded_frame =
            (now() - *last_decoded_frame_time_).seconds();

        const double stale_timeout_seconds =
            static_cast<double>(stale_timeout_ms_) / 1000.0;

        if (
            seconds_since_last_decoded_frame >
            stale_timeout_seconds)
        {
            status.level =
                diagnostic_msgs::msg::DiagnosticStatus::WARN;

            status.message =
                "Chassis CAN data is stale";
        }
        else
        {
            status.level =
                diagnostic_msgs::msg::DiagnosticStatus::OK;

            status.message =
                "Receiving valid chassis CAN frames";
        }
    }

    const auto add_value =
        [&status](
            const std::string& key,
            const std::string& value)
        {
            diagnostic_msgs::msg::KeyValue item;
            item.key = key;
            item.value = value;
            status.values.push_back(item);
        };

    add_value("can_interface", can_interface_);

    add_value(
        "interface_open",
        driver_.isOpen() ? "true" : "false");

    add_value(
        "last_socket_error",
        std::to_string(driver_.lastSocketError()));

    add_value(
        "received_frame_count",
        std::to_string(driver_.receivedFrameCount()));

    add_value(
        "decoded_frame_count",
        std::to_string(driver_.decodedFrameCount()));

    add_value(
        "unknown_frame_count",
        std::to_string(driver_.unknownFrameCount()));

    if (seconds_since_last_decoded_frame < 0.0)
    {
        add_value(
            "seconds_since_last_decoded_frame",
            "never");
    }
    else
    {
        add_value(
            "seconds_since_last_decoded_frame",
            std::to_string(
                seconds_since_last_decoded_frame));
    }

    add_value(
        "stale_timeout_ms",
        std::to_string(stale_timeout_ms_));

    message.status.push_back(status);
    diagnostics_publisher_->publish(message);
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
