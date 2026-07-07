# Software License Agreement (BSD License)
#
#  Copyright (c) 2014-2016, Timm Linder, Fabian Girrbach, Social Robotics Lab, University of Freiburg
#  All rights reserved.
# catkin_make  -DCMAKE_BUILD_TYPE=Debug --pkg legs2dscan_immekf_gnntracker
# 
cmake_minimum_required(VERSION 2.8.3)
# project(srl_nearest_neighbor_tracker)
project(legs2dscan_immekf_gnntracker)
# SET(CMAKE_BUILD_TYPE "Debug")
# SET(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g -ggdb")
add_definitions("-Wall -g -rdynamic")
 # target system does not support the ‘stabs’ debug format
# add_compile_options(-std=c++11)
set(CMAKE_CXX_FLAGS "-std=c++11 ${CMAKE_CXX_FLAGS}")

find_package(catkin REQUIRED COMPONENTS roscpp tf eigen_conversions tf_conversions geometry_msgs sensor_msgs spencer_tracking_msgs cmake_modules angles )
# spencer_diagnostics
# srl_laser_segmentation) # now ' spencer_tracking_msgs::LaserscanSegmentation'msg'

find_package(Eigen REQUIRED)
find_package(Boost REQUIRED COMPONENTS thread) # SIGSEGV
# include_directories()

catkin_package()

if(NOT CMAKE_BUILD_TYPE MATCHES Debug)
  # Disable debug and info-level logging statements in release mode
  add_definitions(-DROSCONSOLE_MIN_SEVERITY=ROSCONSOLE_SEVERITY_WARN)
else()
  add_definitions(-DROSCONSOLE_MIN_SEVERITY=ROSCONSOLE_SEVERITY_DEBUG)
endif()

include_directories(include
 ../../src/public_log/include
 # ../public_log/include
 /usr/include/eigen3
 ${Eigen_INCLUDE_DIRS}
 ${Boost_INCLUDE_DIRS}
 ${catkin_INCLUDE_DIRS}
 )
link_directories(${catkin_LIBRARY_DIRS})
set(SOURCE_DIR src)
# /lidarxy_immekf_nntracker

#x86
IF (${CMAKE_SYSTEM_PROCESSOR} MATCHES "x86_64")
	#x86_64 ubuntu 14.04   4.4.0-31-generic
	IF(${CMAKE_SYSTEM_VERSION} MATCHES "4.4.0-31-generic")
		message("anbot,x86_64,ubuntu 14.04")
	    link_directories(
	        ../../src/public_log/lib/log4cplus-x64-14.04
			)
	#x86_64 ubuntu 16.04   4.13.0-39-generic
	ELSE() 
	   message("anbot,x86_64,ubuntu 16.04")
	   link_directories(
	       ../../src/public_log/lib/log4cplus-x64-16.04
		   )
	ENDIF()

#arm64
ELSE(${CMAKE_SYSTEM_PROCESSOR} MATCHES "x86_64")
	#arm64 tx2       4.4.38-tegra
	IF(${CMAKE_SYSTEM_VERSION} MATCHES "4.4.38-tegra")
		message("anbot,arm64,tx2,ubuntu 16.04")
	    link_directories(
	        ../../src/public_log/lib/log4cplus_arm64-tx2
	        )
	#arm64  rk3399     4.4.77
	ELSE()
		message("anbot,arm64,rk3399,ubuntu 16.04")
	    link_directories(
	        ../../src/public_log/lib/log4cplus_arm64
	        )
	ENDIF()

ENDIF(${CMAKE_SYSTEM_PROCESSOR} MATCHES "x86_64")

set(TRACKER_SOURCES
    ${SOURCE_DIR}/geometry_ros_utils.cpp
    ${SOURCE_DIR}/ros_params.cpp
    # ${SOURCE_DIR}/ros/ros_interface.cpp # wn' main()
    ${SOURCE_DIR}/ekf.cpp
    ${SOURCE_DIR}/nearest_neighbor_tracker.cpp
    ${SOURCE_DIR}/logic_initiator.cpp
    ${SOURCE_DIR}/imm_filter.cpp

    ${SOURCE_DIR}/occlusion_handling/laser_shade_occlusion_manager.cpp
    ${SOURCE_DIR}/occlusion_handling/basic_occlusion_manager.cpp
    ${SOURCE_DIR}/occlusion_handling/polygon_occlusion_manager.cpp

    ${SOURCE_DIR}/basic_nearest_neighbor_data_association.cpp
    ${SOURCE_DIR}/global_nearest_neighbor_data_association.cpp

    ${SOURCE_DIR}/missed_lowconfidence_obs_recovery.cpp
)

add_executable(ekfgnnlegs_maintracknode ${SOURCE_DIR}/threeCb_sterilizer_static_obj2dtracks_main_ros_interface.cpp ${TRACKER_SOURCES})
# add_executable(ekfgnnlegs_maintracknode ${SOURCE_DIR}/syncAbcTopicNameChk_main_ros_interface.cpp  ${TRACKER_SOURCES})
add_dependencies(ekfgnnlegs_maintracknode ${catkin_EXPORTED_TARGETS})
target_link_libraries(ekfgnnlegs_maintracknode
 # log4cplus
 ${catkin_LIBRARIES}
 ${Boost_LIBRARIES})

# add_executable(srlobj_logger_mainnode ${SOURCE_DIR}/ros/nodes/logger_node.cpp)
# add_dependencies(srlobj_logger_mainnode ${catkin_EXPORTED_TARGETS})
# target_link_libraries(srlobj_logger_mainnode ${catkin_LIBRARIES})

install(TARGETS ekfgnnlegs_maintracknode # srlobj_logger_node
  ARCHIVE DESTINATION ${CATKIN_PACKAGE_LIB_DESTINATION}
  LIBRARY DESTINATION ${CATKIN_PACKAGE_LIB_DESTINATION}
  RUNTIME DESTINATION ${CATKIN_PACKAGE_BIN_DESTINATION}
)

install(DIRECTORY include/${PROJECT_NAME}/
  DESTINATION ${CATKIN_PACKAGE_INCLUDE_DESTINATION}
  FILES_MATCHING PATTERN "*.h"
  PATTERN ".svn" EXCLUDE
)

install(DIRECTORY launch/
  DESTINATION ${CATKIN_PACKAGE_SHARE_DESTINATION}/launch
  PATTERN ".svn" EXCLUDE
)
