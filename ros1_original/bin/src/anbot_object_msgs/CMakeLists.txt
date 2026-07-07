
cmake_minimum_required(VERSION 2.8.3)
project(anbot_object_msgs)

find_package(catkin REQUIRED COMPONENTS dynamic_reconfigure actionlib_msgs std_msgs geometry_msgs std_srvs sensor_msgs message_generation)

add_message_files(FILES Cluster.msg ClusterArray.msg Track.msg TrackArray.msg FloorCoeffs.msg CommandData.msg)

add_service_files(FILES	LearnModel.srv UseModel.srv SaveMap.srv DumpGraph.srv)

#add_action_files( DIRECTORY action FILES)

#generate_dynamic_reconfigure_options( dynamic_reconfigure/Joystick.cfg )

generate_messages(DEPENDENCIES actionlib_msgs std_msgs geometry_msgs std_srvs sensor_msgs)

catkin_package(CATKIN_DEPENDS dynamic_reconfigure actionlib_msgs std_msgs geometry_msgs std_srvs sensor_msgs message_runtime)

# install(DIRECTORY dynamic_reconfigure DESTINATION ${CATKIN_PACKAGE_SHARE_DESTINATION})
