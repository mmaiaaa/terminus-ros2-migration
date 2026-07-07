cmake_minimum_required(VERSION 2.8.3)
project(move_base)
add_compile_options(-std=c++11)
find_package(catkin REQUIRED
    COMPONENTS
        cv_bridge
        image_transport
        sensor_msgs
        std_msgs
        cmake_modules
        roscpp
        pluginlib
        actionlib
        dynamic_reconfigure
        message_generation
        nav_core
        tf
)
find_package(Eigen REQUIRED)
add_definitions(${EIGEN_DEFINITIONS})

# dynamic reconfigure
generate_dynamic_reconfigure_options(
    cfg/MoveBase.cfg
)

catkin_package(
    CATKIN_DEPENDS
        roscpp
        dynamic_reconfigure
)

include_directories(
  include 
  include/move_base 
  include/log4cplus
  ${catkin_INCLUDE_DIRS}
)



include_directories(
    include
    ../../../../src/public_log/include
    ../../../include/
    ${catkin_INCLUDE_DIRS}
    ${EIGEN_INCLUDE_DIRS}
)



#link_directories(   
#    ../../../../src/public_log/lib/log4cplus
#)


#x86
IF (${CMAKE_SYSTEM_PROCESSOR} MATCHES "x86_64")

	#x86_64 ubuntu 14.04   4.4.0-31-generic
	IF(${CMAKE_SYSTEM_VERSION} MATCHES "4.4.0-31-generic")
		message("anbot,x86_64,ubuntu 14.04")
	    link_directories(
	        ../../../../src/public_log/lib/log4cplus-x64-14.04
			)
			
	#x86_64 ubuntu 16.04   4.13.0-39-generic
	ELSE() 
	   message("anbot,x86_64,ubuntu 16.04")
	   link_directories(
	       ../../../../src/public_log/lib/log4cplus-x64-16.04
		   )
		   
	ENDIF()


#arm64
ELSE(${CMAKE_SYSTEM_PROCESSOR} MATCHES "x86_64")

	#arm64 tx2       4.4.38-tegra
	IF(${CMAKE_SYSTEM_VERSION} MATCHES "4.4.38-tegra")
		message("anbot,arm64,tx2,ubuntu 16.04")
	    link_directories(
	        ../../../../src/public_log/lib/log4cplus_arm64-tx2
	        )
	        
	#arm64  rk3399     4.4.77
	ELSE()
		message("anbot,arm64,rk3399,ubuntu 16.04")
	    link_directories(
	        ../../../../src/public_log/lib/log4cplus_arm64
	        )
	ENDIF()

ENDIF(${CMAKE_SYSTEM_PROCESSOR} MATCHES "x86_64")



# move_base
add_library(move_base
  src/vfhplus.cpp 
  src/WorldFunction.cpp
  src/FindVoroAndDistPath.cpp
  src/CconstFindPath.cpp
  src/GvdMapAction.cpp
  src/move_base.cpp
  src/SpcDataSrv.cpp
)
target_link_libraries(move_base
    ${Boost_LIBRARIES}
    ${catkin_LIBRARIES}
    log4cplus
    )
add_dependencies(move_base move_base_gencfg)
add_dependencies(move_base geometry_msgs_gencpp)

add_executable(move_base_node
  src/move_base_node.cpp
)
target_link_libraries(move_base_node log4cplus move_base)
set_target_properties(move_base_node PROPERTIES OUTPUT_NAME move_base)

install(DIRECTORY launch
    DESTINATION ${CATKIN_PACKAGE_SHARE_DESTINATION}
    USE_SOURCE_PERMISSIONS
    )

install(
    PROGRAMS
       scripts/subtopic_forwarder.py
       scripts/subtopic_forwarder_node.py
       scripts/warner.py
    DESTINATION ${CATKIN_PACKAGE_BIN_DESTINATION}
)

install(
    TARGETS
        move_base
        move_base_node
    ARCHIVE DESTINATION ${CATKIN_PACKAGE_LIB_DESTINATION}
    LIBRARY DESTINATION ${CATKIN_PACKAGE_LIB_DESTINATION}
    RUNTIME DESTINATION ${CATKIN_PACKAGE_BIN_DESTINATION}
)

## Mark cpp header files for installation
install(DIRECTORY include/${PROJECT_NAME}/
  DESTINATION ${CATKIN_PACKAGE_INCLUDE_DESTINATION}
  FILES_MATCHING PATTERN "*.h"
)
