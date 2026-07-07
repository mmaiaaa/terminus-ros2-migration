cmake_minimum_required(VERSION 2.8.3)
project(anbot_base_control)

add_compile_options(-std=c++11)

find_package(catkin REQUIRED COMPONENTS
  geometry_msgs
  nav_msgs
  roscpp
  rospy
  std_msgs
  sensor_msgs
  anbot_msgs
  tf
)

catkin_package(
  CATKIN_DEPENDS geometry_msgs nav_msgs roscpp rospy std_msgs tf sensor_msgs anbot_msgs
  DEPENDS system_lib
)

include_directories(
	include 
	${catkin_INCLUDE_DIRS} 
	../../../src/public_log/include
	../../../src/public_log/include/log4cplus
)

#x86
IF (${CMAKE_SYSTEM_PROCESSOR} MATCHES "x86_64")

	#x86_64 ubuntu 14.04   4.4.0-31-generic
	IF(${CMAKE_SYSTEM_VERSION} MATCHES "4.4.0-31-generic")
		message("anbot,x86_64,ubuntu 14.04")
	    link_directories(
	        ../../../src/public_log/lib/log4cplus-x64-14.04
			)
			
	#x86_64 ubuntu 16.04   4.13.0-39-generic
	ELSE() 
	   message("anbot,x86_64,ubuntu 16.04")
	   link_directories(
	       ../../../src/public_log/lib/log4cplus-x64-16.04
		   )
		   
	ENDIF()


#arm64
ELSE(${CMAKE_SYSTEM_PROCESSOR} MATCHES "x86_64")

	#arm64 tx2       4.4.38-tegra
	IF(${CMAKE_SYSTEM_VERSION} MATCHES "4.4.38-tegra")
		message("anbot,arm64,tx2,ubuntu 16.04")
	    link_directories(
	        ../../../src/public_log/lib/log4cplus_arm64-tx2
	        )
	        
	#arm64  rk3399     4.4.77
	ELSE()
		message("anbot,arm64,rk3399,ubuntu 16.04")
	    link_directories(
	        ../../../src/public_log/lib/log4cplus_arm64
	        )
	ENDIF()

ENDIF(${CMAKE_SYSTEM_PROCESSOR} MATCHES "x86_64")




add_executable(
							anbot_base_node 
							src/anbot_base_node.cpp 
							src/wheel_control.cpp 
							src/currenttime.cpp 
							src/serial_lib/serial/serial.cc 
							src/serial_lib/serial/impl/unix.cc 
							src/serial_lib/serial/impl/list_ports/list_ports_linux.cc
							src/publicremotelog.cpp
)


target_link_libraries(
						anbot_base_node 
						${catkin_LIBRARIES} 
						log4cplus
)

add_dependencies(anbot_base_node anbot_msgs_gencpp)

install(PROGRAMS
  DESTINATION ${CATKIN_PACKAGE_BIN_DESTINATION}
)

install(TARGETS anbot_base_node
  RUNTIME DESTINATION ${CATKIN_PACKAGE_BIN_DESTINATION}
)

