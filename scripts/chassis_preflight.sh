#!/usr/bin/env bash

set -u

REPOSITORY_ROOT="$(
    cd "$(dirname "${BASH_SOURCE[0]}")/.." &&
    pwd
)"

WORKSPACE="${REPOSITORY_ROOT}/ros2_ws"
ROS_SETUP="/opt/ros/jazzy/setup.bash"
WORKSPACE_SETUP="${WORKSPACE}/install/setup.bash"

CAN_INTERFACE="${1:-can0}"

FAILURES=0
WARNINGS=0

pass()
{
    printf 'PASS: %s\n' "$1"
}

warn()
{
    printf 'WARN: %s\n' "$1"
    WARNINGS=$((WARNINGS + 1))
}

fail()
{
    printf 'FAIL: %s\n' "$1" >&2
    FAILURES=$((FAILURES + 1))
}

echo "Terminus chassis read-only preflight"
echo "CAN interface: ${CAN_INTERFACE}"
echo

if [[ -f "${ROS_SETUP}" ]]; then
    pass "ROS 2 Jazzy setup exists"
    set +u
source "${ROS_SETUP}"
set -u
else
    fail "ROS 2 Jazzy setup is missing: ${ROS_SETUP}"
fi

if command -v ros2 >/dev/null 2>&1; then
    pass "ros2 command is available"
else
    fail "ros2 command is unavailable"
fi

if command -v ip >/dev/null 2>&1; then
    pass "ip command is available"
else
    fail "ip command is unavailable"
fi

if command -v candump >/dev/null 2>&1; then
    pass "candump command is available"
else
    warn "candump is unavailable; install can-utils"
fi

if [[ -f "${WORKSPACE_SETUP}" ]]; then
    pass "ROS 2 workspace has been built"
else
    fail "Workspace setup is missing: ${WORKSPACE_SETUP}"
fi

if ip link show "${CAN_INTERFACE}" >/dev/null 2>&1; then
    pass "CAN interface exists: ${CAN_INTERFACE}"
else
    fail "CAN interface does not exist: ${CAN_INTERFACE}"
fi

if ip link show "${CAN_INTERFACE}" 2>/dev/null |
    grep -qE 'state UP|<[^>]*UP[^>]*>'; then
    pass "CAN interface is up"
else
    fail "CAN interface is not up"
fi

INTERFACE_DETAILS="$(
    ip -details link show "${CAN_INTERFACE}" 2>/dev/null || true
)"

if grep -qiE 'vcan|can state|link/can' <<<"${INTERFACE_DETAILS}"; then
    pass "Interface appears to be CAN or VCAN"
else
    warn "Unable to confirm CAN/VCAN interface type"
fi

if [[ -f "${WORKSPACE_SETUP}" && -f "${ROS_SETUP}" ]]; then
    set +u
source "${WORKSPACE_SETUP}"
set -u

    if ros2 pkg executables anbot_chassis_driver 2>/dev/null |
        grep -q 'anbot_chassis_can_node'; then
        pass "Chassis CAN executable is installed"
    else
        fail "Chassis CAN executable is not installed"
    fi

    if ros2 node list 2>/dev/null |
        grep -qx '/anbot_chassis_can_node'; then
        fail "A chassis CAN node is already running"
    else
        pass "No duplicate chassis CAN node is running"
    fi
fi

SOURCE_FILE="$(
    printf '%s' \
        "${WORKSPACE}/src/anbot_chassis_driver/" \
        "src/chassis_can_node.cpp"
)"

if [[ -f "${SOURCE_FILE}" ]] &&
    grep -q 'Listening read-only' "${SOURCE_FILE}"; then
    pass "Source identifies the node as read-only"
else
    warn "Could not verify the read-only log marker"
fi

echo
echo "Preflight summary:"
echo "  Failures: ${FAILURES}"
echo "  Warnings: ${WARNINGS}"

if (( FAILURES > 0 )); then
    exit 1
fi

echo
echo "Preflight passed."
