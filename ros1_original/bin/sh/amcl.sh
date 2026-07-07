#! /bin/sh

 
export PATH=/opt/ros/indigo/bin:/usr/lib/lightdm/lightdm:/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin:/usr/games
source /home/anbot/system/bin/devel/setup.bash
export LD_LIBRARY_PATH=/home/anbot/system/bin/devel/lib:/opt/ros/indigo/lib:/opt/ros/indigo/lib/python2.7/dist-packages:/opt/ros/indigo/lib/python2.7/dist-packages:/opt/ros/indigo/lib/python2.7/dist-packages && /opt/ros/indigo/bin/roslaunch anbot_bringup anbot_navigation.launch


