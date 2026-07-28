from launch import LaunchDescription
from launch.actions import DeclareLaunchArgument
from launch.substitutions import LaunchConfiguration, PathJoinSubstitution
from launch_ros.actions import Node
from launch_ros.substitutions import FindPackageShare


def generate_launch_description():
    can_interface = LaunchConfiguration("can_interface")
    parameters_file = LaunchConfiguration("parameters_file")

    default_parameters_file = PathJoinSubstitution(
        [
            FindPackageShare("anbot_chassis_driver"),
            "config",
            "chassis_can.yaml",
        ]
    )

    return LaunchDescription(
        [
            DeclareLaunchArgument(
                "can_interface",
                default_value="can0",
                description="SocketCAN interface used by the chassis receiver",
            ),
            DeclareLaunchArgument(
                "parameters_file",
                default_value=default_parameters_file,
                description="YAML parameter file for the chassis CAN node",
            ),
            Node(
                package="anbot_chassis_driver",
                executable="anbot_chassis_can_node",
                name="anbot_chassis_can_node",
                output="screen",
                parameters=[
                    parameters_file,
                    {
                        "can_interface": can_interface,
                    },
                ],
            ),
        ]
    )
