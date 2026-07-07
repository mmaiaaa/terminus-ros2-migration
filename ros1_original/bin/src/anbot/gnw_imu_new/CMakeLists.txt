cmake_minimum_required(VERSION 2.8.3)
project(gnw_imu_new)
add_compile_options(-std=c++11)


find_package(catkin REQUIRED COMPONENTS
  roscpp
  sensor_msgs
  geometry_msgs
  std_msgs
  tf
)

find_package(Boost REQUIRED COMPONENTS system)

catkin_package(
  INCLUDE_DIRS include
# LIBRARIES 
  CATKIN_DEPENDS roscpp sensor_msgs 
  DEPENDS system_lib
)

set(gnw_imu_SRCS
   src/imu_gnw.cpp
   src/uart.cpp

)

set(gnw_imu_HDRS
   include/imu_gnw.h
   include/uart.h
   include/common.h
    
)


include_directories(
  ${catkin_INCLUDE_DIRS}
)

add_executable(gnw_imu_new_node ${gnw_imu_HDRS} ${gnw_imu_SRCS})
target_link_libraries(gnw_imu_new_node ${catkin_LIBRARIES})


