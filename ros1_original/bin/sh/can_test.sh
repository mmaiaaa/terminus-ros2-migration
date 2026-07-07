#!/bin/bash

LOG_PATH=/home/anbot/can_test.log
date   >>   ${LOG_PATH}
echo  "can  test   start"  >> ${LOG_PATH}


echo 123456 | sudo -S ifconfig can0 down
echo 123456 | sudo -S ifconfig can1 down
sleep  1

echo 123456 | sudo -S ip link set can0 type can bitrate 500000
echo 123456 | sudo -S ip link set can1 type can bitrate 500000
sleep  1

echo 123456 | sudo -S ip link set can0 up type can
echo 123456 | sudo -S ip link set can1 up type can
sleep  1



gnome-terminal -x bash -c 'candump  can0  -t  A'
sleep   1


gnome-terminal -x bash -c 'candump  can1   -t A'
sleep   1


#echo "can0   send  data"   >>   ${LOG_PATH}
cansend   can0  100#0102030405060708
cansend   can0  200#0102030405060708
cansend   can0  300#0102030405060708
cansend   can0  400#0102030405060708
cansend   can0  500#0102030405060708
cansend   can0  600#0102030405060708
cansend   can0  700#0102030405060708
cansend   can0  701#0102030405060708
cansend   can0  702#0102030405060708
cansend   can0  703#0102030405060708


#echo "can1   send  data"   >> ${LOG_PATH}
cansend   can1  100#0102030405060708
cansend   can1  200#0102030405060708
cansend   can1  300#0102030405060708
cansend   can1  400#0102030405060708
cansend   can1  500#0102030405060708
cansend   can1  600#0102030405060708
cansend   can1  700#0102030405060708
cansend   can1  701#0102030405060708
cansend   can1  702#0102030405060708
cansend   can1  703#0102030405060708



