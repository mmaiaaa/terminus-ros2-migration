#! /bin/sh

gmappingMapPath="/home/anbot/system/bin/src/anbot/anbot_navigation_config/gmapping/" 

if [ ! -d "$gmappingMapPath" ]; then  
	mkdir -p "$gmappingMapPath"  
fi

cd $gmappingMapPath

rosrun map_server map_saver -f mymap


#cd /home/anbot/system/bin/src/anbot/anbot_navigation_config/maps/
#dirname=`date`
#mkdir "'$dirname'"
#cp mymap.* "'$dirname'/"
#rosrun map_server map_saver -f gmappingmap
