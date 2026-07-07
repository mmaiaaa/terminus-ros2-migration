cmake_minimum_required(VERSION 2.8.3)
project(anbot_control)

#ADD_DEFINITIONS(-DQT_NO_KEYWORDS)

add_compile_options(-std=c++11)

set (USE_SPI_TO_CAN_MODULE False)


find_package(catkin
    REQUIRED
    COMPONENTS
    geometry_msgs
    message_generation
    rosconsole
    roscpp
    roscpp_serialization
    roslib rostime
    std_msgs std_srvs
    geometry_msgs
    actionlib
    move_base_msgs
    anbot_msgs
    spencer_tracking_msgs # 
    tf
    turtlebot_msgs
    cpp_common
)

find_package(Qt5Widgets REQUIRED Core Gui Network Qt5Sql)
find_package(Boost REQUIRED COMPONENTS thread)



catkin_package(
   INCLUDE_DIRS include
   CATKIN_DEPENDS geometry_msgs  std_msgs std_srvs anbot_msgs spencer_tracking_msgs turtlebot_msgs
   DEPENDS system_lib
)

include_directories(
    include
    #include/log4cplus
    ../public_log/include/
    ../public_log/include/log4cplus
    ../include/
    include/states/
    include/quazip

    ${catkin_INCLUDE_DIRS}
    ${Boost_INCLUDE_DIRS}
    ${Qt5Widgets_INCLUDE_DIRS}
    ${QT_INCLUDE_DIRS}
    ${QT5_INCLUDE_DIRS}
    /usr/include/qt5/
    /usr/include/qt5/QtCore/
    /usr/include/qt5/QtNetwork/
    /opt/qt57/include/
    ../../src/public_log/include
)


#x86
IF (${CMAKE_SYSTEM_PROCESSOR} MATCHES "x86_64")

	#x86_64 ubuntu 14.04   4.4.0-31-generic
	IF(${CMAKE_SYSTEM_VERSION} MATCHES "4.4.0-31-generic")
		message("anbot,x86_64,ubuntu 14.04")
	    link_directories(
	        ../../src/public_log/lib/log4cplus-x64-14.04
	        lib/quazip
			)
			
	#x86_64 ubuntu 16.04   4.13.0-39-generic
	ELSE() 
	   message("anbot,x86_64,ubuntu 16.04")
	   link_directories(
	       ../../src/public_log/lib/log4cplus-x64-16.04
	       lib/quazip
		   )
		   
	ENDIF()


#arm64
ELSE(${CMAKE_SYSTEM_PROCESSOR} MATCHES "x86_64")

	#arm64 tx2       4.4.38-tegra
	IF(${CMAKE_SYSTEM_VERSION} MATCHES "4.4.38-tegra")
		message("anbot,arm64,tx2,ubuntu 16.04")
	    link_directories(
	        ../../src/public_log/lib/log4cplus_arm64-tx2
	        lib/quazip_arm64
	        )
	        
	#arm64  rk3399     4.4.77
	ELSE()
		message("anbot,arm64,rk3399,ubuntu 16.04")
	    link_directories(
	        ../../src/public_log/lib/log4cplus_arm64
	        lib/quazip_arm64
	        )
	ENDIF()

ENDIF(${CMAKE_SYSTEM_PROCESSOR} MATCHES "x86_64")




set(Control_node_SRCS 
    src/main.cpp
    src/anbot.cpp
    src/queue.cpp
	  
    src/setforbidareamanage.cpp
    src/anbotdaemon.cpp
    src/basefunction.cpp
    src/log.cpp
    src/fsmaction.cpp
    src/missionmanage.cpp
    src/automission.cpp
    src/baseparacfg.cpp
    src/basepararead.cpp
    src/cmdinvoker.cpp
    src/states/topctlfsm.cpp
    src/mission/navmission.cpp
    src/mission/chargemission.cpp
    src/mission/oncenavmission.cpp
    src/base_control_intf/navintf.cpp
    src/base_control_intf/devintf.cpp
    src/base_control_intf/timermonitor.cpp
    include/datasync.h
    src/server/server.cpp
    src/server/tcpclient.cpp
    src/server/onlinefsmact.cpp
    src/server/onlinem.cpp
    src/server/onlinem2interactive.cpp
    src/server/msgclient.cpp
    src/server/client2interactive.cpp

    src/fsmframe/anfsmaction.cpp
    src/fsmframe/anstatemachine.cpp

    src/cjson/cJSON.c
    src/common/config.cpp
    src/sensors/sensors.cpp
    src/hardwareselftest.cpp
    src/alarmreport.cpp
    src/mapmanage.cpp
    src/logmanage.cpp
    src/softwareupdate.cpp
    src/encryptdecrypt.cpp
    src/antimer.cpp
    src/convertmaptogps.cpp
    src/devmFsm.cpp
    src/MotorFsm.cpp
    src/steerFsm.cpp
    src/productinfo.cpp
    src/cpuUsage.cpp
    src/avgcalc.cpp
    src/virtualPathway.cpp
    src/SpecialAreaProc.cpp
    src/statistic.cpp
    src/terminalSharing.cpp
)

set(Control_node_HDRS
    include/queue.h
    include/basefunction.h
    include/server/server.h
    include/server/tcpclient.h
    include/server/msgclient.h
    include/server/client2interactive.h
    include/server/onlinem2interactive.h
    include/anfsmaction.h
    include/anstatemachine.h
    include/onlinefsmact.h
    include/onlinem.h
    include/cjson/cJSON.h
    include/verifyutil.h
    include/encryptdecrypt.h
    include/cpuUsage.h
    include/avgcalc.h
    include/statistic.h
)






set(QT_WIDGET_HDRS
    include/log.h
    include/setforbidareamanage.h
    include/virtualPathway.h
    include/SpecialAreaProc.h
    include/anbotret.h
    include/anbot.h
    include/anbotdaemon.h
    include/fsmaction.h
    include/states/topctlfsm.h
    include/automission.h
    include/chargemission.h
    include/navmission.h
    include/missionmanage.h
    include/anfsmaction.h
    include/anstatemachine.h
    include/onlinefsmact.h
    include/onlinem.h
    include/server/server.h
    include/server/client2interactive.h
    include/server/onlinem2interactive.h
    include/baseparacfg.h
    include/basepararead.h
    include/cmdinvoker.h
    include/navintf.h
    include/devintf.h
    include/timermonitor.h
    include/sensors/sensors.h
    include/hardwareselftest.h
    include/alarmreport.h
    include/oncenavmission.h
    include/mapmanage.h
    include/logmanage.h
    include/softwareupdate.h
    include/antimer.h
    include/convertmaptogps.h
    include/server/tcpclient.h
    include/SteerFsm.h
    include/devmFsm.h
    include/MotorFsm.h  
    include/statistic.h
    include/terminalSharing.h
)

qt5_wrap_cpp(QT_WIDGET_HDRS_MOC ${QT_WIDGET_HDRS} )

add_executable(anbotControl
    ${Control_node_SRCS}
    ${Control_node_HDRS}
    ${QT_WIDGET_HDRS_MOC}
    ${SPI_TO_CAN_MODULE_HDRS}
    ${SPI_TO_CAN_MODULE_SRCS}
)

target_link_libraries(anbotControl
    Qt5Network
    Qt5::Widgets
    Qt5Sql
    ${QT_LIBRARIES}
    ${catkin_LIBRARIES}
    ${Boost_LIBRARIES}
    ${OpenCV_LIBS}
    log4cplus
    quazip
    crypto
)

set( CMAKE_BUILD_TYPE Debug )

