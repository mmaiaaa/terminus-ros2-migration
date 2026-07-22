cmake_minimum_required(VERSION 3.8)
project(anbot_chassis_protocol)

find_package(ament_cmake REQUIRED)

add_library(anbot_chassis_protocol
    src/wheel_protocol.cpp
    src/wheel_stream_parser.cpp
)

target_include_directories(anbot_chassis_protocol PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

target_compile_features(anbot_chassis_protocol PUBLIC cxx_std_17)

target_compile_options(anbot_chassis_protocol PRIVATE
    -Wall
    -Wextra
    -Wpedantic
)

install(
    TARGETS anbot_chassis_protocol
    EXPORT export_anbot_chassis_protocol
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
)

install(
    DIRECTORY include/
    DESTINATION include
)

ament_export_targets(export_anbot_chassis_protocol HAS_LIBRARY_TARGET)
ament_export_include_directories(include)

if(BUILD_TESTING)
    find_package(ament_cmake_gtest REQUIRED)

    ament_add_gtest(test_wheel_protocol
        test/test_wheel_protocol.cpp
    )

    target_link_libraries(test_wheel_protocol
        anbot_chassis_protocol
    )

    ament_add_gtest(test_wheel_stream_parser
        test/test_wheel_stream_parser.cpp
    )

    target_link_libraries(test_wheel_stream_parser
        anbot_chassis_protocol
    )
endif()

ament_package()