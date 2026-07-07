#! /bin/sh

var=`arch`

if [ $var = "x86_64" ]
then
    /home/anbot/system/bin/tmate_x86 -S /home/anbot/tmate_socket show-messages     >  /home/anbot/log/tmate.log  2>&1
fi


if [ $var = "aarch64" ]
then
    /home/anbot/system/bin/tmate_arm64 -S /home/anbot/tmate_socket  show-messages  > /home/anbot/log/tmate.log  2>&1
fi




