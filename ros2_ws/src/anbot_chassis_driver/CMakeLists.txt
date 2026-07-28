cmake_minimum_required(VERSION 3.8)
project(anbot_chassis_driver)

find_package(ament_cmake REQUIRED)
find_package(anbot_chassis_protocol REQUIRED)
find_package(anbot_chassis_transport REQUIRED)

add_library(anbot_chassis_driver
    src/chassis_driver.cpp
    src/chassis_can_driver.cpp
)

target_include_directories(anbot_chassis_driver PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

target_compile_features(anbot_chassis_driver PUBLIC cxx_std_17)

target_compile_options(anbot_chassis_driver PRIVATE
    -Wall
    -Wextra
    -Wpedantic
)

ament_target_dependencies(
    anbot_chassis_driver
    anbot_chassis_protocol
    anbot_chassis_transport
)

install(
    TARGETS anbot_chassis_driver
    EXPORT export_anbot_chassis_driver
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
)

install(
    DIRECTORY include/
    DESTINATION include
)

ament_export_targets(
    export_anbot_chassis_driver
    HAS_LIBRARY_TARGET
)

ament_export_include_directories(include)

ament_export_dependencies(
    anbot_chassis_protocol
    anbot_chassis_transport
)

if(BUILD_TESTING)
    find_package(ament_cmake_gtest REQUIRED)

    ament_add_gtest(test_chassis_driver
        test/test_chassis_driver.cpp
    )

    target_link_libraries(
        test_chassis_driver
        anbot_chassis_driver
    )

    ament_target_dependencies(
        test_chassis_driver
        anbot_chassis_protocol
        anbot_chassis_transport
    )

    ament_add_gtest(test_chassis_can_driver
        test/test_chassis_can_driver.cpp
    )

    target_link_libraries(
        test_chassis_can_driver
        anbot_chassis_driver
    )

    ament_target_dependencies(
        test_chassis_can_driver
        anbot_chassis_protocol
        anbot_chassis_transport
    )
endif()

ament_package()