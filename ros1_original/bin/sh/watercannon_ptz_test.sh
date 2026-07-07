#! /bin/bash

echo 123456 | sudo -S ifconfig can0 down
echo 123456 | sudo -S ifconfig can1 down
sleep  1

echo 123456 | sudo -S ip link set can0 type can bitrate 500000
echo 123456 | sudo -S ip link set can1 type can bitrate 500000
sleep  1

echo 123456 | sudo -S ip link set can0 up type can
echo 123456 | sudo -S ip link set can1 up type can
sleep  1


sh   watercannon_cmd.sh    &
sh   ptz_cmd.sh   &


:<<EOF
while true
do
    #date
    #sleep 1

    echo  "water cannon  test   start"
    cansend    can0  356#0103000001030002     #水炮上
    sleep   10
    cansend    can0  356#0203000002030003    #水炮下    
    sleep   10
   
   # cansend    can0  356#0103000003030000     #水炮左
   # sleep   10
   # cansend    can0  356#0203000003030000       #水炮右
   # sleep   10
   #   cansend    can0  356#0000000000000002       #开启水炮
   # sleep    1
   # cansend    can0  356#0000000000000003       #关闭水炮
   # sleep    1
    
    echo  "water cannon  test   end"



    echo  "ptz  test   start"
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

    cansend  can0  357#FF010000000001         #云台停
    sleep    10

    cansend  can0  357#FF01000B00020E        #云台补光灯关
    sleep    5
    cansend  can0  357#FF01000900020C        #云台补光灯开
    sleep    5

    cansend  can0  357#FF010003003D41        #云台雨刷灯开
    sleep    2
    cansend  can0  357#FF01000700333B        #云台雨刷灯关
    sleep    2

    echo  "ptz  test   end"
done

EOF

