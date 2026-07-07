# build the driver node

add_compile_options(-std=c++11)

add_executable(velodyne_node velodyne_node.cc driver.cc)
add_dependencies(velodyne_node velodyne_driver_gencfg)
target_link_libraries(velodyne_node
  velodyne_input
  ${catkin_LIBRARIES}
  ${libpcap_LIBRARIES}
)


include_directories(
    /home/anbot/system/bin/src/public_log/include/
    /home/anbot/system/bin/src/public_log/include/log4cplus
    ${catkin_INCLUDE_DIRS}
)


#x86
IF (${CMAKE_SYSTEM_PROCESSOR} MATCHES "x86_64")

	#x86_64 ubuntu 14.04   4.4.0-31-generic
	IF(${CMAKE_SYSTEM_VERSION} MATCHES "4.4.0-31-generic")
		message("anbot,x86_64,ubuntu 14.04")
	    link_directories(
	       /home/anbot/system/bin/src/public_log/lib/log4cplus-x64-14.04
			)
			
	#x86_64 ubuntu 16.04   4.13.0-39-generic
	ELSE() 
	   message("anbot,x86_64,ubuntu 16.04")
	   link_directories(
	       /home/anbot/system/bin/src/public_log/lib/log4cplus-x64-16.04
		   )
		   
	ENDIF()

#arm64
ELSE(${CMAKE_SYSTEM_PROCESSOR} MATCHES "x86_64")

	#arm64 tx2       4.4.38-tegra
	IF(${CMAKE_SYSTEM_VERSION} MATCHES "4.4.38-tegra")
		message("anbot,arm64,tx2,ubuntu 16.04")
	    link_directories(
	        /home/anbot/system/bin/src/public_log/lib/log4cplus_arm64-tx2
	        )
	        
	#arm64  rk3399     4.4.77
	ELSE()
		message("anbot,arm64,rk3399,ubuntu 16.04")
	    link_directories(
	        /home/anbot/system/bin/src/public_log/lib/log4cplus_arm64
	        )
	ENDIF()

ENDIF(${CMAKE_SYSTEM_PROCESSOR} MATCHES "x86_64")






# build the nodelet version
add_library(velodyne_driver_nodelet nodelet.cc driver.cc)
add_dependencies(velodyne_driver_nodelet velodyne_driver_gencfg)
target_link_libraries(velodyne_driver_nodelet
  velodyne_input
  ${catkin_LIBRARIES}
  ${libpcap_LIBRARIES}
)

# install runtime files
install(TARGETS velodyne_node
        RUNTIME DESTINATION ${CATKIN_PACKAGE_BIN_DESTINATION}
        COMPONENT main
)
install(TARGETS velodyne_driver_nodelet
        LIBRARY DESTINATION ${CATKIN_PACKAGE_LIB_DESTINATION}
)
