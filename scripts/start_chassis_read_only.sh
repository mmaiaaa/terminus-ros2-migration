#!/usr/bin/env bash

set -euo pipefail

REPOSITORY_ROOT="$(
    cd "$(dirname "${BASH_SOURCE[0]}")/.." &&
    pwd
)"

WORKSPACE="${REPOSITORY_ROOT}/ros2_ws"
ROS_SETUP="/opt/ros/jazzy/setup.bash"
WORKSPACE_SETUP="${WORKSPACE}/install/setup.bash"

CAN_INTERFACE="${1:-can0}"

"${REPOSITORY_ROOT}/scripts/chassis_preflight.sh" \
    "${CAN_INTERFACE}"

set +u
source "${ROS_SETUP}"
set -u
set +u
source "${WORKSPACE_SETUP}"
set -u

exec ros2 launch \
    anbot_chassis_driver \
    chassis_can.launch.py \
    can_interface:="${CAN_INTERFACE}"
