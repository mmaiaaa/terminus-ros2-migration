#!/usr/bin/env python3

import math
import os
import socket
import struct
import threading
import time
import unittest

import launch
import launch_ros.actions
import launch_testing
import launch_testing.actions
import pytest
import rclpy
from rclpy.qos import qos_profile_sensor_data
from diagnostic_msgs.msg import DiagnosticArray
from diagnostic_msgs.msg import DiagnosticStatus
from nav_msgs.msg import Odometry
from std_msgs.msg import Int16MultiArray


CAN_FRAME_FORMAT = "=IB3x8s"

POSITION_FRAME_ID = 0x183
WHEEL_SPEED_FRAME_ID = 0x184
HEADING_FRAME_ID = 0x283

POSITION_PAYLOAD = bytes.fromhex("3301693E59B1253F")
HEADING_PAYLOAD = bytes.fromhex("B4E1E93F00000003")
WHEEL_SPEED_PAYLOAD = bytes.fromhex("52FFB00011223344")


def vcan0_is_available():
    return os.path.exists("/sys/class/net/vcan0")


@pytest.mark.launch_test
def generate_test_description():
    chassis_node = launch_ros.actions.Node(
        package="anbot_chassis_driver",
        executable="anbot_chassis_can_node",
        name="anbot_chassis_can_node",
        output="screen",
        parameters=[
            {
                "can_interface": "vcan0",
                "odom_frame_id": "odom",
                "base_frame_id": "base_footprint",
                "poll_period_ms": 10,
                "maximum_frames_per_poll": 64,
                "diagnostic_period_ms": 200,
                "stale_timeout_ms": 500,
            }
        ],
    )

    return (
        launch.LaunchDescription(
            [
                chassis_node,
                launch_testing.actions.ReadyToTest(),
            ]
        ),
        {
            "chassis_node": chassis_node,
        },
    )


