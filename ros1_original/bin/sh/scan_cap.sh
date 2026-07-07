#! /bin/sh
#激光雷达数据抓包

CURRENT_TIME=$(date "+%y-%m-%d-%H_%M_%S")
PASSWORD="123456"   						#超级用户密码
LASER_IP="192.168.113.107" 				#激光雷达的IP地址
FILEPATH="/home/anbot/log/"        #数据存放目录

NETDEVICE="eth0"					#当前机器人用网卡号

var=`arch`
#echo "$var"

if [ $var = "x86_64" ]
then
    NETDEVICE="enp4s0" 
    PASSWORD="1"
fi


#抓包
echo $PASSWORD | sudo -S timeout 10 tcpdump -i $NETDEVICE host ${LASER_IP} -C 50  -Z root -w  ${FILEPATH}scan_raw.pcap 


