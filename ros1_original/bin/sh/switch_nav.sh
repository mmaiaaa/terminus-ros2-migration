#! /bin/sh

rosnode kill /slam_gmapping


NAME="map_server"
echo $NAME
ID=`ps -ef | grep "$NAME" | grep -v "$0" | grep -v "grep" | awk '{print $2}'`
echo $ID
echo "---------------"
for id in $ID
do
kill -9 $id
echo "killed $id"
done
echo "---------------"

#kill  move_base
NAME="move_base"
echo $NAME
ID=`ps -ef | grep "$NAME" | grep -v "$0" | grep -v "grep" | awk '{print $2}'`
echo $ID
echo "---------------"
for id in $ID
do
kill -9 $id
echo "killed $id"
done
echo "---------------"

#kill  all mrpt node
NAME="mrpt_localization_node"
echo $NAME
MRPT_ID=`ps -ef | grep "$NAME" | grep -v "$0" | grep -v "grep" | awk '{print $2}'`
echo $MRPT_ID
echo "---------------"
for id in $MRPT_ID
do
kill -9 $id
echo "killed $id"
done
echo "---------------"

#kill all amcl node
NAME="amcl"
echo $NAME
AMCL_ID=`ps -ef | grep "$NAME" | grep -v "$0" | grep -v "grep" | awk '{print $2}'`
echo $AMCL_ID
echo "---------------"
for id in $AMCL_ID
do
kill -9 $id
echo "killed $id"
done
echo "---------------"

#kill all uwb node
NAME="uwb_localization_node"
echo $NAME
UWB_ID=`ps -ef | grep "$NAME" | grep -v "$0" | grep -v "grep" | awk '{print $2}'`
echo $UWB_ID
echo "---------------"
for id in $UWB_ID
do
kill -9 $id
echo "killed $id"
done
echo "---------------"



echo "y"|rosnode cleanup


#mapserver reload  the map
rosrun map_server map_server /home/anbot/system/bin/src/anbot/anbot_navigation_config/maps/mymap.yaml &


sleep 3
roslaunch anbot_bringup localization.launch &
roslaunch anbot_bringup  move_base.launch.xml &



 

