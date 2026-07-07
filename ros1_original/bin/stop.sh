#!/bin/bash

killall -9 anbotControl

#关闭电源板监控复位功能
ARCH=`dpkg --print-architecture`
if [ "$ARCH" =  "arm64" ];then
echo "---------------"
echo "turn off  maintrol  monitor"
cansend   can0    770#0000000055000000
cansend   can1    770#0000000055000000
cansend   can0    770#0000000055000000
cansend   can1    770#0000000055000000
cansend   can0    770#0000000055000000
cansend   can1    770#0000000055000000
cansend   can0    770#0000000055000000
cansend   can1    770#0000000055000000
cansend   can0    770#0000000055000000
cansend   can1    770#0000000055000000
fi

sh_path="/home/anbot/system/bin/sh/"
sh ${sh_path}kill_maincontrol.sh
sh ${sh_path}killall_ros.sh
sh ${sh_path}kill_movebase.sh


