cmake_minimum_required(VERSION 3.0.0 FATAL_ERROR)

project (mhedemo CXX)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")
set(CMAKE_CXX_FLAGS_DEBUG "-ggdb")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")

include_directories("${PROJECT_SOURCE_DIR}/src")

file(GLOB_RECURSE mhedemo_SOURCES "src/*.cpp")

add_executable(mhedemoapp ${mhedemo_SOURCES})

add_custom_command(TARGET mhedemoapp PRE_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_directory
  ${CMAKE_SOURCE_DIR}/data $<TARGET_FILE_DIR:mhedemoapp>/data)
