#!/bin/bash

sh stop.sh
echo "y" | rosclean purge
roscore &
sleep 3
#rosrun can_device can_device_node  &
roslaunch can_device can_device.launch &

