#!/usr/bin/env bash

set -euo pipefail

REPOSITORY_ROOT="$(
    cd "$(dirname "${BASH_SOURCE[0]}")/.." &&
    pwd
)"

WORKSPACE="${REPOSITORY_ROOT}/ros2_ws"
ROS_SETUP="/opt/ros/jazzy/setup.bash"

if [[ ! -f "${ROS_SETUP}" ]]; then
    echo "ERROR: ROS 2 Jazzy was not found at ${ROS_SETUP}" >&2
    exit 1
fi

if [[ ! -d "${WORKSPACE}/src" ]]; then
    echo "ERROR: ROS 2 workspace was not found at ${WORKSPACE}" >&2
    exit 1
fi

set +u
source "${ROS_SETUP}"
set -u

cd "${WORKSPACE}"

colcon build \
    --packages-select \
        anbot_chassis_protocol \
        anbot_chassis_transport \
        anbot_chassis_driver \
        anbot_model_config \
    --symlink-install \
    --event-handlers console_direct+

echo
echo "ROS 2 workspace build completed successfully."
