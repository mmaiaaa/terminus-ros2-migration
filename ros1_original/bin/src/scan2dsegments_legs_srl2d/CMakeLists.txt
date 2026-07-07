cmake_minimum_required(VERSION 2.8.3)
project(scan2dsegments_legs_srl2d)
# incl' zermaspcl3dnongrd_srl2dsegments
# add_compile_options(-std=c++11)
# SET(CMAKE_CXX_FLAGS "-std=c++11 -O2 -g -Wall ${CMAKE_CXX_FLAGS}")
set( CMAKE_CXX_FLAGS "-std=c++11 -O0" )

find_package(catkin REQUIRED COMPONENTS
  message_filters
  # nodelet
  roscpp
  # roslib
  sensor_msgs
  std_msgs
  geometry_msgs
  spencer_tracking_msgs
  # std_msgs
  # tf2_sensor_msgs
  tf2_ros
  tf2
  # velodyne_pointcloud
  # pcl_conversions pcl_ros
  # eigen_conversions
  # visualization_msgs
  # message_generation
)
find_package(Eigen3 REQUIRED)
find_package(OpenCV REQUIRED)
find_package(PCL REQUIRED)
add_definitions(${PCL_DEFINITIONS})


# add_message_files(FILES  LaserscanSegment.msg   LaserscanSegmentation.msg) # now'wn'spencer_tracking_msgs

# generate_messages( DEPENDENCIES std_msgs sensor_msgs )

catkin_package(
  CATKIN_DEPENDS roscpp message_filters sensor_msgs std_msgs geometry_msgs spencer_tracking_msgs tf2 tf2_ros )
  # velodyne_pointcloud)
#   INCLUDE_DIRS include   LIBRARIES pclmsg_srl2d_segments
  # CATKIN_DEPENDS roscpp message_filters nodelet sensor_msgs geometry_msgs spencer_tracking_msgs tf2 tf2_ros # roscpp
  # CATKIN_DEPENDS roscpp message_filters nodelet std_msgs sensor_msgs tf2 tf2_ros # roscpp
#   message_runtime )

include_directories(
  include   ${catkin_INCLUDE_DIRS}
 /usr/include/eigen3
  ${OpenCV_INCLUDE_DIRS}
)
# link_directories(${PCL_LIBRARY_DIRS})

add_executable(scan2dsegments_legs_srl2d_node src/scan2d_srl2d_segments_node.cpp src/scan2dsegments_features_cvforest_srl2d_exclclosetiny_impl.cpp src/extendedjumpdistance_pntsneighbors_segm.cpp)
target_link_libraries(scan2dsegments_legs_srl2d_node ${catkin_LIBRARIES} ${OpenCV_LIBRARIES} ${PCL_LIBRARIES})


# was'nodelet' add_library(pclmsg_srl2d_segments src/pclmsg_srl2d_segments_nodelet.cpp)
# add_library(pclmsg_srl2d_segments src/pclmsg_srl2d_segments_nodelet.cpp src/extendedjumpdistance_segm.cpp)
# target_link_libraries(pclmsg_srl2d_segments ${catkin_LIBRARIES})

install(TARGETS scan2dsegments_legs_srl2d_node
# install(TARGETS rslidar16_pclmsg_srl2d_segments_node vlp16_pclmsg_srl2d_segments_node
	RUNTIME DESTINATION ${CATKIN_PACKAGE_BIN_DESTINATION}
	LIBRARY DESTINATION ${CATKIN_PACKAGE_LIB_DESTINATION}
	ARCHIVE DESTINATION ${CATKIN_PACKAGE_LIB_DESTINATION})

install(DIRECTORY include/${PROJECT_NAME}/
  DESTINATION ${CATKIN_PACKAGE_INCLUDE_DESTINATION}
)

install(DIRECTORY launch
  DESTINATION ${CATKIN_PACKAGE_SHARE_DESTINATION}
)

if(CATKIN_ENABLE_TESTING)
  find_package(roslint REQUIRED)
  roslint_cpp()
  roslint_add_test()
endif()

