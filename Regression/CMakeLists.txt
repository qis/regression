cmake_minimum_required(VERSION 3.28 FATAL_ERROR)
project(regression DESCRIPTION "Regression" VERSION 0.1.0 LANGUAGES CXX)

configure_file(res/version.h.in ${CMAKE_CURRENT_BINARY_DIR}/src/version.h LF)

find_package(CommonLibSSE CONFIG REQUIRED)
add_commonlibsse_plugin(regression SOURCES src/main.cpp)

target_compile_features(regression PRIVATE cxx_std_23)
target_include_directories(regression PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/src)
target_include_directories(regression PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)
target_precompile_headers(regression PRIVATE src/main.hpp)

find_package(boost_algorithm REQUIRED CONFIG)
target_link_libraries(regression PRIVATE Boost::algorithm)

find_package(boost_json REQUIRED CONFIG)
target_link_libraries(regression PRIVATE Boost::json)

add_custom_command(TARGET regression POST_BUILD COMMAND
  ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:regression>
  ${CMAKE_SOURCE_DIR}/../SKSE/Plugins/$<TARGET_FILE_NAME:regression>)
