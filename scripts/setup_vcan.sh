#!/usr/bin/env bash

set -euo pipefail

INTERFACE="${1:-vcan0}"

if [[ "${EUID}" -ne 0 ]]; then
    echo "ERROR: Run this script with sudo." >&2
    exit 1
fi

modprobe vcan

if ! ip link show "${INTERFACE}" >/dev/null 2>&1; then
    ip link add dev "${INTERFACE}" type vcan
fi

ip link set "${INTERFACE}" up

echo "Virtual CAN interface is ready:"
ip -details link show "${INTERFACE}"
