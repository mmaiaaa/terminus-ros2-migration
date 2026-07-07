#!/bin/bash


echo  "ptz  test   start"

while  true
do
    cansend  can0  357#FF01004B00004C
    sleep  1
    cansend   can0  357#FF01004D00004E
    sleep  1

    cansend  can0  357#FF01004B00004C         #云台归中
    sleep   10
    cansend  can0  357#FF01004B00004C         #云台归中
    sleep   10
    cansend  can0  357#FF01000800141D         #云台上
    sleep   10
    cansend  can0  357#FF010010001425         #云台下
    sleep   10
    cansend  can0  357#FF010004140019         #云台左
    sleep    10
    cansend  can0  357#FF010002140017         #云台右
    sleep    10

    #cansend  can0  357#FF010000000001         #云台停
    #sleep    10

    cansend  can0  357#FF01000B00020E        #云台补光灯关
    sleep    5
    cansend  can0  357#FF01000900020C        #云台补光灯开
    sleep    5

    #cansend  can0  357#FF010003003D41        #云台雨刷灯开
    #sleep    2
    #cansend  can0  357#FF01000700333B        #云台雨刷灯关
    #sleep    2
done 




