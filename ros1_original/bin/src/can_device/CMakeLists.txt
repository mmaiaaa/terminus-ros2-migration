cmake_minimum_required(VERSION 2.8.3)
project(can_device)

## Find catkin macros and libraries
## if COMPONENTS list like find_package(catkin REQUIRED COMPONENTS xyz)
## is used, also find other catkin packages

add_compile_options(-std=c++11)

find_package(catkin REQUIRED COMPONENTS
  geometry_msgs
  message_generation
  anbot_msgs
  roscpp
  rospy
  std_msgs
  cpp_common
  tf
)


find_package(Qt5Widgets REQUIRED Core)
find_package(Boost REQUIRED COMPONENTS thread)



catkin_package(
   INCLUDE_DIRS include
   CATKIN_DEPENDS geometry_msgs
   std_msgs
   std_srvs
   anbot_msgs
   turtlebot_msgs
   DEPENDS system_lib
)


include_directories(
  include
  ../include
  ${catkin_INCLUDE_DIRS}
  ${Boost_INCLUDE_DIRS}
  ${Qt5Widgets_INCLUDE_DIRS}
  ${QT_INCLUDE_DIRS}
  ../../src/public_log/include
)


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




set(can_device_SRCS 
    src/sensor/sensor.cpp
    src/sensor/com_sensor.cpp
    src/detect/detect.cpp
    src/can/cancomm.cpp
    src/chassis/chassis.cpp
    src/chassis/com_chassis.cpp
    src/chassis/ComChassisFDC.cpp
    src/battery/battery.cpp
    src/publicremotelog.cpp
    src/aoa/AOACom.cpp
    src/powerboard/powerboard.cpp
    src/waterCannonPtz/waterCannonPtz.cpp
)

set(can_device_HDRS
    include/sensor/sensor.h
    include/sensor/com_sensor.h
    include/sensor/sensor_macro.h
    include/detect/detect.h
    include/detect/detect_macro.h
    include/chassis/chassis.h
    include/chassis/com_chassis.h
    include/battery/battery.h
    include/battery/dx_battery.h
    include/battery/jgne_battery.h
    include/battery/jgne_battery.cpp
    include/aoa/AOACom.h
    include/powerboard/powerboard.h
    include/waterCannonPtz/waterCannonPtz.h
)

set(SPI_TO_CAN_MODULE_HDRS
    include/can/cancomm.h
	include/can/mcp2515.h
	include/can/spi.h
	include/can/MCP2515_REG.h
)

set(SPI_TO_CAN_MODULE_SRCS
	src/can/mcp2515.cpp
	src/can/spi.cpp
)


set(SERIAL_HDRS
    lib/serial_lib/serial/serial.h
    lib/serial_lib/serial/v8stdint.h
    lib/serial_lib/serial/impl/unix.h
)


set(SERIAL_SRCS
        lib/serial_lib/serial/serial.cc
        lib/serial_lib/serial/impl/unix.cc
        lib/serial_lib/serial/impl/list_ports/list_ports_linux.cc
)



set(QT_WIDGET_HDRS
)


qt5_wrap_cpp(QT_WIDGET_HDRS_MOC ${QT_WIDGET_HDRS})


add_executable(can_device_node
    src/can_device_node.cpp
    src/basefunction.cpp
    include/basefunction.h
    src/StateSemaphore.cpp
    include/StateSemaphore.h
    lib/can/controlcan.h

    ${can_device_SRCS}
    ${can_device_HDRS}
    ${SPI_TO_CAN_MODULE_HDRS}
    ${SPI_TO_CAN_MODULE_SRCS}
    ${QT_WIDGET_HDRS_MOC}
    ${SERIAL_HDRS}
    ${SERIAL_SRCS}
)



 IF(${CMAKE_SYSTEM_PROCESSOR} MATCHES "x86_64")
    target_link_libraries(can_device_node
		Qt5::Widgets
		log4cplus
		${QT_LIBRARIES}
		${catkin_LIBRARIES}
		${Boost_LIBRARIES}
		${OpenCV_LIBS}
		${PROJECT_SOURCE_DIR}/lib/can/lib/linux_x64/libcontrolcan.so
    )
 ELSE() 
	target_link_libraries(can_device_node
		Qt5::Widgets
		log4cplus
		${QT_LIBRARIES}
		${catkin_LIBRARIES}
		${Boost_LIBRARIES}
		${OpenCV_LIBS}
        ${PROJECT_SOURCE_DIR}/lib/can/lib/linux_arm/libcontrolcan.so
	)
 ENDIF()


set(CMAKE_BUILD_TYPE Debug)




