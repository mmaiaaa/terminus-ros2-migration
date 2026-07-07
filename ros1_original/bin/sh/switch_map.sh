#! /bin/sh

#kill  all mrpt node
NAME="mrpt_localization_node"
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


NAME="anbot_base_node"
echo $NAME
ID=`ps -ef | grep "$NAME" | grep -v "$0" | grep -v "grep" | awk '{print $2}'`
echo $ID
echo "---------------"
for id in $ID
do
#kill -9 $id
echo "killed $id"
done
echo "---------------"

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



echo "y"|rosnode cleanup

roslaunch anbot_bringup gmapping.launch.xml &
#roslaunch anbot_bringup makemap.launch
#rosrun move_base move_base &
roslaunch anbot_bringup mapping_move_base.launch & 


