#!/bin/bash

#日志存放目录
logPath="/home/anbot/log/" 

echo  "-----------------------"
if [ ! -d "$logPath" ]; then  
	mkdir "$logPath"  
	echo "create logDir succees"
else
	echo "logDir already exists"
fi
echo  "-----------------------"



echo "y" | rosclean purge

COUNT=0

NAME="ros"
ID=`ps -ef | grep "$NAME" | grep -v "$0" | grep -v "grep" | awk '{print $2}'`
echo $ID
for id in $ID
	do
	let COUNT=COUNT+1;
done

echo $COUNT;

if [ $COUNT -gt 0 ];
	then echo "an other system already started!\nprogram while exit!!!"
	exit 0;
fi


#sh   /home/anbot/system/bin/sh/cleanup_ros_log.sh  &

sleep 5

roslaunch anbot_bringup navigation.launch




