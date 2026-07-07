#! /bin/sh

cd /home/anbot/log/

rm -f image.tar.gz

mkdir image

cd image/

maxsize=100

fileSize=`ls -lt | awk '{if(NR>1) a+=$5} END {print a/1024/1024}'`
echo $fileSize
if [ $(echo "$fileSize > $maxsize" | bc) -eq 1 ];then
	echo "******************"
	cd ../
	tar czvf image.tar.gz image/
	rm -rf image/*
fi

