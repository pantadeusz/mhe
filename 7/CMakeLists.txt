cmake_minimum_required(VERSION 3.5)

project(hello-catch2-cmake LANGUAGES CXX VERSION 0.0.1)
set(CMAKE_CXX_FLAGS "-std=c++17 -Wall -Wextra")
include_directories("${PROJECT_SOURCE_DIR}/src")
include_directories("${PROJECT_SOURCE_DIR}/thirdparty")
file(GLOB_RECURSE app_SOURCES "src/*.cpp" "src/*/*.cpp")
# file(GLOB_RECURSE test_SOURCES "test/*.cpp")

add_executable(salesman ${app_SOURCES})

