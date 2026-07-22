cmake_minimum_required(VERSION 3.8)
project(anbot_chassis_transport)

find_package(ament_cmake REQUIRED)

add_library(anbot_chassis_transport
    src/mock_transport.cpp
    src/serial_transport.cpp
)

target_include_directories(anbot_chassis_transport PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

target_compile_features(anbot_chassis_transport PUBLIC cxx_std_17)

target_compile_options(anbot_chassis_transport PRIVATE
    -Wall
    -Wextra
    -Wpedantic
)

install(
    TARGETS anbot_chassis_transport
    EXPORT export_anbot_chassis_transport
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
)

install(
    DIRECTORY include/
    DESTINATION include
)

ament_export_targets(
    export_anbot_chassis_transport
    HAS_LIBRARY_TARGET
)

ament_export_include_directories(include)

if(BUILD_TESTING)
    find_package(ament_cmake_gtest REQUIRED)

    ament_add_gtest(test_mock_transport
        test/test_mock_transport.cpp
    )

    target_link_libraries(
        test_mock_transport
        anbot_chassis_transport
    )

    ament_add_gtest(test_serial_transport
        test/test_serial_transport.cpp
    )

    target_link_libraries(
        test_serial_transport
        anbot_chassis_transport
        util
    )
endif()

ament_package()