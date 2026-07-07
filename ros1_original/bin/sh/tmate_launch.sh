#! /bin/sh

var=`arch`

if [ $var = "x86_64" ]
then
    cd /home/anbot
    gnome-terminal -x  /home/anbot/system/bin/tmate_x86 -S /home/anbot/tmate_socket
fi


if [ $var = "aarch64" ]
then
   cd /home/anbot
   gnome-terminal -x  /home/anbot/system/bin/tmate_arm64 -S /home/anbot/tmate_socket
fi




