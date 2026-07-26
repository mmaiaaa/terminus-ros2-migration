cmake_minimum_required(VERSION 3.8)
project(anbot_chassis_protocol)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

find_package(ament_cmake REQUIRED)

add_library(${PROJECT_NAME}
  src/wheel_protocol.cpp
  src/wheel_stream_parser.cpp
  src/chassis_can_protocol.cpp
  src/candump_parser.cpp
  src/candump_replay.cpp
)

target_include_directories(${PROJECT_NAME}
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_17)

install(
  DIRECTORY include/
  DESTINATION include
)

install(
  TARGETS ${PROJECT_NAME}
  EXPORT export_${PROJECT_NAME}
  ARCHIVE DESTINATION lib
  LIBRARY DESTINATION lib
  RUNTIME DESTINATION bin
)

ament_export_targets(export_${PROJECT_NAME} HAS_LIBRARY_TARGET)
ament_export_include_directories(include)
ament_export_libraries(${PROJECT_NAME})

if(BUILD_TESTING)
  find_package(ament_cmake_gtest REQUIRED)

  ament_add_gtest(test_wheel_protocol
    test/test_wheel_protocol.cpp
  )

  target_link_libraries(test_wheel_protocol
    ${PROJECT_NAME}
  )

  ament_add_gtest(test_wheel_stream_parser
    test/test_wheel_stream_parser.cpp
  )

  target_link_libraries(test_wheel_stream_parser
    ${PROJECT_NAME}
  )

  ament_add_gtest(test_chassis_can_protocol
    test/test_chassis_can_protocol.cpp
  )

  target_link_libraries(test_chassis_can_protocol
    ${PROJECT_NAME}
  )

  ament_add_gtest(test_candump_parser
    test/test_candump_parser.cpp
  )

  target_link_libraries(test_candump_parser
    ${PROJECT_NAME}
  )

  ament_add_gtest(test_candump_replay
    test/test_candump_replay.cpp
  )

  target_link_libraries(test_candump_replay
    ${PROJECT_NAME}
  )
endif()

ament_package()
