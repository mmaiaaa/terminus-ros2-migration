#! /bin/sh

rosnode kill /slam_gmapping 
roslaunch anbot_bringup gmapping.launch.xml &
