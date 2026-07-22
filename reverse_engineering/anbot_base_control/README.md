# ANBOT Base Control Recovery Evidence

This directory preserves binary and debug evidence recovered from the original
ROS 1 Kinetic ANBOT system.

## Original platform

- Ubuntu 16.04 Xenial
- ROS Kinetic
- AArch64
- Original package: anbot_base_control

## Preserved components

- anbot_base_node executable
- anbot_base_node.cpp.o
- wheel_control.cpp.o
- currenttime.cpp.o
- publicremotelog.cpp.o
- Wheel_Control DWARF output
- Wheel_Control disassembly output

## Recovery rules

1. Files in this directory are evidence and must not be edited.
2. Reconstructed ROS 2 code must be stored under ros2_ws/src.
3. Guessed protocol values must be clearly marked as unverified.
4. Hardware transmission must remain disabled until decoder and mock tests pass.