class TestChassisCanNode(unittest.TestCase):

    @classmethod
    def setUpClass(cls):
        if not vcan0_is_available():
            raise unittest.SkipTest(
                "vcan0 is unavailable; create it before running this test"
            )

        rclpy.init()

        cls.node = rclpy.create_node(
            "test_chassis_can_node_integration"
        )

        cls.odometry_message = None
        cls.wheel_speed_message = None
        cls.diagnostic_messages = []

        cls.odometry_subscription = cls.node.create_subscription(
            Odometry,
            "/chassis/odom",
            cls._odometry_callback,
            qos_profile_sensor_data,
        )

        cls.wheel_speed_subscription = cls.node.create_subscription(
            Int16MultiArray,
            "/chassis/wheel_speeds",
            cls._wheel_speed_callback,
            qos_profile_sensor_data,
        )

        cls.diagnostics_subscription = cls.node.create_subscription(
            DiagnosticArray,
            "/diagnostics",
            cls._diagnostics_callback,
            10,
        )

        cls.executor_thread = threading.Thread(
            target=cls._spin,
            daemon=True,
        )
        cls.executor_thread.start()

    @classmethod
    def tearDownClass(cls):
        if hasattr(cls, "node"):
            cls.node.destroy_node()

        if rclpy.ok():
            rclpy.shutdown()

        if hasattr(cls, "executor_thread"):
            cls.executor_thread.join(timeout=2.0)

    @classmethod
    def _spin(cls):
        while rclpy.ok():
            rclpy.spin_once(cls.node, timeout_sec=0.05)

    @classmethod
    def _odometry_callback(cls, message):
        cls.odometry_message = message

    @classmethod
    def _wheel_speed_callback(cls, message):
        cls.wheel_speed_message = message

    @classmethod
    def _diagnostics_callback(cls, message):
        cls.diagnostic_messages.append(message)

    @staticmethod
    def _diagnostic_values(status):
        return {
            item.key: item.value
            for item in status.values
        }

    @classmethod
    def _wait_for_diagnostic(cls, predicate, timeout=5.0):
        deadline = time.monotonic() + timeout

        while time.monotonic() < deadline:
            for message in reversed(cls.diagnostic_messages):
                for status in message.status:
                    if predicate(status):
                        return status

            time.sleep(0.05)

        raise AssertionError(
            "Timed out waiting for expected diagnostic status"
        )

    @staticmethod
    def _send_can_frame(can_socket, frame_id, payload):
        if len(payload) != 8:
            raise ValueError("CAN payload must contain exactly 8 bytes")

        frame = struct.pack(
            CAN_FRAME_FORMAT,
            frame_id,
            len(payload),
            payload,
        )

        can_socket.send(frame)

    def test_decodes_can_frames_and_publishes_ros_messages(self):
        # Allow DDS discovery and the launched node to finish opening vcan0.
        discovery_deadline = time.monotonic() + 5.0

        while time.monotonic() < discovery_deadline:
            if (
                self.node.count_publishers("/chassis/odom") > 0
                and self.node.count_publishers(
                    "/chassis/wheel_speeds"
                ) > 0
            ):
                break

            time.sleep(0.05)
        else:
            self.fail("Chassis publishers were not discovered")

        initial_status = self._wait_for_diagnostic(
            lambda status: (
                status.level == DiagnosticStatus.WARN
                and status.message
                == "Interface open; no valid chassis frame received"
            )
        )

        initial_values = self._diagnostic_values(initial_status)

        self.assertEqual(initial_values["can_interface"], "vcan0")
        self.assertEqual(initial_values["interface_open"], "true")
        self.assertEqual(initial_values["decoded_frame_count"], "0")
        self.assertEqual(
            initial_values["seconds_since_last_decoded_frame"],
            "never",
        )

        with socket.socket(
            socket.PF_CAN,
            socket.SOCK_RAW,
            socket.CAN_RAW,
        ) as can_socket:
            can_socket.bind(("vcan0",))

            self._send_can_frame(
                can_socket,
                POSITION_FRAME_ID,
                POSITION_PAYLOAD,
            )

            self._send_can_frame(
                can_socket,
                HEADING_FRAME_ID,
                HEADING_PAYLOAD,
            )

            self._send_can_frame(
                can_socket,
                WHEEL_SPEED_FRAME_ID,
                WHEEL_SPEED_PAYLOAD,
            )

        message_deadline = time.monotonic() + 5.0

        while time.monotonic() < message_deadline:
            if (
                self.odometry_message is not None
                and self.wheel_speed_message is not None
            ):
                break

            time.sleep(0.05)
        else:
            self.fail(
                "Timed out waiting for decoded chassis messages"
            )

        odometry = self.odometry_message

        self.assertEqual(
            odometry.header.frame_id,
            "odom",
        )
        self.assertEqual(
            odometry.child_frame_id,
            "base_footprint",
        )

        self.assertAlmostEqual(
            odometry.pose.pose.position.x,
            0.2275436371564865,
            places=6,
        )
        self.assertAlmostEqual(
            odometry.pose.pose.position.y,
            0.6472373604774475,
            places=6,
        )
        self.assertAlmostEqual(
            odometry.pose.pose.position.z,
            0.0,
            places=6,
        )

        expected_yaw = 1.82720041275

        self.assertAlmostEqual(
            odometry.pose.pose.orientation.z,
            math.sin(expected_yaw * 0.5),
            places=6,
        )
        self.assertAlmostEqual(
            odometry.pose.pose.orientation.w,
            math.cos(expected_yaw * 0.5),
            places=6,
        )

        expected_pose_covariance = {
            0: 0.01,
            7: 0.01,
            14: 99999.0,
            21: 99999.0,
            28: 99999.0,
            35: 0.01,
        }

        expected_twist_covariance = {
            0: 999999.0,
            7: 999999.0,
            14: 999999.0,
            21: 999999.0,
            28: 999999.0,
            35: 999999.0,
        }

        for index, value in expected_pose_covariance.items():
            self.assertEqual(
                odometry.pose.covariance[index],
                value,
            )

        for index, value in expected_twist_covariance.items():
            self.assertEqual(
                odometry.twist.covariance[index],
                value,
            )

        pose_diagonal_indices = set(
            expected_pose_covariance.keys()
        )
        twist_diagonal_indices = set(
            expected_twist_covariance.keys()
        )

        for index, value in enumerate(
            odometry.pose.covariance
        ):
            if index not in pose_diagonal_indices:
                self.assertEqual(value, 0.0)

        for index, value in enumerate(
            odometry.twist.covariance
        ):
            if index not in twist_diagonal_indices:
                self.assertEqual(value, 0.0)

        self.assertEqual(
            list(self.wheel_speed_message.data),
            [-174, 176, 17, 34, 51, 68],
        )

        healthy_status = self._wait_for_diagnostic(
            lambda status: (
                status.level == DiagnosticStatus.OK
                and status.message
                == "Receiving valid chassis CAN frames"
            )
        )

        healthy_values = self._diagnostic_values(healthy_status)

        self.assertEqual(
            healthy_values["received_frame_count"],
            "3",
        )
        self.assertEqual(
            healthy_values["decoded_frame_count"],
            "3",
        )
        self.assertEqual(
            healthy_values["unknown_frame_count"],
            "0",
        )

        stale_status = self._wait_for_diagnostic(
            lambda status: (
                status.level == DiagnosticStatus.WARN
                and status.message == "Chassis CAN data is stale"
            ),
            timeout=3.0,
        )

        stale_values = self._diagnostic_values(stale_status)

        self.assertEqual(
            stale_values["decoded_frame_count"],
            "3",
        )


@launch_testing.post_shutdown_test()
class TestChassisCanNodeShutdown(unittest.TestCase):

    def test_process_exited_cleanly(
        self,
        proc_info,
        chassis_node,
    ):
        launch_testing.asserts.assertExitCodes(
            proc_info,
            process=chassis_node,
        )
